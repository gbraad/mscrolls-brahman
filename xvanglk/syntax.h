
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__syntax)
#define __syntax

/*************************/
/* External declarations */
/*************************/

extern int32_t       nr_of_words;   /* fileio.c */
extern wordTable *word_table;   /* dirs.c   */
extern verbDir   *verb_dir;     /* fileio.c */
extern dirInfo   *loc_dir;      /* dirs.c   */
extern dirInfo   *obj_dir;      /* dirs.c   */

extern char      *outputline;   /* init.c   */

extern void Output(char*);                    /* output.c   */
extern int32_t  StringToNum(char*, int32_t*);         /* trnslate.c */
extern int32_t  IsLocId(int32_t);                     /* execute.c  */
extern int32_t  IsObjId(int32_t);                     /* execute.c  */
extern void PrintWord(int32_t);                   /* output.c   */
extern void PrintSysDescr(sysDescr*);         /* output.c   */
extern void PrintParsedInput(parsedInput*);   /* debug.c    */
extern void PrintUsrActionRec(usrActionRec*); /* debug.c    */

#endif
