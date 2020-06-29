#include <stdio.h>

main()
{
    register c, last_c=0;

    while ((c = getchar()) != EOF) {
	if (c == ' ' && last_c == ' ') continue;
	else {
	    putchar(c);
	    last_c = c;
	}
    }
}
