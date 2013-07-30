/*
 *  mod_zipread.c -- Apache zipread module
 *
 * Apache2 API compliant module, version 0.1 (2003)
 *
 * Copyright (c) 2003 Benoit Artuso, All rights reserved.
 * Distributed under GNU GENERAL PUBLIC LICENCE see copying for more information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *
 * This module permits to browse ZIP archive and to read contents of a file
 *
 * This version use ZZIPLIB
 *
 */

#include <assert.h>

#include <zzip/zzip.h>

#include "httpd.h"
#include "http_request.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_log.h"
#include "ap_config.h"
#include "apr_strings.h"

#include "util_script.h"

// module prototype
module AP_MODULE_DECLARE_DATA zipread_module;

// our private per-directory config structure
typedef struct zipread_config_struct
{
    apr_array_header_t *index_names;
} zipread_config_rec;

static const char *zipread_addindex(cmd_parms *cmd, void *dummy, const char *arg)
{
    zipread_config_rec *d = dummy;

    if (!d->index_names) {
	d->index_names = apr_array_make(cmd->pool, 2, sizeof(char *));
    }
    *(const char **)apr_array_push(d->index_names) = arg;
    return NULL;
}

static const command_rec zipread_cmds[] =
{
    AP_INIT_ITERATE("ZipReadDirIndex", zipread_addindex, NULL, OR_INDEXES, "a list of file names to handle as indexes"),
    { NULL }
};

// allocate new private config structure
static void *create_zipread_config(apr_pool_t *p, void *s)
{
    zipread_config_rec *n = apr_pcalloc(p, sizeof(zipread_config_rec));
    n->index_names = NULL;
    return (void*)n;
}

// merge two private config structures
static void *merge_zipread_configs(apr_pool_t *p, void *basev, void *addv)
{
    assert(basev);
    assert(addv);

    zipread_config_rec *n = apr_pcalloc(p, sizeof(zipread_config_rec));

    zipread_config_rec *base = (zipread_config_rec *) basev;
    zipread_config_rec *add = (zipread_config_rec *) addv;

    n->index_names = add->index_names ? add->index_names : base->index_names;

    return (void*)n;
}

// zipread_getcontenttype : given a file name, try to find the content-type associated
// Note : if ap_sub_req_lookup_file is given a filename without '/', it will NOT try to locate it on disk

static char* zipread_getcontenttype(request_rec *r, const char *pi)
{
    request_rec *newRequest;
    char *pc = apr_pstrdup(r->pool, pi);
    char *p;

    if ((p = ap_strrchr(pc, '/')))
	p++;
    else
	p = pc;

    newRequest = ap_sub_req_lookup_file(p, r, NULL);

    // ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread subreq mime: %s", newRequest->content_type);

    if (newRequest->content_type)
	return (char*)newRequest->content_type;

    return "text/plain";
}

// zipread_showfile : send a file (with its correct content-type) to the browser

static int zipread_showfile(request_rec *r, const char *fname)
{
    char *zipfile, *name;
    ZZIP_DIR *dir;
    unsigned int itnum;

    if (!r->path_info) return HTTP_NOT_FOUND;

    zipfile = r->filename;

    if (!fname || !*fname)
    {
	name = apr_pstrdup(r->pool, r->path_info);
    }
    else
    {
	name = apr_pstrcat(r->pool, r->path_info, fname, NULL);
    }

    r->content_type = zipread_getcontenttype(r, name);
    if (*name == '/') name++;

    // ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread showfile: %s - %s - %s", zipfile, fname, name);

    for(itnum = 0; itnum < 5; itnum++)
    {
	dir = zzip_dir_open(zipfile, 0);
	if (dir)
	{
	    ZZIP_STAT st;

	    // fetch stat info of filename, before opening it
	    if (zzip_dir_stat(dir, name, &st, 0) != 0)
	    {
		// check if a directory entry is available for that name.
		name = apr_pstrcat(r->pool, name, "/", NULL);
	    
		if (zzip_dir_stat(dir, name, &st, 0) != 0)
		{
		    zzip_dir_close(dir);

		    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread showfile stat failed: %d - %s",
				  zzip_error(dir), zzip_strerror(zzip_error(dir)));

		    return HTTP_NOT_FOUND;
		}

		// found a directory entry, do an external redirect to get the
		// links in the directory listing right.
	    
		name = apr_pstrcat(r->pool, r->uri, "/", NULL);

		apr_table_setn(r->headers_out, "Location", name);

		// ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread showfile directory entry.");

		return HTTP_MOVED_PERMANENTLY;
	    }

	    ap_set_content_length(r, st.st_size);

	    // cannot check last-modified date of the file itself here, because
	    // zziplib doesnt extract it. instead we use the zip file's date
	    r->mtime = r->finfo.mtime;
	    ap_set_last_modified(r);

	    if (!r->header_only)
	    {
		ZZIP_FILE *fp = zzip_file_open(dir, name, 0);
		if (fp)
		{
		    int len;
		    char buf[32769];
		    while ((len = zzip_file_read (fp, buf, 32768)))
		    {
			ap_rwrite(buf, len, r);
		    }
		    zzip_file_close(fp);

		    zzip_dir_close(dir);
		    return OK;
		}
		else
		{
		    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread showfile file open failed: %d - %s.",
				  zzip_error(dir), zzip_strerror(zzip_error(dir)));

		    if (zzip_dir_stat(dir, name, &st, 0) != 0)
		    {
			ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread showfile after stat failed: %d - %s",
				      zzip_error(dir), zzip_strerror(zzip_error(dir)));

			break;
		    }

		    zzip_dir_close(dir);

		    continue;
		}
	    }

	    zzip_dir_close(dir);
	    return OK;
	}
	else
	{
	    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread showfile zip file not open.");
		
	    return HTTP_NOT_FOUND;
	}
    }

    zzip_dir_close (dir);

    return HTTP_NOT_FOUND;
}

// zipread_cmp : helper function for use with qsort

static int zipread_cmp(const void *c1, const void *c2)
{
    return strcmp(*(char **)c1,*(char **)c2);
}

// zipread_showheader : send the first line of the directory index to the browser

static void zipread_showheader(request_rec * r, char *fn)
{
    ap_rputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n<html>\n<head>\n",r);
    ap_rprintf(r,"<title>Index of %s</title>\n", fn);
    ap_rputs("</head>\n<body bgcolor=\"#ffffff\" text=\"#000000\">\n\n<table><tr><td bgcolor=\"#ffffff\" class=\"title\">\n<font size=\"+3\" face=\"Helvetica,Arial,sans-serif\">\n",r);
    ap_rprintf(r,"<b>Index of %s</b></font>\n", fn);
    ap_rputs("</td></tr></table><pre>",r);
}

// zipread_showentry : show one line in the directory listing

static void zipread_showentry(request_rec *r, const char *filepath, const char *filename, int dir_found, int pi_start)
{
    if (dir_found)
    {
	ap_rputs("<img src=\"/icons/folder.gif\" alt=\"[DIR]\" />",r);
    }
    else
    {
	// there has to be a better way to do this
	char *ex = ap_strrchr(filename, '.');

	if (ex && (strcmp(ex,".htm") == 0 || strcmp(ex,".html") == 0))
	{
	    ap_rputs("<img src=\"/icons/text.gif\" alt=\"[TXT]\" />",r);
	}
	else
	{
	    ap_rputs("<img src=\"/icons/unknown.gif\" alt=\"[UNK]\" />",r);
	}
    }
    ap_rprintf (r, "<a href=\"%s/%s\">%s</a>\n",
		apr_pstrndup(r->pool, r->uri, pi_start), filepath, filename);
}

// zipread_showlist : show the directory contents. filter is the sub-directory.

static int zipread_showlist(request_rec *r, const char *filter)
{
    const char *filename = r->filename;
    const char *pathinfo = r->path_info;
    unsigned int len_filter = filter ? strlen(filter) : 0;

    ZZIP_DIRENT dirent;
    int pi_start;
    char *ppdir;
    apr_array_header_t * arr = apr_array_make(r->pool, 0, sizeof(char *));

    // open the zip file
    ZZIP_DIR *dir = zzip_dir_open(filename, 0);
    if (!dir) return HTTP_NOT_FOUND;

    r->content_type = "text/html";
    zipread_showheader(r, r->uri);

    if (filter && *filter == '/') {
	filter++;
	len_filter--;
    }

    // figure out the parent directory
    ppdir = apr_pstrdup(r->pool, "");
    if (pathinfo && strlen(pathinfo) >= 2)
    {
	int i;
	for (i = strlen(pathinfo)-2 ; i >= 1 ; i--)
	{
	    if (pathinfo[i] == '/')
	    {
		ppdir = apr_pstrndup(r->pool, pathinfo, i);
		break;
	    }
	}
    }
    else
    {
	// the parent dir is outside of the zip file.
	ppdir = "/..";
    }

    // find the start of pathinfo in r->uri
    if (pathinfo)
	pi_start = ap_find_path_info(r->uri, r->path_info);
    else
	pi_start = strlen(r->uri);

    ap_rprintf(r,"<img src=\"/icons/back.gif\" alt=\"[BCK]\" /><a href=\"%s%s/\">%s</a>\n",
	       apr_pstrndup(r->pool, r->uri, pi_start), ppdir, "Parent Directory");

    while ( zzip_dir_read(dir, &dirent) )
    {
	if (!filter || strncmp(dirent.d_name, filter, len_filter) == 0)
	{
	    char **n = apr_array_push(arr);
	    *n = apr_pstrdup(r->pool, dirent.d_name);
	}
    }

    {
	char **list = (char **)arr->elts;
	char *old = "";
	int i;

	// Sort the list of files we contructed
	qsort((void *)list, arr->nelts, sizeof(char *), zipread_cmp);

	// Show the list of files: get first path part and remove the duplicates
	if (1 && filter)
	{
	    for (i = 0; i < arr->nelts; i++)
	    {
		int dir_found = 0;
		char *p1;

		// cut off anything after the first /
		if ((p1 = strchr(list[i] + len_filter, '/')))
		{
		    dir_found = 1;
		    *(p1+1) = '\0';
		}

		if (strcmp(list[i], old) != 0)
		{
		    if (list[i][strlen(list[i])-1] == '/')
		    {
			dir_found = 1;

			// skip the base filter directory entry
			if (strcmp(list[i], filter) == 0) continue;
		    }

		    zipread_showentry(r, list[i], list[i] + len_filter, dir_found, pi_start);

		    old = apr_pstrdup(r->pool, list[i]);
		}
	    }
	}
	else
	{
	    // just list all paths unfiltered
	    for (i = 0; i < arr->nelts; i++)
	    {
		int dir_found = 0;

		if (list[i][strlen(list[i])-1] == '/')
		{
		    dir_found = 1;
		}

		zipread_showentry(r, list[i], list[i], dir_found, pi_start);
	    }
	}
    }

    ap_rputs("<hr /></pre>mod_zipread on Apache 2.0 (built on "__DATE__ " " __TIME__ ")\n</body></html>", r);
    zzip_dir_close (dir);

    return OK;
}

// zipread_handler : generate a html page OR to send the file requested

static int zipread_handler(request_rec * r)
{
    const char *pathinfo = r->path_info;
    const char *filter = NULL;

    if (strcmp(r->handler, "zipread"))
	return DECLINED;

    // ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_zipread handling request for: %s -- %s", r->path_info, r->args);

    // We must decide what to do : show the directory, show a file

    if (pathinfo && pathinfo[0] && pathinfo[0] == '/')
    {
	int len = strlen(pathinfo);
	if (len == 0 || pathinfo[len - 1] == '/')
	{
	    zipread_config_rec *cfg = (zipread_config_rec*)ap_get_module_config(r->per_dir_config, &zipread_module);
	    filter = apr_pstrdup(r->pool, pathinfo+1);
	    if (cfg->index_names && cfg->index_names->nelts > 0)
	    {
		int i;
		char **list = (char **)cfg->index_names->elts;
		for (i=0; i < cfg->index_names->nelts; i++)
		{
		    if (zipread_showfile(r, list[i]) == OK) return OK;
		}
	    }
	    else
	    {
		if ((zipread_showfile(r,"index.html") == OK) || (zipread_showfile(r,"index.htm") == OK))
		    return OK;
	    }

	    // show listing of this directory
	    filter = pathinfo;
	}
	else
	{
	    return zipread_showfile(r, NULL);
	}
    }
    else
    {
	// No path was given to fetch from the zipfile. We decided here whether
	// to display the zip's root index or allow download of the zip.

	if (r->args && (strcmp(r->args, "download") == 0 || strcmp(r->args,"dl") == 0))
	{
	    // dont handle request, let it fall through to the file handler
	    return DECLINED;
	}

	// we are supposed to show a listing, but if the zip file is not
	// displayed with a / at the end, the links will be wrong.
	if (pathinfo[0] != '/')
	{
	    char *name = apr_pstrcat(r->pool, r->uri, "/", NULL);

	    apr_table_setn(r->headers_out, "Location", name);
	    return HTTP_MOVED_PERMANENTLY;
	}

	if (r->args && (strcmp(r->args, "full") == 0 || strcmp(r->args,"all") == 0))
	{
	    // show complete listing of zip file
	    filter = NULL;
	}
	else
	{
	    // see if we can find an index file in the zip's root
	    zipread_config_rec *cfg = (zipread_config_rec*)ap_get_module_config(r->per_dir_config, &zipread_module);
	    filter = apr_pstrdup(r->pool, pathinfo+1);
	    if (cfg->index_names)
	    {
		int i;
		char **list = (char **)cfg->index_names->elts;
		for (i=0; i < cfg->index_names->nelts; i++)
		    if (zipread_showfile(r, list[i]) == OK) return OK;
	    }
	    else
	    {
		if ((zipread_showfile(r,"index.html") == OK) || (zipread_showfile(r,"index.htm") == OK))
		    return OK;
	    }

	    // else show zip's root directory listing
	    filter = "";
	}
    }

    if (r->header_only) return OK;

    return zipread_showlist(r, filter);
}

static void zipread_register_hooks (apr_pool_t *p)
{
    ap_hook_handler(zipread_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA zipread_module =
{
    STANDARD20_MODULE_STUFF,
    create_zipread_config,	/* create per-dir    config structures */
    merge_zipread_configs,	/* merge  per-dir    config structures */
    NULL,			/* create per-server config structures */
    NULL,			/* merge  per-server config structures */
    zipread_cmds,		/* table of config file commands       */
    zipread_register_hooks	/* register hooks                      */
};
