#include <sys/types.h>
#include <sys/stat.h>

main(argc, argv)
char **argv;
{
	struct stat stbuf;
	register n;

	if (stat(argv[1], &stbuf)) {
		perror(argv[1]);
		exit(-1);
	}
	n = (stbuf.st_size + 255)/256;
	printf(".define TREEPAGE = %02xh\n", n);
}
