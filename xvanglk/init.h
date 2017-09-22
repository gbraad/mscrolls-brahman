
/* Copyright 2016, 2017 Marnix van den Bos */


#if !defined(__init)
#define __init

#define STACK_SIZE  50        /* max 50 ints on stack.     */
                              /* Also defined in sysfunc.h */
#define DATAFILE "out.dat"    /* storyfile                 */


extern short transcript;      /* letsplay.c */

extern char *outputline;      /* init.c     */

/*************************/
/* External declarations */
/*************************/

extern int      CreateGlkWindows(void);     /* output.c   */
extern void     GetGlkChar(char*);          /* input.c    */
extern char     *Output(char*);             /* output.c   */
extern int32_t  ReadStoryInfo(storyInfo*);  /* fileio.c   */
extern int32_t  InitDirs(void);             /* fileio.c   */
extern int32_t  InitVerbs(void);            /* fileio.c   */
extern int32_t  InitLocations(void);        /* fileio.c   */
extern int32_t  InitObjects(void);          /* fileio.c   */
extern int32_t  InitAttributes(void);       /* fileio.c   */
extern int32_t  LetsPlay(void);             /* letsplay.c */
extern void CleanUp(void);                  /* cleanup.c  */
extern void     PrintString(char*);         /* output.c   */

#endif
