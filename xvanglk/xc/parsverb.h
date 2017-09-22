
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__parsverb)
#define __parseverb

/*************************/
/* External declarations */
/*************************/

extern int32_t  line_num;
extern FILE     *datafile;
extern verbDir  *verb_dir;

extern void     PrintActionRec(actionRec*);
extern char     *GetNextWord(int32_t*,int32_t, int32_t, FILE**);
extern int32_t  LookUpId(char*);
extern int32_t  ParseSentence(char*, actionRec*);
extern void     InitVerbActionRec(actionRec*);
extern int32_t  StoreActionRec(actionRec*);
extern int32_t  IsIntAct(int32_t);
extern int32_t  ParseAssignment(char**, int32_t*, int32_t*, int32_t*, int32_t, FILE**);
extern int32_t  ParseIfThenElse(char**, int32_t*, int32_t, int32_t, FILE**);
extern int32_t  ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**);
extern int32_t  StoreKeyword(int32_t);
extern int32_t  StoreFun(int32_t*, int32_t);
extern void     Errhdr(void);          /* output.c */

#endif
