#include	<stdio.h>
#include	<strings.h>

#define NIL		((tree) 0)
#define TRUE	1
#define FALSE	0

#define  tree struct node *
struct node {
	char *name;
	tree sibling;
	tree child;
};
typedef struct node node;

static int files = FALSE;
char *prog;

char *checkalloc(n)
unsigned n;
{
	char *malloc();
	char *result;

	if ((result = malloc(n)) == 0) {
		errs(prog, ": not enough memory\n");
		exit(1);
	}
	return(result);
}

tree cell(name, sibling, child)
char *name;
tree sibling;
tree child;
{
	tree t;

	t = (tree) checkalloc(sizeof(node));
	strcpy(t->name = checkalloc((unsigned) strlen(name)+1), name);
	t->sibling = sibling;
	t->child = child;
	return(t);
}

void freetree(t)
tree t;
{
	if (t != NIL) {
		free(t->name);
		freetree(t->sibling);
		freetree(t->child);
		free((char *) t);
	}
}

char	*strcpy(), *strcat();

struct {
	unsigned char left,right;
}	huftree[256];

main(argc, argv)
char	*argv[];
{
	FILE	*fp;
	FILE	*fq;
	int	n, i;
	char	lines[80];
	tree	maketree();

	n = filelength(argv[1])/2 - 1;
	if (n > 255) n=254;
	fp = fopen("text.tree.left", "r");
	fq = fopen("text.tree.right", "r");
	for (i=0; i<=n; i++) {
		huftree[i].left = getc(fp);
		huftree[i].right = getc(fq);
	}
	treep(cell("root", NIL, maketree(0)), 132);
	putchar('\n');
	return;
}

tree
maketree(p)
int	p;
{
	char	*printable();
	char	*s = malloc(10);

	if (huftree[p].right == -1) {
		sprintf(s, "`%s'", printable(huftree[p].left));
		return cell(s, NIL, NIL);
	}
	return cell(".", cell(".", NIL,
		maketree(huftree[p].left)),
		maketree(huftree[p].right));
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
	if (c & 0x80) {
		sprintf(s, "0x%x", c&0xff);
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
