#include <stdio.h>

char *tokens[128], *malloc();

int n_tokens = 0;

char fasttokens[] = { 'e', ' ', 'r', 't', 'o', 'n', 's' };

read_tokens(name)
char *name;
{
	register FILE *fp;
	register i=0;
	char s[100];

	fp = fopen(name, "r");
	if (!fp) {
		perror(name);
		exit(-1);
	}

	while (getline(s, fp)) {
		tokens[i] = malloc(100);	/* should be enuf */
		strcpy(tokens[i], s);
		i++;
	}
	n_tokens = i;
}

static
getline(s, fp)
register char *s;
register FILE *fp;
{
	register c;

	while ((c = getc(fp)) != '\n' && c != EOF) *s++ = c;
	*s = 0;
	if (c == EOF) return 0;
	return 1;
}
