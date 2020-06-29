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
			if ((c = getchar()) == ' ') c = getchar();	/* skip a space */
			if (isupper(c)) putchar(tolower(c));
			else putchar(c);
			break;
		case ',' :
		case ':' :
		case ';' :
			putchar(c);
			if ((c = getchar()) == ' ') c = getchar();	/* skip a space */
			putchar(c);
			break;
		default :
			putchar(c);
		}
}
