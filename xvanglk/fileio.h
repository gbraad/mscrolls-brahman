
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__fileio)
#define __fileio

#define MAX_VERBS_IN_MEM    10
#define MAX_LOCS_IN_MEM     10
#define MAX_OBJS_IN_MEM     10


/*************/
/* externals */
/*************/

extern FILE    *datafile;
extern dirData dirs;

extern char*   outputline;  /* output.c */

/********************************/
/* Extern function declarations */
/********************************/

extern uint16_t     htons(uint16_t);                  /* endian.c   */
extern uint16_t     ntohs(uint16_t);                  /* endian.c   */
extern uint32_t     htonl(uint32_t);                  /* endian.c   */
extern uint32_t     ntohl(uint32_t);                  /* endian.c   */
extern uint64_t     htonll(uint64_t);                 /* endian.c   */
extern uint64_t     ntohll(uint64_t);                 /* endian.c   */

extern verbInfo     *RdVerb(int64_t);                 /* readverb.c */
extern locationInfo *ReadLocation(int64_t);           /* readloc.c  */
extern objectInfo   *ReadObject(int64_t);             /* readobj.c  */
extern int32_t      *RdTrCode(int32_t*);              /* readtrig.c */
extern void         PrintWTable(void);                /* debug.c    */
extern void         PrintVerb(verbInfo*);             /* debug.c    */
extern void         PrintLocation(locationInfo*);     /* debug.c    */
extern void         PrintObject(objectInfo*);         /* debug.c    */
extern void         Output(char*);                    /* output.c   */

extern int32_t      IsVerbId(int32_t);                 /* execute.c  */
extern int32_t      IsLocId(int32_t);                  /* execute.c  */
extern int32_t      IsObjId(int32_t);                  /* execute.c  */
extern int32_t      IsCTriggId(int32_t);               /* execute.c  */

/* Special Ids for save() and restore() functions */
extern int32_t action;                                 /* letsplay.c */
extern int32_t prepos;                                 /* letsplay.c */
extern int32_t direction;                              /* letsplay.c */
extern int32_t active_entity;                          /* letsplay.c */
extern int32_t curr_loc;                               /* letsplay.c */
extern int32_t actor;                                  /* letsplay.c */
extern int32_t subject;                                /* letsplay.c */
extern int32_t specifier;                              /* letsplay.c */
extern int32_t value;                                  /* letsplay.c */
/* End of special Ids for save() and restore() functiona */
#endif


