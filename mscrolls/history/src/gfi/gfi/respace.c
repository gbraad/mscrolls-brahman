#include <stdio.h>

char punc[] = ";:,.?!";

main()
{
	register c;

	while ((c = getchar()) != EOF) {
		if (index(punc, c)) {
			putchar(c);
			c = getchar();
			if (c)
				putchar(' ');
			putchar(c);
		} else
			putchar(c);
	}
}
