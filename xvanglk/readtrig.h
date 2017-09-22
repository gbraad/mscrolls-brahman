
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__readtrig)
#define __readtrig


/*************/
/* Externals */
/*************/

extern FILE *datafile;

extern char *outputline;  /* init.c    */

extern void    Output(char*);                       /* output.c  */
extern int32_t GetNextCode32(int32_t*);              /* fileio.c  */
extern char    *ReadString(void);                    /* fileio.c  */
extern int32_t ReadFun(int32_t*, int32_t*, int32_t); /* readfun.c */
extern int32_t IsIntAct(int32_t);                    /* execute.c */
extern int32_t IsTestFun(int32_t);                   /* execute.c */
extern int32_t IsCAttrId(int32_t);                   /* execute.c */
extern int32_t IsLAttrId(int32_t);                   /* execute.c */
extern int32_t IsTriggId(int32_t);                   /* execute.c */


#endif
