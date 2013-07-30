// $Id$ large parts from dietlibc

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

char *getenv(const char *name)
{
	int i;
	unsigned int len;

	if (!environ || !name) return NULL;

	len = strlen(name);

	for (i = 0; environ[i]; ++i)
	{
		if ( (memcmp(environ[i], name, len) == 0) && (environ[i][len] == '='))
			return environ[i] + len + 1;
	}

	return NULL;
}

int clearenv()
{
	environ = NULL;
	return 0;
}

int setenv(const char *name, const char *value, int overwrite)
{
	if (getenv(name))
	{
		if (!overwrite) return 0;
		unsetenv(name);
	}

	{
		char *c = malloc(strlen(name) + strlen(value) + 2);
		strcpy(c, name);
		strcat(c, "=");
		strcat(c, value);
		return putenv(c);
	}
}

int unsetenv(const char *name)
{
	return putenv(name);
}

int putenv(const char *string)
{
	static char **origenv = NULL;

	size_t len;
	int envc;
	int remove=0;
	char *tmp;
	const char **ep;
	char **newenv;

	if (!origenv) origenv = environ;

	if (!(tmp = strchr(string,'='))) {
		len = strlen(string);
		remove = 1;
	}
	else {
		len = tmp - string + 1;
	}

	for (envc = 0, ep = (const char**)environ; (ep && *ep); ++ep)
	{
		if (*string == **ep && memcmp(string, *ep, len) == 0) {
			if (remove) {
				for (; ep[1]; ++ep) ep[0] = ep[1];
				ep[0] = NULL;
				return 0;
			}
			*ep = string;
			return 0;
		}
		++envc;
	}

	if (tmp)
	{
		newenv = (char**)realloc(environ == origenv ? 0 : environ,
								 (envc+2) * sizeof(char*));
		if (!newenv) return -1;

		if (envc && (environ == origenv)) {
			memcpy(newenv, origenv, envc * sizeof(char*));
		}

		newenv[envc] = (char*)string;
		newenv[envc+1] = 0;
		environ = newenv;
	}

	return 0;
}
