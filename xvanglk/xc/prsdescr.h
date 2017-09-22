
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsdescr)
#define __prsdescr

#define MAX_COND_LEN		100 /* also defined in prsefun.h and prsassgn.h */


/*************/
/* Externals */
/*************/

extern int32_t     line_num;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**); /* fileio.c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);               /* tables.c */
extern int32_t  ProcLocDSys(char*, int32_t);
extern int32_t  ProcObjDSys(char*, int32_t);
extern int32_t  IsLocId(int32_t);
extern int32_t  StoreId(int32_t);
extern int32_t  StoreString(char*);
extern void     ErrHdr(void);                                                /* output.c */

#endif
