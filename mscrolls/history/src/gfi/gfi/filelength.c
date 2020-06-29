#include <sys/types.h>
#include <sys/stat.h>

filelength(file)
char *file;
{
	static struct stat stbuf;

	stat(file, &stbuf);
	return stbuf.st_size;
}
