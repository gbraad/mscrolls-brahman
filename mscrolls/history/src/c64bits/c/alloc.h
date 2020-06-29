/*************************************************************************
 *
 *	%W%
 *	%G%
 *
 *************************************************************************/

#define alloc(type)		((type *)malloc((unsigned)sizeof(type)))
#define allocstr(s)		(malloc((unsigned)(strlen(s)+1)))

char *malloc();
