
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsassgn)
#define __prsassgn

#define MAX_COND_LEN		100 /* also defined in prsefun.h and prsetrig.h*/


/*************/
/* Externals */
/*************/

extern int32_t     line_num;
extern FILE    *datafile;
extern dirData dir_data;



extern int32_t  ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**);                   /* prsefun.c  */
extern int32_t  CheckPars(int32_t*);                                                      /* prsefun.c  */
extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**);              /* fileio.c   */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);                         /* fileio.c   */
extern int32_t  IsSpecId(int32_t);                                                        /* fileio.c   */
extern int32_t  IsIntAct(int32_t);                                                        /* keyword.c  */
extern int32_t  LookUpId(char*);                                                          /* prsesntc.c */
extern int32_t  IsDirection(char*, int32_t*);                                             /* prsesntc.c */
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);                         /* tables.c   */
extern int32_t  GetAttrId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t);  /* tables.c   */ /* 10march2017 */
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);                           /* tables.c   */
extern int32_t  GetTimerId(char*, int32_t*, int32_t);                                     /* tables.c   */
extern int32_t  IsLAttrId(int32_t);                                                       /* tables.c   */
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);                            /* tables.c   */
extern int32_t  IsLDescrId(int32_t);

extern int32_t  StringToNum(char*, int32_t*);                                             /* prsetrig.c */
extern void ErrHdr(void);                                                                 /* output.c   */
#endif
