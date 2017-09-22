#if !defined(__debug)
#define __debug

/*************************/
/* External declarations */
/*************************/

extern dirInfo    *loc_dir;                  /* dirs.c   */
extern dirInfo    *obj_dir;                  /* dirs.c   */
extern objectInfo **objects;                 /* fileio.c */
extern attrInfo   *c_loc_attrs;              /* fileio.c */
extern attrInfo   *c_obj_attrs;              /* fileio.c */
extern attrInfo   *local_attrs;              /* fileio.c */
extern timerInfo  *timers;                   /* fileio.c */
extern int32_t    *com_loc_flags;            /* fileio.c */
extern int32_t    *com_obj_flags;            /* fileio.c */
extern int32_t    *local_flags;              /* fileio.c */
extern int32_t    *exit_data;                /* fileio.c */
extern int32_t    nr_of_locs;                /* fileio.c */
extern int32_t    nr_of_objs;                /* fileio.c */
extern int32_t    nr_of_cflags;              /* fileio.c */
extern int32_t    loc_flags_string_len;      /* fileio.c */
extern int32_t    nr_of_words;               /* dirs.c   */
extern int32_t    nr_of_directions;          /* fileio.c */
extern int32_t    first_direction_id;        /* fileio.c */
extern int32_t    nr_of_cattrs;              /* fileio.c */
extern int32_t    nr_of_lattrs;              /* fileio.c */
extern int32_t    nr_of_timers;              /* fileio.c */
extern wordTable  *word_table;               /* dirs.c   */

extern int32_t curr_loc;                     /* letsplay.c */
extern int32_t direction;                    /* letsplay.c */
extern int32_t prepos;                       /* letsplay.c */
extern int32_t action;                       /* letsplay.c */
extern int32_t actor;                        /* letsplay.c */
extern int32_t subject;                      /* letsplay.c */
extern int32_t specifier;                    /* letsplay.c */

extern void  PrintString(char*);                       /* output.c  */
extern void  PrintNumber(int32_t);                     /* output.c  */
extern char  *outputline;                              /* init.c    */
extern void  Output(char*);                            /* output.c  */
extern int32_t IsLocId(int32_t);                       /* execute.c */
extern int32_t IsObjId(int32_t);                       /* execute.c */
extern void  PrintWord(int32_t);                       /* output.c  */
extern void  PrintId(int32_t);                         /* output.c  */
extern void  PrintExtendedSysDescr(extendedSysDescr*); /* output.c  */

#endif
