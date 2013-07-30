#ifndef SYS_STAT_H
#define SYS_STAT_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

	struct stat
	{
		//dev_t         st_dev;      /* Device */
		//ino_t         st_ino;      /* INode */
		mode_t        st_mode;     /* Zugriffsrechte, Art */
		//nlink_t       st_nlink;    /* Anzahl harter Links */
		//uid_t         st_uid;      /* UID des Besitzers */
		//gid_t         st_gid;      /* GID des Besitzers */
		//dev_t         st_rdev;     /* Typ (wenn INode-Gerat) */
		size_t         st_size;     /* GroBe in Bytes*/
		//unsigned long st_blksize;  /* BlockgroBe */
		//unsigned long st_blocks;   /* Allozierte Blocks */
		//time_t        st_atime;    /* Letzter Zugriff */
		//time_t        st_mtime;    /* Letzte Modifikation */
		//time_t        st_ctime;    /* Letzte Aenderung */		
	};
	
#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001
	

	int mkdir(const char* pathname, mode_t mode);
	int stat(const char* file_name, struct stat* buf);

#ifdef __cplusplus
}
#endif

#endif

