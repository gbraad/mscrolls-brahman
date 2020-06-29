#include <stdio.h>
#include <sys/file.h>

unsigned char *filebuf, *malloc();
int filelen;
char *printable();
int wrapped = 0;		/* did the indexes wrap */

struct node {
	int count;
	int ch;
	struct node *left, *right;
} *nodes[256], *nalloc();

struct {
	char left, right;
} tree[256];
int count[256];
int code[256], len[256];
int n;
int temp[24];

main(argc, argv)
char **argv;
{
	register fd;
	FILE *fleft, *fright, *findex, *fout;
	register char *p;

	if (argc != 2) {
		printf("Usage: huf file\n");
		exit(-1);
	}

	/* load up the file */
	filelen = filelength(argv[1]);
	filebuf = malloc(filelen);
	fd = open(argv[1], O_RDONLY);
	read(fd, filebuf, filelen);
	close(fd);

	/* huf it */
	fleft = fopen("text.tree.left", "w");
	fright = fopen("text.tree.right", "w");
	findex = fopen("text.indexes", "w");
	fout = fopen("text.out", "w");
	huf (fleft, fright, findex, fout);
}

huf(fleft, fright, findex, fout)
FILE *fleft, *fright, *findex, *fout;
{
	register int *ip, *iq, *ir;
	register struct node **npp, *np;
	register unsigned char *cp;
	register i, j, k, c, nodeno, finished;
	int m;
	register byte, byteno, bitno;
	int msgst;

	for (i = 0; i < 256; i++) count[i] = 0;
	cp = filebuf;
	for (i=0; i<filelen; i++) {
		if (*cp == '\n') *cp = 0;	/* zap eom chars */
		count[*cp++]++;
		}
	for (i=0, j=0;i<256;i++) if (count[i]) j++;
	if (j>128) {
	    fprintf(stderr,"huf: ERROR, %d characters too many.\n",j-128);
	    exit(-1);
	    }
	ip = count; npp = nodes;
	nodeno=0;
	for (i=0; i < 256; i++, ip++)
		if (*ip != 0) {
			printf("'%s' : %d\n", printable(i), *ip);
			*npp = nalloc();
			(*npp)->ch = i;	/* these nodes will be leaves so put in chars	*/
			(*npp)->count = *ip;
			(*npp)->left = NULL;
			(*npp)->right = NULL;
			nodeno++; npp++;
		}

	/* sort the nodes (ok ok so its a bubble sort !)	*/
	for (i=0; i<nodeno; i++)
		for (j=0, npp = nodes; j<nodeno-1; j++, npp++)
			if ((*npp)->count < (npp[1])->count) {
				np = *npp;
				*npp = npp[1];
				npp[1] = np;
			}

	/* build the tree	*/
	for (i=nodeno-1 ; i>0; i--) {
		np = nalloc(); npp = nodes+i;
		/* make new node with count equal to sum of 2 smallest nodes	*/
		c = np->count = (npp[0])->count + (npp[-1])->count;
		/* point to two smallest nodes	*/
		np->left = npp[-1];
		np->right = npp[0];
		/* find place to insert new node	*/
		for (j=0, npp=nodes;
			(*npp)->count > c;
			j++, npp++);
		/* move the other nodes up to make space	*/
		for (k=i, npp = nodes+i; k>j; k--, npp--)
			npp[0] = npp[-1];
		/* and insert the new node	*/
		*npp = np;
	}
	/* now there should be one node in the array which is the root of the	*/
	/* Huffman binary tree	*/

	/* work out binary for each char	*/
	find_binary(nodes[0]);

	/* work out what the tree is in the usual form	*/
	for (i = 0; i < 128; i++) tree[i].left = tree[i].right = 0;
	build_tree(nodes[0], 0);

	/* write out tree	*/
	for (i = 255; !(tree[i].left || tree[i].right); i--) ;
	for (j = 0; j <= i; j++) {
		putc(tree[j].left, fleft);
		putc(tree[j].right, fright);
	}

	/* print out binary */
	for (i=0; i<256; i++)
		if (count[i]) {
			printf("`%s' = ", printable(i));
			bits(code[i], len[i]);
			for (j=0; j<len[i]; j++)
				printf("%d", temp[j]);
			putchar('\n');
		}

	/* and crunch text	*/
	bitno = 128; byteno = 0; byte = 0;
	msgst = 1;
	cp = filebuf;
	for (i = 0; i <= filelen; i++) {
		if (msgst) {
			if (bitno != 128) {	/* ie some some byte not written	*/
				putc(byte, fout);
				byteno++;
				bitno = 128;
				byte = 0;
			}
			if (i<filelen) putindex(byteno, findex);
			msgst = 0;
		}
		c = *cp++;
		if (!c) msgst = 1;
		bits(code[c], len[c]);
		for (j = 0; j < len[c]; j++) {
			if (temp[j]) byte |= bitno;
			bitno /= 2;
			if (bitno == 0) {
				putc(byte, fout);
				byte = 0;
				byteno++;
				bitno = 128;
			}
		}
	}
	if (bitno != 128) {
		putc(byte, fout);	/* flush last char */
		byteno++;
	}

	printf("compression rate = %04.2lf %%\n", (100.0*(filelen - byteno))/filelen);
	if (!wrapped) make_xdef("textwrap.o", "TextWrap", 0x7fff);
}

bits (k, len)
register k, len;
{
	register a, b;

	while (len != 0) {
		a = k/2; temp[--len] = k&1; k = a;
	}
}

find_binary(np)
register struct node *np;
{
	static thiscode=0, thislen=0;

	if (np->left) {
		thiscode *= 2; thiscode++; thislen++;
		find_binary(np->left);
		thiscode /= 2; thislen--;
		if (!np->right) return;
	}
	if (np->right) {
		thiscode *= 2; thislen++;
		find_binary(np->right);
		thiscode /= 2; thislen--;
		return;
	}

	code[np->ch] = thiscode;
	len[np->ch] = thislen;
}

build_tree(np, place)
register struct node *np;
{
	static first_free=1;
	register i;

	if (np->ch != -1) {	/* is this a leaf */
		tree[place].left = np->ch;
		tree[place].right = -1;
		return;
	}

	i = first_free++;	/* build the left subtree	*/
	tree[place].left = i;
	build_tree(np->left, i);

	i = first_free++;	/* build right subtree	*/
	tree[place].right = i;
	build_tree(np->right, i);
}

struct node *nalloc()
{
	struct node *np;

	np =  (struct node *)malloc(sizeof(struct node));
	np->ch = -1;	/* nodes which are not leaves have -1 as the char	*/
	return np;
}

putindex(n, fp)
register n;
register FILE *fp;
{
	static last_n= -1, index_no=0;
	n &= 0xffff;
	if (n < last_n)	{	/* index wrapped ? */
	    printf("index wrapped at $%04x (%d)\n", index_no, index_no);
	    printf("making textwrap.o...\n");
	    make_xdef("textwrap.o", "TextWrap", index_no+1);
	    wrapped = 1;
	}
	index_no++;
	last_n = n;
	putc(n>>8, fp);		/* msb */
	putc(n&0xff, fp);	/* lsb */
}

char *printable(c)
register c;
{
	static char s[10];

	if (c > 31 && c < 128) {
		s[0] = c;
		s[1] = 0;
		return s;
	}
	if (c >= 128) {
		sprintf(s, "0x%x", c);
		return s;
	}
	s[0] = '\\';
	s[2] = 0;
	switch (c) {
	case 0 :
		s[1] = '0';
		return s;
	case '\b' :
		s[1] = 'b';
		return s;
	case '\t' :
		s[1] = 't';
		return s;
	case '\n' :
		s[1] = 'n';
		return s;
	case '\r' :
		s[1] = 'r';
		return s;
	default :
		sprintf(s, "\\%03o", c);
		return s;
	}
}
