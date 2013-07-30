#include <config.h>

#include <stdio.h>

#include <sdi/log.h>

int main(int argc, char *argv[])
{
	int result = 0;
	
    for(int i = 1; i < argc; ++i) {
    	const char* filename = argv[i];
		FILE* file = fopen(filename, "r");
		if(!file) {
			fprintf(stderr, "Couldn't open file '%s'\n", filename);
			result = 1;
			continue;
		}
		
		while(feof(file) == 0) {
			char buf[64]; // small buffer, to force multiple fread calls...
			size_t res = fread(buf, 1, sizeof(buf), file);
			fwrite(buf, 1, res, stdout);
		}
		
		if(fclose(file) != 0) {
			fprintf(stderr, "Error when closing file\n");
			result = 1;
			continue;
		}
    }

	return result;
}

