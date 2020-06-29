#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#define  STDIN 0
#define  STDOUT 1
#define  TRUE -1
#define  FALSE 0
#define  NIL ((tree) 0)
#define  XNIL ((xtree) 0)
#define  YNIL ((ytree) 0)
#define  SNIL ((displaylineptr) 0)
#define  PAGELENGTH 22
#define  MAX(x,y) (((x) > (y)) ? (x) : (y))
#define  tree struct node *
#define  xtree struct xnode *
#define  ytree struct ynode *
#define  displaylineptr struct displayline *

void errs();

struct node {
	char *name;
	tree sibling;
	tree child;
};
typedef struct node node;
struct xnode {
	char  *name;
	xtree sibling;
	xtree child;
	xtree youngest;
	int   nchild;
	int   width;
	int   stwidth;
	int   midpoint;
	char  connector;
};
typedef struct xnode xnode;
struct ynode {
	char  *name;
	ytree sibling;
	ytree child;
	int   length;
};
typedef struct ynode ynode;
struct displayline {
	char *info;
	displaylineptr next;
};
typedef struct displayline displayline;

char *nullstring = "";
int  count;
int  pause;
int  termwidth;

char *myalloc(n)
unsigned n;
{
	char *malloc();
	char *result;

	if ((result = malloc(n)) == 0) {
		errs("not enough memory\n");
		exit(1);
	}
	return(result);
}

void newline()
{
	if (pause && ++count % PAGELENGTH == 0)
		while (getchar() != '\n')
			;
	else
		putchar('\n');
}

xtree makex(t)
tree t;
{
	char *myalloc();
	xtree xt;

	if (t == NIL)
		return(XNIL);
	else {
		xt = (xtree) myalloc(sizeof(xnode));
		xt->name = t->name;
		xt->sibling = makex(t->sibling);
		xt->child = makex(t->child);
		xt->youngest = XNIL;
		xt->nchild = 0;
		xt->width = 0;
		xt->stwidth = 0;
		xt->midpoint = 0;
		xt->connector = '-';
		return(xt);
	}
}

ytree makey(t)
tree t;
{
	ytree yt;

	if (t == NIL)
		yt = YNIL;
	else {
		yt = (ytree) myalloc(sizeof(ynode));
		yt->name = t->name;
		yt->sibling = makey(t->sibling);
		yt->child = makey(t->child);
		yt->length = 0;
	}
	return(yt);
}

void addchar(sp, col, c)
displaylineptr sp;
int  col;
char c;
{
	int  i;

	if (col < termwidth) {
		if (col >= strlen(sp->info))
			sp->info[col+1] = '\0';
		for (i = strlen(sp->info); i < col; i++) {
			sp->info[i] = ' ';
			sp->info[i + 1] = '\0';
		}
		sp->info[col] = c;
	}
}

width(xt)
xtree xt;
{
	int  idwidth;
	int  stwidth;

	idwidth = strlen(xt->name) + 2;
	stwidth = widthsum(xt->child, xt);
	xt->stwidth = stwidth;
	xt->width = (stwidth > idwidth) ? stwidth : idwidth;
	return(xt->width);
}

widthsum(xt2, xt)
xtree xt2;
xtree xt;
{
	if (xt2 == XNIL)
		return(0);
	else {
		++xt->nchild;
		xt->youngest = xt2;
		return(width(xt2) + widthsum(xt2->sibling, xt));
	}
}

length(yt)
ytree yt;
{
	ytree yt1;

	if (yt == YNIL)
		return(0);
	else
		if (yt->child == YNIL)
			yt->length = 1;
		else
			for (yt1 = yt->child; yt1 != YNIL; yt1 = yt1->sibling)
				yt->length += length(yt1);
		return(yt->length);
}

void padoutlist(xt, nchild, padding)
xtree xt;
int  nchild;
int  padding;
{
	int  share;

	while (nchild > 0) {
		share = padding / nchild;
		xt->width += share;
		padoutlist(xt->child, xt->nchild, (xt->width - xt->stwidth));
		padding -= share;
		--nchild;
		xt = xt->sibling;
	}
}

midpoint(xt)
xtree xt;
{
	if (xt == XNIL)
		return(0);
	else {
		switch (xt->nchild) {
		case 0:
			xt->midpoint = (xt->width - 1) / 2;
			break;
		case 1:
			xt->midpoint = midpoint(xt->child);
			break;
		default:
			xt->midpoint =
				MAX(
					(midpoint(xt->child) + xt->width -
							xt->youngest->width + midpoint(xt->youngest)) / 2,
					(strlen(xt->name) + 1) / 2
				);
		}
		return(xt->midpoint);
	}
}

void tab(n)
int  n;
{
	while (n-- > 0)
		putchar(' ');
}

void uscore(n)
int  n;
{
	while (n-- > 0)
		putchar('_');
}

void connect0(xt)
xtree xt;
{
	if (xt != XNIL) {
		if (*xt->name == '\0')
			tab(xt->width);
		else {
			if (xt->midpoint == 0)
				(void) midpoint(xt);
			switch (xt->connector) {
			case '-':
				tab(xt->midpoint);
				putchar('-');
				tab(xt->width - 1 - xt->midpoint);
				break;
			case '\\':
				tab(xt->midpoint + 1);
				putchar('\\');
				tab(xt->width - 2 - xt->midpoint);
				break;
			case '/':
				tab(xt->midpoint - 1);
				putchar('/');
				tab(xt->width - xt->midpoint);
				break;
			}
		}
	}
}

void connect1(xt)
xtree xt;
{
	int  done;

	if (xt != XNIL) {
		switch (xt->nchild) {
		case 0:
			tab(xt->width);
			break;
		case 1:
			if (xt->midpoint == 0)
				(void) midpoint(xt);
			tab(xt->midpoint);
			putchar('|');
			tab(xt->width - 1 - xt->midpoint);
			break;
		default:
			if (xt->midpoint == 0)
				(void) midpoint(xt);
			tab(done = xt->child->midpoint + 2);
			uscore(xt->midpoint - done);
			putchar('|');
			done = xt->youngest->width - xt->youngest->midpoint + 1;
			uscore(xt->width - 1 - xt->midpoint - done);
			tab(done);
			xt->child->connector = '\\';
			xt->youngest->connector = '/';
			break;
		}
	}
}

void treepline(xt)
xtree xt;
{
	int  gap;

	if (*xt->name == '\0')
		tab(xt->width);
	else {
		if (xt->midpoint == 0)
			(void) midpoint(xt);
		gap = xt->width - strlen(xt->name);
		tab(xt->midpoint - (strlen(xt->name) - 1) / 2);
		fputs(xt->name, stdout);
		tab(gap - xt->midpoint + (strlen(xt->name) - 1) / 2);
	}
}

void treeplist(xt)
xtree xt;
{
	xtree xt2;

	for (xt2 = xt; xt2 != XNIL; xt2 = xt2->sibling)
		connect0(xt2);
	newline();
	for (xt2 = xt; xt2 != XNIL; xt2 = xt2->sibling)
		treepline(xt2);
	newline();
	for (xt2 = xt; xt2 != XNIL; xt2 = xt2->sibling)
		connect1(xt2);
	newline();
}

reconnect(xt)
xtree xt;
{
	int  result;
	xtree remainder;
	xtree xt2;

	result = 0;
	while (xt != XNIL) {
		remainder = xt->sibling;
		if (xt->child == XNIL)
			xt->name = nullstring;
		else {
			result = 1;
			*xt = *xt->child;
			xt2 = xt;
			while (xt2->sibling != XNIL)
				xt2 = xt2->sibling;
			xt2->sibling = remainder;
		}
		xt = remainder;
	}
	return(result);
}

void freextree(xt)
xtree xt;
{
	if (xt != XNIL) {
		freextree(xt->sibling);
		freextree(xt->child);
		free((char *) xt);
	}
}

void freeytree(yt)
ytree yt;
{
	if (yt != YNIL) {
		freeytree(yt->sibling);
		freeytree(yt->child);
		free((char *) yt);
	}
}

void freescreen(sp)
displaylineptr sp;
{
	if (sp != SNIL) {
		freescreen(sp->next);
		free(sp->info);
		free((char *) sp);
	}
}

void htreepscr(yt, sp, col)
ytree yt;
displaylineptr sp;
int  col;
{
	ytree yt1;
	int  nchild;
	int  i;
	int  j;
	displaylineptr sp1;

	if (yt != YNIL) {
		for (i = 0; i < strlen(yt->name); i++)
			addchar(sp, col+i, yt->name[i]);
		col += strlen(yt->name) + 2;
		sp1 = sp;
		nchild = 0;
		for (yt1 = yt->child; yt1 != YNIL; yt1 = yt1->sibling) {
			++nchild;
			htreepscr(yt1, sp1, col);
			for (i = 0; i < 2 * yt1->length; i++)
				sp1 = sp1->next;
		}
		col -= 2;
		if (nchild > 0)
			if (nchild == 1) {
				addchar(sp, col, '-');
				addchar(sp, col+1, '-');
			} else {
				sp1 = sp;
				addchar(sp1, col, '+');
				addchar(sp1, col+1, '-');
				sp1 = sp1->next;
				yt1 = yt->child;
				for (i = 1; i < nchild; i++) {
					for (j = 0; j < 2 * yt1->length - 1; j++) {
						addchar(sp1, col, '|');
						sp1 = sp1->next;
					}
					addchar(sp1, col, (i < nchild - 1) ? '|' : '`');
					addchar(sp1, col+1, '-');
					sp1 = sp1->next;
					yt1 = yt1->sibling;
				}
			}
	}
}


void htreep(t)
tree t;
{
	ytree yt;
	int  height;
	int  i;
	displaylineptr screen;
	displaylineptr sp;
	char c;

	if (t != NIL) {
		yt = makey(t);
		height = length(yt) * 2 - 1;
		if (height > 0) {
			sp = screen = (displaylineptr) myalloc(sizeof(displayline));
			sp->info = myalloc((unsigned) termwidth + 1);
			sp->info[0] = '\0';
			for (i = 2; i <= height; i++) {
				sp->next = (displaylineptr) myalloc(sizeof(displayline));
				sp = sp->next;
				sp->info = myalloc((unsigned) termwidth + 1);
				sp->info[0] = '\0';
			}
			sp->next = SNIL;
			htreepscr(yt, screen, 0);
			sp = screen;
			for (i = 1; i <= height; i++) {
				fputs(sp->info, stdout);
				newline();
				sp = sp->next;
			}
			freescreen(screen);
		}
        freeytree(yt);
	}
}

void treep(t, tw)
tree t;
int  tw;
{
	xtree xt;
	int  treewidth;

	termwidth = tw;
	pause = isatty(STDIN) && isatty(STDOUT);
	if (t == NIL) {
		puts(" nil ");
		putchar('\n');
	} else {
		treewidth = width(xt = makex(t));
		if (treewidth >= termwidth) {
			freextree(xt);
			htreep(t);
		} else {
			padoutlist(xt->child, xt->nchild, (xt->width - xt->stwidth));
			count = 0;
			treepline(xt);
			newline();
			connect1(xt);
			newline();
			if (xt->child != XNIL)
				do {
					treeplist(xt->child);
				} while (reconnect(xt->child));
			freextree(xt);
		}
	}
}

void errs(s)
register char *s;
{
	fprintf(stderr, "%s\n", s);
	exit(-1);
}
