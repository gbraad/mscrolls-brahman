
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsstory)
#define __prsstory

/*************/
/* Externals */
/*************/

extern int32_t line_num;
extern int32_t total_lines;
extern int32_t nr_of_syn_verbs;
extern char    current_filename[];  /* init.c */
extern FILE    *source_file;
extern FILE    *datafile;

extern char    *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t IsValidKeyword(int32_t);                                     /* keyword.c */
extern int32_t ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**); /* fileio.c  */
extern int32_t InitVerbDir(void);
extern int32_t InitExits(void);    /* tables.c */
extern int32_t AddWord(char*, char*, int32_t, int32_t, int32_t*);
extern int32_t ParseWordGroup(char**, int32_t*, int32_t, FILE**);
extern int32_t CreateWordTable(void);
extern int32_t ParseLoc(int64_t, FILE**);
extern int32_t ParseObj(int64_t, FILE**);
extern int32_t ParseCDescrs(char**, int32_t*, FILE**);
extern int32_t ParseCTrigs(char**, int32_t*, FILE**);
extern int32_t ParseAttributes(char**, int32_t*, int32_t, FILE**);
extern int32_t ParseFlags(char**, int32_t*, int32_t, FILE**);
extern int32_t ParseVerb(int64_t, FILE**);
extern int32_t ParseTimers(char**, int32_t*, FILE**);
extern int32_t StoreString(char*);
extern void    ErrHdr(void);          /* output.c */

#endif
