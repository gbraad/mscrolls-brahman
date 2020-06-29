#include <stdio.h>

main()
{
	register c;

	while ((c = getchar()) != EOF)
		if (c == '\n') putchar(0);
		else putchar(c);
}
