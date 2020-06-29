#include <stdio.h>

extern char fasttokens[];
extern char *tokens[];

enum states { NORMAL, FAST1, FAST2 };

main()
{
	register c;
	read_tokens("tokenfile");
	while ((c = getchar()) != EOF) outchar(c);
}

outchar(c)
register c;
{
	static enum states state=NORMAL;
	static fastch;
	register i;

	switch (state) {
	case NORMAL :
		if (c == ' ')
			state = FAST1;
		else if (c & 0x80)
			printf(tokens[(c & 0x7f) -1]);
		else
			putchar(c);
		break;
	case FAST1 :
		fastch = c;
		state = FAST2;
		break;
	case FAST2 :
		if (c == ' ') {		/* really a fast token ? */
			i = lookupfast(fastch);
			state = NORMAL;
			if (i)
				printf(tokens[i-1]);
			else {
				putchar(' ');
				outchar(fastch);
				outchar(' ');
			}
		} else {
			putchar(' ');
			state = NORMAL;
			outchar(fastch);
			outchar(c);
		}
		break;
	}
}

lookupfast(c)
{
	register i;

	for (i=1; i<=7; i++)
		if (fasttokens[i-1] == c) return i;
	return 0;
}
