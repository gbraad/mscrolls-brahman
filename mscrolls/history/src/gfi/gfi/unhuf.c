#include <stdio.h>

struct tree {
	unsigned char left, right;
} tree[256];

main()
{
	FILE *fp, *fq;
	register i, c;
	register struct tree *tp;
	register bit;

	fp = fopen("text.tree.left", "r");
	fq = fopen("text.tree.right", "r");
	tp = tree;
	for (i=filelength("text.tree.left"); i; i--) {
		tp->left = getc(fp);
		tp->right = getc(fq);
		tp++;
	}
	fclose(fp); fclose(fq);

	fq = fopen("text.out", "r");
	tp = tree;	/* root */
	while ((c = getc(fp)) != EOF) {
		for (bit=128; bit > 0; bit /= 2) {
			if (c & bit)
				tp = tree + tp->left;
			else
				tp = tree + tp->right;
			if (tp->right == -1) {
				if (!tp->left) {	/* message terminator */
					putchar('\n');
					tp = tree;
					break;		/* skip rest of this byte */
				}
				putchar(tp->left);
				tp = tree;
			}
		}
	}
}
