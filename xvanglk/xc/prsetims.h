
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__prsetims)
#define __prsetims

/*************/
/* externals */
/*************/

extern int32_t  line_num;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**); /* fileio.c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
extern int32_t  StringToNum(char*, int32_t*);
extern int32_t  GetTimerId(char*, int32_t*, int32_t);
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);
extern int32_t  GetTriggerId(char*, int32_t*, int32_t, int32_t);
extern void     ProcTimerInfo(timerInfo*);
extern void     ErrHdr(void);                                                /* output.c */

#endif
