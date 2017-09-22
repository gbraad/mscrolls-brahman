
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsetrig)
#define __prsetrig

#define MAX_COND_LEN		100 /* also defined in prsefun.h and prsassgn.h */


/*************/
/* Externals */
/*************/

extern int32_t     line_num;
extern FILE    *datafile;
extern dirData dir_data;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**);             /* fileio.c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);                        /* fileio.c */
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);                           /* tables.c */
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);                        /* tables.c */
extern int32_t  GetAttrId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t); /* tables.c */ /* 10march2017 */
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);                          /* tables.c */
extern int32_t  GetFlagId(char*, int32_t*, int32_t, int32_t, int32_t);                   /* tables.c */
extern int32_t  GetTriggerId(char*, int32_t*, int32_t, int32_t);                         /* tables.c */
extern void     InitActionRec(actionRec*);
extern int32_t  ParseSentence(char*, actionRec*);
extern int32_t  ParseAssignment(char**, int32_t*, int32_t*, int32_t*, int32_t, FILE**);  /*prsassgn.c */
extern int32_t  ParsePar(char**, int32_t*, int32_t*, int32_t*, int32_t, int32_t, FILE**);
extern int32_t  ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**);
extern void     ProcCData(int32_t, contData*);
extern int32_t  ProcLocDSys(char*, int32_t);
extern int32_t  ProcObjDSys(char*, int32_t);
extern int32_t  StoreKeyword(int32_t);
extern int32_t  StoreFun(int32_t*, int32_t);
extern int32_t  IsTestFun(int32_t);
extern int32_t  IsDirection(char*, int32_t*);
extern int32_t  IsIntAct(int32_t);
extern int32_t  IsLocId(int32_t);
extern int32_t  LookUpId(char*);
extern int32_t  StoreKeyword(int32_t);
extern int32_t  StoreId(int32_t);
extern int32_t  StoreString(char*);
extern int32_t  StoreInt32(int32_t);
extern int32_t  StoreActionRec(actionRec*);
extern void ErrHdr(void);                                                                /* output.c */

#endif
