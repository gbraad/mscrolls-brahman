
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__arithmtc)
#define __arithmtc


/*************************/
/* External declarations */
/*************************/

extern timerInfo *timers;

extern char *outputline;  /* init.c     */

extern void Output(char*);                                                       /* output.c   */
extern int32_t  GetPar(int32_t*, int32_t*, int32_t*, char**, int32_t**);         /* sysfunc.c  */
extern int32_t  GetLvaluePar(int32_t*, int32_t*, int32_t*, char**, int32_t**);   /* sysfunc.c  */
extern int32_t  GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);        /* sysfunc.c  */
extern int32_t  CheckPars(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t); /* checkpar.c */
extern int32_t  NextOpcode(int32_t**);
extern void ConvSpecId(int32_t*, int32_t*);                                      /* sysfunc.c  */

extern int32_t  IsTimerId(int32_t);

#endif
