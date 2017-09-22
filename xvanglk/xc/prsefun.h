
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsefun)
#define __prsefun

#define MAX_COND_LEN    100  /* also defined in prsetrig.h and prsassgn.h */

/*************/
/* externals */
/*************/

extern int32_t   line_num;
extern int32_t   nr_of_words;
extern wordTable *word_table;

extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t  ParseLeftBanana(int32_t*, int32_t*);
extern int32_t  ParseRightBanana(int32_t*, int32_t*);
extern int32_t  StringToNum(char*, int32_t*);
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);
extern int32_t  GetFlagId(char*, int32_t*, int32_t, int32_t, int32_t);
extern int32_t  GetTriggerId(char*, int32_t*, int32_t, int32_t);
extern int32_t  GetAttrId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t); /* 10march2017 */
extern int32_t  GetTimerId(char*, int32_t*, int32_t);
extern int32_t  LookUpId(char*);
extern int32_t  IsLocId(int32_t);
extern int32_t  IsObjId(int32_t);
extern int32_t  IsCDescrId(int32_t);
extern int32_t  IsLDescrId(int32_t);
extern int32_t  IsCAttrId(int32_t);
extern int32_t  IsLAttrId(int32_t);
extern int32_t  IsLRelId(int32_t);
extern int32_t  IsCFlagId(int32_t);
extern int32_t  IsLFlagId(int32_t);
extern int32_t  IsCTriggId(int32_t);
extern int32_t  IsLTriggId(int32_t);
extern int32_t  IsTimerId(int32_t);
extern int32_t  IsWordId(int32_t);
extern int32_t  IsIntAct(int32_t);
extern int32_t  IsSpecId(int32_t);
extern void ErrHdr(void);          /* output.c */

#endif
