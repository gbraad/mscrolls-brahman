
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prseloc)
#define __prseloc

/*************/
/* externals */
/*************/

extern int32_t  line_num;
extern int32_t  first_direction_id;
extern int32_t  nr_of_locations;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**); /* fileio.c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);
extern int32_t  ParseDescriptions(char**, int32_t*, int32_t, FILE**);
extern int32_t  ProcLocDSys(char*, int32_t);                                 /* tables.c */
extern int32_t  ProcExit(int32_t, int32_t, int32_t);
extern int32_t  ParseFlags(char**, int32_t*, int32_t, FILE**);
extern int32_t  ParseAttributes(char**, int32_t*, int32_t, FILE**);
extern int32_t  ParseTriggers(char**, int32_t*, int32_t, FILE**);
extern int32_t  IsDirection(char*, int32_t*);
extern int32_t  StoreKeyword(int32_t);
extern void ErrHdr(void);                                                    /* output.c */

#endif
