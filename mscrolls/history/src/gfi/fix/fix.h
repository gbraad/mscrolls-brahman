#include <sys/types.h>

typedef int (*intfnp)();
typedef int intfn();


union useful {
	struct {
		u_char c1,c2,c3,c4;
		} chars;
	struct {
		u_short s1,s2;
		} shorts;
	u_long L;
	};

union signed_useful {
	struct {
		char c1,c2,c3,c4;
		} chars;
	struct {
		short s1,s2;
		} shorts;
	long L;
	};

typedef union useful M68;
typedef union signed_useful s_M68;

#define dest_B dest->chars.c1
#define dest_W dest->shorts.s1
#define dest_L dest->L

#define source_B source->chars.c1
#define source_W source->shorts.s1
#define source_L source->L

#define bits(a,b,c) (((a)>>(c)) & ((1 << ((b)-(c)+1))-1))

