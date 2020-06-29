/* tokenise the file */
#include <sys/file.h>

#define MAXMATCHES	40000

char *filebuf, *newbuf;
int filelen, newlen;
char *tokenfile="tokenfile";
char *malloc();

extern char *tokens[];
extern n_tokens;
extern char fasttokens[];

struct match {
	int tokennum;
	char *address;
};

struct match matches[MAXMATCHES];
int n_matches=0;

main(argc, argv)
char **argv;
{
	register i, fd;
	register char *p;

	/* args */
	for (i=1; i<argc && argv[i][0]=='-'; i++)
		for (p = &argv[i][1]; *p; p++)
			switch (*p) {
			case 't' :	/* tokenfile */
				tokenfile = argv[i+1];
				i++;
				break;
			}
	
	read_tokens(tokenfile);

	/* load up the file */
	filelen = filelength(argv[i]);
	filebuf = malloc(filelen);
	fd = open(argv[i], O_RDONLY);
	read(fd, filebuf, filelen);
	close(fd);

	tokenise();
	printf("tokenising saved %d bytes\n", filelen-newlen);

	/* write out tokenised file */
	fd = open(argv[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
	write(fd, newbuf, newlen);
	close(fd);
}

tokenise()
{
	register i, j;
	register matchnum;
	register struct match *mp;
	register char *filep, *outp;

	for (i=1; i<=n_tokens; i++)
		printf("token `%s' has %d matches\n", tokens[i-1], search(i));
	sortmatches();
	mp = matches;
	filep = filebuf;
	outp = newbuf = (char *)malloc(filelen);
	i=filelen;
	matchnum=0;
	while (i) {
		if (matchnum < n_matches && filep == mp->address) {	/* ahah! a token */
			if (mp->tokennum < 7) {	/* fasttoken ? */
			        if (outp[-1] == '@' &&
				    outp[-2] == ' ') *outp++ = ' ';
				*outp++ = ' ';
				*outp++ = fasttokens[mp->tokennum-1];
				*outp++ = ' ';
			} else
				*outp++ = 128+mp->tokennum;
			/* skip token in text */
			j = strlen(tokens[mp->tokennum-1]);
			filep += j;
			i -= j;
			mp++;	/* next match */
			matchnum++;
		} else {
			*outp++ = *filep++;
			i--;
		}
	}
	newlen = outp - newbuf;
}

/* returns number of matches */
search(tokennum)
register tokennum;
{
#if 1
	register i, len;
	register char *filep;
	char *oldfilep;
	register char *s = tokens[tokennum-1];
	register char *p, *tokenp;
	register inc;
	int skip[256];
	int n=0;
	len = strlen(s);

	/* initialise the skip array */
	for (i=0; i<256; i++) skip[i] = len;
	for (p=s, i=len-1; *p; p++, i--)
		skip[(*p)&0xff] = i;
	
	/* do the search */
	filep = filebuf + len - 1;
	while (filep < filebuf + filelen) {
		if (*filep == 0xff) {
			filep++;
			continue;
		}
		oldfilep = filep;
		tokenp = s + len - 1;
		i = 0;
		while (i<len) {
			i++;
			if (*filep != *tokenp)
				goto nomatch;
			filep--;
			tokenp--;
		}
		if (n_matches == MAXMATCHES) {
			printf("too many matches!!!\n");
			exit(-1);
		}
		filep++;
		matches[n_matches].tokennum = tokennum;
		matches[n_matches].address = filep;
		n_matches++;
		/* zap token in text to prevent subsequent matches */
		for (i=len; i; i--) *filep++ = 0xff;
		filep += len-1;
		n++;
		continue;

nomatch:
		inc = skip[(*filep)&0xff];	/* suss out how much to skip */
		filep += inc;
		if (filep <= oldfilep) filep = oldfilep+1;
	}
	return n;
#else
	register char *s = tokens[tokennum-1];
	register char *filep, *fileq, *tokenp;
	register n=0, i;
	register len = strlen(s);

	for (filep = filebuf; filep < filebuf+filelen-len; filep++) {
		for (fileq=filep, tokenp=s; *tokenp; )
			if (*fileq++ != *tokenp++) goto nomatch;
		/* matches */
		if (n_matches == MAXMATCHES) {
			printf("too many matches!!!\n");
			exit(-1);
		}
		matches[n_matches].tokennum = tokennum;
		matches[n_matches].address = filep;
		n_matches++;
		/* zap token in text to prevent subsequent matches */
		for (i=len; i; i--) *filep++ = 0xff;
		filep--;
		n++;
nomatch:
		continue;
	}
	return n;
#endif
}

static comp(mp, mq)
register struct match *mp, *mq;
{
	if (mp->address > mq->address)
		return 1;
	if (mp->address < mq->address)
		return -1;
	return 0;
}

sortmatches()
{
	qsort(matches, n_matches, sizeof(struct match), comp);
}
