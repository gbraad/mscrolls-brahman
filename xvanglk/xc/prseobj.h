
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prseobj)
#define __prseobj


/*************/
/* Externals */
/*************/

extern int32_t  line_num;

extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);
extern int32_t  StoreKeyword(int32_t);
extern int32_t  ParseDescriptions(char**, int32_t*, int32_t, FILE**);
extern int32_t  ProcObjDSys(char*, int32_t);
extern int32_t  LookUpId(char*);
extern int32_t  HasType(int32_t, int32_t);
/* extern int32_t  PCont(char**, int32_t*, int32_t, FILE*); */
extern int32_t  ProcCIData(int32_t, int32_t, int32_t);
extern int32_t  ProcCData(int32_t, int32_t);
extern int32_t  ParseFlags(char**, int32_t*, int32_t, FILE**);
extern int32_t  ParseAttributes(char**, int32_t*, int32_t, FILE**);
extern int32_t  ParseTriggers(char**, int32_t*, int32_t, FILE**);
extern void ErrHdr(void);          /* output.c */

#endif

