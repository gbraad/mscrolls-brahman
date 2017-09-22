
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__parsevoc)
#define __parsevoc

/*************/
/* Externals */
/*************/

extern int32_t line_num;
extern int32_t nr_of_syn_verbs;     /* tables.c */
extern int32_t nr_of_directions;    /* tables.c */
extern char    current_filename[];  /* init.c */
extern FILE    *datafile;

extern void    PrintWTable(void);
extern char    *strlwr(char*);
extern void    PrintWordList(void);        /* REMOVE THIS LINE LATER */
extern char    *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t IsValidKeyword(int32_t);   /* keyword.c */
extern int32_t ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**);  /* fileio.c */
extern int32_t GetNewWordId(int32_t*);
extern int32_t GetNewVerbId(int32_t*);
extern int32_t ProcWordInfo(wordInfo*);
extern int32_t CreateWordTable(void);
extern int32_t InitVerbDir(void);
extern int32_t ParseVerb(int64_t, FILE**);
extern void    ErrHdr(void);              /* output.c */

#endif
