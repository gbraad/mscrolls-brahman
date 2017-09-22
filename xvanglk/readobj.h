
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__readobj)
#define __readobj

/*************/
/* Externals */
/*************/

extern FILE *datafile;

extern char *outputline;   /* init.c   */

extern void          Output(char*);                      /* output.c  */
extern void          PrintCompActionRec(compActionRec*); /* debug.c   */
extern void          PrintObject(objectInfo*);           /* debug.c   */
extern int32_t       GetNextCode32(int32_t*);            /* fileio.c  */
extern descrInfo     *ReadDescr(int32_t*);
extern triggerInfo   *ReadTrigger(int32_t*);
extern compActionRec *ReadActionRec(int32_t*);
extern int32_t       IsDescrId(int32_t);                 /* execute.c */
extern int32_t       IsTriggId(int32_t);                 /* execute.c */

#endif
