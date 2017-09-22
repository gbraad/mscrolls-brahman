
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__storyinf)
#define __storyinf

/*************/
/* externals */
/*************/

extern FILE *source_file;
extern FILE *datafile;
extern int32_t  line_num;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**); /* fileio.c */
extern char *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern void ErrHdr(void);

#endif
