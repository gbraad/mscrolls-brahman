/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsedsys)
#define __prsedsys

#define SPACE    ' '
#define CR        10

/*************************/
/* External declarations */
/*************************/

extern int32_t nr_of_words;
extern int32_t line_num;
extern wordTable *word_table;
extern void ErrHdr(void);          /* output.c */

extern int32_t StringToNum(char*, int32_t*);
extern char* strlwr(char*);        /* keyword.c */

#endif
