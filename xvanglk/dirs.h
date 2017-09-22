
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__dirs)
#define __dirs


extern wordTable *word_table;         /* fileio.c */
extern verbDir   *verb_dir;           /* fileio.c */
extern dirInfo   *loc_dir;            /* fileio.c */
extern dirInfo   *obj_dir;            /* fileio.c */
extern int32_t       *trigg_owners;       /* fileio.c */
extern int32_t       *descr_owners;       /* fileio.c */
extern int32_t       *exit_data;          /* fileio.c */
extern int32_t       *com_loc_flags;      /* fileio.c */
extern int32_t       *com_obj_flags;      /* fileio.c */
extern int32_t       *local_flags;        /* fileio.c */
extern timerInfo *timers;             /* fileio.c */


extern int32_t ReadDirOffsets(dirData*);  /* fileio.c */
extern int32_t ReadVoc(int64_t);             /* fileio.c */
extern int32_t ReadTimers(int64_t);          /* fileio.c */
extern int32_t RdTrigOwners(int64_t);        /* fileio.c */
extern int32_t RdDescOwners(int64_t);        /* fileio.c */
extern int32_t ReadMapData(int64_t);         /* fileio.c */
extern int32_t ReadFlags(int64_t);           /* fileio.c */
extern int32_t ReadLocDir(int64_t);          /* fileio.c */
extern int32_t ReadObjDir(int64_t);          /* fileio.c */
extern int32_t RdCTriggs(int64_t);           /* fileio.c */

#endif
