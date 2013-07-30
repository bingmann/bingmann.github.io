#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <algorithm>

#define USE_COLOR

#ifdef USE_COLOR
#define BLUE	"\33[34m"
#define WHITE	"\33[00m"
#define RED		"\33[31m"
#else
#define BLUE	""
#define WHITE	""
#endif

// 79 for now because of buggy console
static const int CHAR_COLUMNS = 79;

struct DirEntry
{
	std::string name;

	L4_ThreadId_t	server;
	objectid_t		handle;

	inline DirEntry(const struct dirent *de)
		: name(de->d_name), server(de->d_server), handle(de->d_object)
	{		
	}
};

int main(int argc, char** argv)
{
	std::string path;
	bool listlong = false;

	for(int ai = 1; ai < argc; ai++)
	{
		if (strcmp(argv[ai], "-l") == 0 || strcmp(argv[ai], "--long") == 0)
		{
			listlong = true;
		}
		else {
			path = argv[ai];
		}
	}

	// try default if no path is given
	if (path.empty()) path = getenv("CWD");
	if (path.empty()) path = ".";

	DIR* dir = opendir(path.c_str());
	if(dir == NULL) {
		fprintf(stderr, "Couldn't open path '%s'\n", path.c_str());
		return 1;
	}
	size_t maxlen = 0;

	typedef std::vector<DirEntry> entries_t;
	std::vector<DirEntry> entries;

	struct dirent* dentry;
	while( (dentry = readdir(dir)) != NULL) {
		// printf("de: %s\n", dentry->d_name);
		entries.push_back(DirEntry(dentry));
		maxlen = std::max(maxlen, entries.back().name.size());
	}

	closedir(dir);

	if (!listlong)
	{
		maxlen++;
		int columns = CHAR_COLUMNS / maxlen;
		char entry[maxlen+1];
		int c = 0;
		struct stat statbuf;
		for(entries_t::iterator i = entries.begin(); i != entries.end(); ++i)
		{
			if(c >= columns) {
				printf("\n");
				c = 0;
			}

			std::string completepath = path;
			completepath += "/";
			completepath += i->name;

			if (stat(completepath.c_str(), &statbuf) == 0)
			{
				if(S_ISDIR(statbuf.st_mode)) {
					printf("%s", BLUE);
				} else {
					printf("%s", WHITE);
				}
			} else {
				printf("%s", RED);
			}
		
			memcpy(entry, i->name.c_str(), i->name.size());
			memset(entry + i->name.size(), 0x20, maxlen - i->name.size());
			entry[maxlen] = 0;

			printf("%s", entry);
			c++;
		}

		printf("%s\n", WHITE);
	}
	else
	{
		maxlen++;
		char entry[maxlen+1];
		struct stat statbuf;
		for(entries_t::iterator i = entries.begin(); i != entries.end(); ++i)
		{
			std::string completepath = path;
			completepath += "/";
			completepath += i->name;

			// printf("stat %s\n", completepath.c_str());

			if (stat(completepath.c_str(), &statbuf) == 0)
			{
				if(S_ISDIR(statbuf.st_mode)) {
					printf("%s", BLUE);
				} else {
					printf("%s", WHITE);
				}
			} else {
				printf("%s", RED);
				statbuf.st_size = 0;
			}
		
			memcpy(entry, i->name.c_str(), i->name.size());
			memset(entry + i->name.size(), 0x20, maxlen - i->name.size());
			entry[maxlen] = 0;

			printf("%s", entry);
			printf("%s %8d  %lx:%lx\n", WHITE, statbuf.st_size, i->server.raw, i->handle);
		}
	}

	return 0;
}

