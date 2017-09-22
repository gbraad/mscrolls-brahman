
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__timers)
#define __timers

/*************/
/* Externals */
/*************/

extern int32_t       nr_of_timers;    /* fileio.c   */
extern timerInfo *timers;         /* fileio.c   */

extern char *outputline;          /* init.c     */

extern void Output(char*);         /* output.c  */
extern int32_t  XeqTrigger(int32_t, int32_t);  /* execute.c */
extern int32_t  LookUpId(char*);       /* syntax.c  */

#endif
