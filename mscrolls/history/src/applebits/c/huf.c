

/*************************************************************************
*
*	@(#)huf.c	1.1
*	6/24/88
*
*************************************************************************/


#include <stdio.h>

char *strcpy(), *strcat(), *mktemp();
char *template = "/tmp/hufXXXXXX";
struct {
	char left, right;
} tree[128];
int count[256], heap[128];
int code[128], len[128], dad[256];
int n;
int temp[24];
int waffle = 0, remove = 0;

main(argc, argv)
int argc;
char **argv;
{
	int i, j;
	char name[30];
	FILE *fin, *fout;

	for (i = 1; (i < argc) && (*argv[i] == '-'); i++)
		for (j = 1; argv[i][j] != '\0'; j++)
			switch (argv[i][j]) {
			case 'w':
				waffle = 1;
				break;
			case 'r':
				remove = 1;
				break;
			case 'l':
				template = ".temphufXXXXXX";
				break;
			default:
				fprintf(stderr, "huf: unknown flag %c\n", argv[i][j]);
				exit (-1);
			}

	if (i == argc) {
		huf(stdin, stdout, "");
		exit (0);
	}
	for (; i < argc; i++)
		if ((fin = fopen(argv[i], "r")) == NULL)
			fprintf(stderr, "huf: can't open %s\n", argv[i]);
		else {
			strcpy (name, argv[i]);
			strcpy (name + strlen (name), ".H");
			if ((fout = fopen(strcat(strcpy(name, argv[i]), ".H"),
				"w")) == NULL) {
				fprintf(stderr, "huf: can't open %s\n", name);
				exit(-1);
			}
			huf(fin, fout, argv[i]);
			if (remove) unlink(argv[i]);
		}
}

huf(fin, fout, fname)
FILE *fin, *fout;
char *fname;
{
	int i,j,k,m,t,x;
	int compr, byte, byteno, bitno, nodeno;
	float saving;
	char *tname, c;
	FILE *tfile;

	tname = mktemp(template);
	if ((tfile = fopen(tname, "w+")) == NULL) {
		fprintf(stderr, "huf: couldn't open temp file %s\n", tname);
		exit(-1);
	}
	if (*fname) fprintf(stderr, "In \"%s\" :\n", fname);
	for (i = 0; i < 128; i++) count[i] = 0;
	i = 0;
	while ((c = getc(fin)) != EOF) {
		putc(c, tfile);
		count[c]++;
		i++;
	}
	putc(0, tfile);
	count[0]++;
	m = i+1;
	if (waffle) fprintf(stderr, "frequencies are as follows: \n");
	n = 0;
	for (i=0; i < 128; i++)
		if (count[i] != 0) {
			if (waffle) {
				outch(i, stderr);
				fprintf(stderr, " : %d\n", count[i]);
			}
			heap[n++] = i;
		};
	n--;
	for (i = n; i >= 0; i--) pqdownheap(i);
	while (n != 0) {
		t = heap[0]; heap[0] = heap[n--];
		pqdownheap(0);
		count[128 + n] = count[t] + count[heap[0]];
		dad[t] = 128 + n;
		dad[heap[0]] = -128 - n;
		heap[0] = 128 + n; pqdownheap(0);
	}
	dad[128] = 0;
	if (waffle) fprintf(stderr, "\nBit strings are as follows :\n");
	for (k = 0; k < 128; k++)
		if (count[k] == 0) {
			code[k] = 0; len[k] = 0;
		} else {
			i = 0; j = 1; t = dad[k]; x = 0;
			while (t != 0) {
				if (t < 0) {
					x = x + j;
					t = -t;
				}
				t = dad[t];
				j = j + j;
				i++;
			}
			code[k] = x; len[k] = i;
			if (waffle) {
				outch(k, stderr);
				fprintf(stderr, " : ");
			}
			bits(x, i);
			if (waffle) for (j = 0; j < i; j++) putc('0' + temp[j], stderr);
			if (waffle) putc('\n', stderr);
		};
	if (waffle) putc('\n', stderr);
	fprintf(stderr, "original length was %d\n", m-1);
	for (i = 0; i < 128; i++) tree[i].left = tree[i].right = 0;
	for (i = 0; i < 128; i++)
		if (count[i] != 0) {
			t = i;
			while (dad[t] != 0)
				if (dad[t] < 0) {
					tree[-dad[t] - 128].left = t;
					t = -dad[t];
				} else {
					tree[dad[t] - 128].right = t;
					t = dad[t];
				}
		}
	for (i = 127; !(tree[i].left + tree[i].right); i--) ;
	putc(i, fout);
	nodeno = i+1;
	for (j = 0; j <= i; j++) {
		putc(tree[j].left, fout);
		putc(tree[j].right, fout);
	}
	bitno = 128; byteno = 0; byte = 0;
	rewind(tfile);
	for (i = 0; i <= m; i++) {
		c = getc(tfile);
		bits(code[c], len[c]);
		for (j = 0; j < len[c]; j++) {
			if (temp[j]) byte |= bitno;
			bitno /= 2;
			if (!bitno) {
				putc(byte, fout);
				byte = 0;
				byteno++;
				bitno = 128;
			}
		}
	}
	compr = 2*(nodeno+1) + byteno - 1;
	fprintf(stderr, "length of compressed file is %d\n", compr);
	saving = 100.0*((float)(m-1) - (float)compr)/(float)(m-1);
	fprintf(stderr, "saving is %2.2f %%\n", saving);
	fclose(tfile);
	unlink(tname);
}

bits (k, len)
int k, len;
{
	int a, b;

	while (len != 0) {
		a = k / 2; b = k % 2; k = a;
		if (b == 1) temp[--len] = 1;
		else temp[--len] = 0;
	}
}

pqdownheap(k)
int k;
{
	int j, v;

	v = heap[k];
	while (k <= n/2 - 1) {
		j = k + k;
		if (j < n)
			if (count[heap[j]] > count[heap[j+1]]) j++;
		if (count[v] <= count[heap[j]]) goto l0;
		heap[k] = heap[j]; k = j;
	}
   l0 : heap[k] = v;
}

outch(c, fp)
char c;
FILE *fp;
{
	c &= 127;
	if (c < 32) {
		c += 64;
		putc('^', fp);
	}
	putc(c, fp);
	return;
}
