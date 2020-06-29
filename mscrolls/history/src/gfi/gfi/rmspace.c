#include <stdio.h>
#include <ctype.h>

main()
{
	register char c, c1, c2;

	while ((c = getchar()) != EOF)
		if (index(";:,.?!", c)) {
			putchar(c);
			c1 = getchar();
			if (c1 == EOF) exit(0);
			c2 = getchar();
			if (c2 == EOF) {
				putchar(c1);
				exit(0);
			}
			if (c1 == ' ' && isalpha(c2)) putchar(c2);
			else { putchar(c1); putchar(c2); }
		} else putchar(c);
}
