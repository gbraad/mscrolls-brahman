
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__fileio)
#define __fileio

#define CR              (char) 10
#define SPACE           ' '
#define STRING_MARKER   '"'
#define COMMENT_MARKER  '#'


/*************/
/* externals */
/*************/

extern FILE *source_file;
extern FILE *datafile;

extern char current_filename[];

extern int32_t line_num;
extern int32_t total_lines;
extern int32_t nr_of_directions;
extern int32_t first_direction_id;

extern int32_t nr_of_verbs;        /* tables.c */
extern int32_t nr_of_syn_verbs;    /* tables.c */
extern int32_t nr_of_words;        /* tables.c */
extern int32_t nr_of_locations;    /* tables.c */
extern int32_t nr_of_objects;      /* tables.c */
extern int32_t nr_of_cflags;       /* tables.c */
extern int32_t nr_of_lflags;       /* tables.c */
extern int32_t nr_of_cattrs;       /* tables.c */
extern int32_t nr_of_lattrs;       /* tables.c */
extern int32_t nr_of_ltrigs;       /* tables.c */
extern int32_t nr_of_ldescrs;      /* tables.c */
extern int32_t nr_of_timers;       /* tables.c */

extern wordInfo     *word_list;
extern wordTable    *word_table;
extern locationData *loc_table;   /* tables.c */
extern objectData   *obj_table;   /* tables.c */
extern flagData     *flag_table;  /* tables.c */
extern attrData     *attr_table;   /* tables.c */
extern triggerData  *trigg_table; /* tables.c */
extern descrData    *descr_table; /* tables.c */
extern verbDir      *verb_dir;    /* tables.c */

extern timerInfo *md_start;
extern timerInfo *md_end;

extern int32_t *exit_table;

extern uint16_t htons(uint16_t);                                /* endian.c   */
extern uint32_t htonl(uint32_t);                                /* endian.c   */
extern uint64_t htonll(uint64_t);                               /* endian.c   */
extern int32_t  CheckIfKeyword(char*);                          /* keyword.c  */
extern int32_t  IsLocId(int32_t);                               /* tables.c   */
extern int32_t  IsLDescrId(int32_t);                            /* tables.c   */
extern int32_t  IsSpecId(int32_t);                              /* tables.c   */
extern int32_t  IsTestFun(int32_t);                             /* tables.c   */
extern int32_t  IsIntAct(int32_t);                              /* tables.c   */

extern int32_t  LookUpId(char*);                            /* prsesntc.c */
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);      /* tables.c   */
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);        /* tables.c   */
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);          /* tables.c   */
extern int32_t  GetTimerId(char*, int32_t*, int32_t);               /* tables.c   */
extern int32_t  GetAttrId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t); /* tables.c  */ /* 10march2017 */
extern void     ErrHdr(void);                               /* output.c   */

#endif


