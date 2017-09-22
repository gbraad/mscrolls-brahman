#if !defined(__letsplay)
#define __letsplay


extern int32_t   nr_of_locs;  /* fileio.c */
extern int32_t   nr_of_objs;  /* fileio.c */
extern dirInfo   *obj_dir;    /* fileio.c */
extern storyInfo story_info;  /* init.c   */

extern char    *outputline;   /* init.c   */

extern void    Output(char*);                                  /* output.c   */
extern void    PrintString(char*);                             /* output.c   */
extern char    *GetInput(char*, char*);                        /* input.c    */
extern int32_t FilledOut(extendedSysDescr*);                   /* trnslate.c */
extern int32_t CountSubjectsInParsedInput(parsedInput*);       /* trnslate.h */
extern int32_t SplitSubject(parsedInput*, int32_t);            /* trnslate.c */
extern void    PrintNotFound(extendedSysDescr*);               /* trnslate.c */
extern int32_t CheckDoTimers(int32_t);                         /* timers.c   */
extern int32_t HandleTimers(void);                             /* timers.c   */
extern void    InitParsedInput(parsedInput*);                  /* syntax.c   */
extern void    PrintParsedInput(parsedInput*);                 /* debug.c    */
extern int32_t InitUsrActionRec(usrActionRec*);                /* syntax.c   */
extern int32_t PrintUsrActionRec(usrActionRec*);               /* debug.c    */
extern int32_t ParseInput(char*, parsedInput*, int32_t);       /* syntax.c   */
extern int32_t XeqPrologue(int32_t);                           /* execute.c  */
extern int32_t XeqEpilogue(int32_t);                           /* execute.c  */
extern int32_t XeqActionRec(usrActionRec*, int32_t*, int32_t); /* execute.c  */
extern int32_t XeqVerbDefault(usrActionRec*, int32_t);         /* execute.c  */
extern int32_t ContList(int32_t, int32_t*, int32_t);           /* sysfunc.c  */

extern resultStruct Translate(parsedInput*, int32_t, usrActionRec*, char*);  /* trnslate.c */

#endif
