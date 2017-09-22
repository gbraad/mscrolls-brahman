
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__readfun)
#define __readfun


/*************/
/* externals */
/*************/

extern FILE *datafile;

extern char *outputline;       /* init.c    */

extern void Output(char*);
extern int32_t  GetNextCode32(int32_t*); /* fileio.c  */
extern int32_t  IsTestFun(int32_t);      /* execute.c */
extern int32_t  IsIntAct(int32_t);       /* execute.c */
extern int32_t  IsCRelId(int32_t);       /* execute.c */
extern int32_t  IsLRelId(int32_t);       /* execute.c */
extern int32_t  ntohl(int32_t);          /* endian.c  */


#endif
