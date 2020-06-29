#include <stdio.h>

/* format :
 *
 * header: (12 bytes)
 * address of offset of root node of tree	(=12)
 * address of start of tree					(=14)
 * address of start of indexes
 * address of start of tokens
 * address of hash table for short indexes or text for long indexes
 * =0 for long indexes, !=0 = address of text
 * rest:
 * tree
 * indexes
 * tokens
 * (hash table for short tokens)
 * text
 *
 */

main()
{
	register i, j;
	register offset, n;
	register FILE *fp, *fq, *fout;
	int tree_size, num_messages, token_size, index_size, hash_size;
	int short_indexes=0;
	int hash_index[100];

	fout = fopen("text.raw", "w");
	if (!fout) {
		perror("text.raw");
		exit(-1);
	}
	tree_size = 2*filelength("text.tree.left");	/* 2 bytes per node */
	num_messages = filelength("text.indexes")/2;
	token_size = filelength("tokenfile");
	if (token_size & 1) token_size++;	/* even align */
	if (short_indexes) {
		hash_size = 2*(num_messages-1)/0x80;	/* hash on msgno/128 */
		index_size = num_messages-1;			/* 1 byte indexes */
		if (index_size & 1) index_size++;		/* even aligned */
	} else
		index_size = 2*(num_messages-1);

	/* output header */
	putshort(12, fout);
	putshort(14, fout);
	putshort(tree_size + 14, fout);	/* indexes after tree */
	putshort(index_size + tree_size + 14, fout);	/* tokens after indexes */
	if (short_indexes) {
		/* indexes of every 128 msgs */
		putshort(token_size + index_size + tree_size + 14, fout);
		/* text */
		putshort(hash_size + token_size + index_size + tree_size + 14, fout);
	} else {
		/* text */
		putshort(token_size + index_size + tree_size + 14, fout);
		putshort(0, fout);
	}

	/* offset of root of tree */
	putshort(0, fout);	/* my huffman always puts root first */

	/* the tree */
	fp = fopen("text.tree.left", "r");
	fq = fopen("text.tree.right", "r");
	while ((n = getc(fp)) != EOF) {
		putc(n, fout);
		putc(getc(fq), fout);
	}
	fclose(fp); fclose(fq);

	/* indexes */
	fp = fopen("text.indexes", "r");
	getshort(fp);	/* first index is always 0 */
	if (short_indexes) {
		offset = 0;
		j = 0;
		n = getshort(fp);				/* first !=0 index */
		while (n != EOF) {
			for (i=0; i<128; i++) {
				putc(n - offset, fout);	/* delta */
				offset = n;
				n = getshort(fp);		/* next index */
				if (n == EOF) break;
			}
			hash_index[j++] = offset;
		}
		if ((num_messages-1) & 1) putc(0, fout);	/* align */
	} else
		while ((n = getshort(fp)) != EOF)
			putshort(n, fout);
	fclose(fp);

	/* tokens */
	fp = fopen("tokenfile", "r");
	while ((n = getc(fp)) != EOF)
		if (n=='\n')
			putc(0, fout);
		else
			putc(n, fout);
	if (filelength("tokenfile") < token_size) putc(0, fout);
	fclose(fp);

	/* hash indexes (maybe) */
	if (short_indexes)
		for (i=0; i<j; i++) putshort(hash_index[i], fout);
	
	/* text */
	fp = fopen("text.out", "r");
	while ((n = getc(fp)) != EOF)
		putc(n, fout);
	fclose(fp);
}

putshort(n, fp)
FILE *fp;
{
	putc(n>>8, fp);
	putc(n, fp);
}

getshort(fp)
FILE *fp;
{
	register n;
	n = getc(fp);
	if (n==EOF) return EOF;
	n = 256*n + getc(fp);
	return n & 0xffff;
}
