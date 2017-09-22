
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__input)
#define __input

extern FILE    *transcriptfile;    /* init.c  */
extern winid_t mainwin;            /* init.c  */

extern short testmode;             /* letsplay.c */
extern short transcript;           /* letsplay.c */
extern char  *outputline;          /* output.c   */
extern FILE  *testfile;            /* init.c     */

extern void Output(char*);         /* init.c     */

#endif
