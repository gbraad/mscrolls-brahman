
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__init)
#define __init

/*************/
/* Externals */
/*************/

extern int32_t endian;
extern uint32_t htonl(uint32_t);
extern uint32_t ntohl(uint32_t);

extern int32_t  ReadStoryInfo(storyInfo*, char*);           /* storyinf.c */
extern int32_t  StoreStoryInfo(storyInfo*);                 /* storyinf.c */
extern int32_t  SetUpTables(void);                          /* tables.c   */
extern void     InitOffsets(void);                          /* tables.c   */
extern int32_t  StoreOffsets(void);                         /* fileio.c   */
extern int32_t  CheckTables(void);                          /* tables.c   */
extern int32_t  FreeTables(void);                           /* tables.c   */
extern int32_t  StoreDirs(void);                            /* fileio.c   */
extern int32_t  ParseStory(FILE*);                      /* prsstory.c */
extern int32_t  PrintWordList(void);                        /* tables.c   */
extern int32_t  CreateWordTable(void);                      /* tables.c   */
extern int32_t  LookUpId(char*);                            /* prsesntc.c */
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);
extern int32_t  GetAttrId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t);  /* tables.c */ /* 10march2017 */
extern int32_t  GetFlagId(char*, int32_t*, int32_t, int32_t, int32_t);
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);
extern int32_t  GetTriggerId(char*, int32_t*, int32_t, int32_t);
extern void     ErrHdr(void);                               /* debug.c */

#endif
