#include <stdio.h>
#include <ctype.h>

main()
{
	register c;

	while ((c = getchar()) != EOF)
		switch (c) {
		case '.' :
		case '!' :
		case '?' :
			putchar(c);
			c = getchar();
			if (c==32 || c&0x80 || isalpha(c)) {
				putchar(' ');
				if (islower(c)) putchar(toupper(c));
				else putchar(c);
			} else putchar(c);
			break;
		case ',' :
		case ':' :
		case ';' :
			putchar(c);
			c = getchar();
			if (c==32 || c&0x80 || isalpha(c)) {
				putchar(' ');
				putchar(c);
			} else
				putchar(c);
			break;
		default :
			putchar(c);
		}
}
