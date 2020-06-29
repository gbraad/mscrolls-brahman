#include <stdio.h>

struct node {
	int count;
	int ch;
	struct node *left, *right;
} *nodes[128], *nalloc();

struct {
	char left, right;
} tree[128];
int count[256];
int code[128], len[128];
int n;
int temp[24];
int wrap_value, wrap_point = -1;

huf(fin, fleft, fright, findex, fout, wrap)
FILE *fin, *fleft, *fright, *findex, *fout;
int wrap;
{
	register int *ip, *iq, *ir;
	register struct node **npp, *np;
	register i, j, k, c, nodeno, finished;
	int m;
	register byte, byteno, bitno;
	int msgst;

	wrap_value = wrap;
	for (i = 0; i < 128; i++) count[i] = 0;
	i = 0;
	while ((c = getc(fin)) != EOF) {
		count[c]++;
		i++;
	}
	m = i;
	ip = count; npp = nodes; nodeno = 0;
	for (i=0; i < 128; i++, ip++)
		if (*ip != 0) {
			printf("%c : %d\n", i, *ip);
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
	for (i = 127; !(tree[i].left || tree[i].right); i--) ;
	for (j = 0; j <= i; j++) {
		putc(tree[j].left, fleft);
		putc(tree[j].right, fright);
	}

	/* and crunch text	*/
	bitno = 1; byteno = 0; byte = 0;
	msgst = 1;
	rewind(fin);
	for (i = 0; i <= m; i++) {
		if (msgst) {
			if (bitno != 1) {	/* ie some some byte not written	*/
				putc(byte, fout);
				byteno++;
				bitno = 1;
				byte = 0;
			}
			if (i<m) putindex(byteno, findex);
			msgst = 0;
		}
		c = getc(fin);
		if (!c) msgst = 1;
		bits(code[c], len[c]);
		for (j = 0; j < len[c]; j++) {
			if (temp[j]) byte |= bitno;
			bitno *= 2;
			if (bitno == 256) {
				putc(byte, fout);
				byte = 0;
				byteno++;
				bitno = 1;
			}
		}
	}
	if (wrap) {		/* put the wrap point in the index ? */
	    rewind(findex);	/* put wrap in first two bytes */
	    putc(wrap_point >> 8, findex); /* high byte of wrap */
	    putc(wrap_point & 0xff, findex); /* low byte */
	}
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
		thiscode *= 2; thislen++;
		find_binary(np->left);
		thiscode /= 2; thislen--;
		if (!np->right) return;
	}
	if (np->right) {
		thiscode *= 2; thiscode++; thislen++;
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

	if ((i = np->left->ch) >= 0)	/* is the left subtree a leaf ?	*/
		tree[place].left = 128+i;
	else {
		i = first_free++;	/* build the left subtree	*/
		tree[place].left = i;
		build_tree(np->left, i);
	}
	if ((i = np->right->ch) >= 0)	/* right subtree a leaf ?	*/
		tree[place].right = 128+i;
	else {
		i = first_free++;	/* build right subtree	*/
		tree[place].right = i;
		build_tree(np->right, i);
	}
}

struct node *nalloc()
{
	char *malloc();
	struct node *np;

	np =  (struct node *)malloc(sizeof(struct node));
	np->ch = -1;	/* nodes which are not leaves have -1 as the char	*/
	return np;
}

putindex(n, fp)
register n;
register FILE *fp;
{
    static last_n = -1;
    static index_no = 0;
    if (wrap_value) {		/* !=0 => new (large) text */
	n %= wrap_value;	/* wrap the indexes at the specified place */
	if (n < last_n)		/* wrapped yet ? */
	    wrap_point = index_no; /* remember where it wrapped */
    }
    last_n = n;			/* remember so we can see where it wraps */
    index_no++;			/* remember how many indexes we have done */
    putc(n>>8, fp);		/* msb */
    putc(n&0xff, fp);		/* lsb */
}
