
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsesntc)
#define __prsesntc

#define SPACE  ' '


/*************************/
/* External declarations */
/*************************/

extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;
extern wordTable *word_table;
extern int32_t       nr_of_words;
extern int32_t       line_num;

extern int32_t  ScanWordTable(char*, wordTable*, int32_t, int32_t);
extern int32_t  IsLocId(int32_t);
extern int32_t  IsObjId(int32_t);
extern void ErrHdr(void);          /* output.c */


#endif
