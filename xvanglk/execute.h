
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__execute)
#define __execute

/***********/
/* Defines */
/***********/

#define EVAL_ARR_SIZE   20  /* Max 20 boolean operators and      */
                            /* values (1 or 0) in one condition. */

/*************************/
/* External declarations */
/*************************/

extern verbInfo     **verbs;
extern int32_t      nr_of_locs;                /* fileio.c */
extern int32_t      nr_of_objs;                /* fileio.c */
extern int32_t      nr_of_directions;
extern int32_t      first_direction_id;
extern locationInfo **locations;
extern objectInfo   **objects;
extern dirInfo      *loc_dir;
extern dirInfo      *obj_dir;
extern int32_t      *exit_data;                /* fileio.c   */
extern int32_t      **common_trigs;            /* fileio.c   */
extern int32_t      *trigg_owners;
extern int32_t      *descr_owners;
extern timerInfo    *timers;

extern char         *outputline;               /* init.c     */

extern void         glk_tick(void);            /* glk.h      */

extern int32_t active_entity;                  /* letsplay.c */
extern int32_t curr_loc;                       /* letsplay.c */
extern int32_t subject;                        /* letsplay.c */

extern void    Output(char*);                  /* output.c   */
extern int32_t InMem(int32_t);                 /* fileio.c   */
extern int32_t Load(int32_t);                  /* fileio.c   */
extern void    SkipFun(int32_t**);             /* sysfunc.c  */
extern int32_t LookUpId(char*);                /* syntax.c   */
extern int32_t Exit(int32_t);                  /* sysfunc.c  */
extern int32_t Entrance(int32_t);              /* sysfunc.c  */
extern int32_t Move(int32_t, int32_t);         /* sysfunc.c  */
extern int32_t XeqTestFun(int32_t, int32_t**);
extern int32_t XeqIntAct(int32_t, int32_t**);  /* xeqfun.c   */
extern void    SkipIntAct(int32_t**);          /* xeqfun.c   */
extern void    PrintId(int32_t);               /* output.c   */




/***********************************/
/* System function implementations */
/***********************************/

extern void Push(int32_t);                  /* sysfunc.c */
extern int32_t  Pop(void);                  /* sysfunc.c */

extern void And(void);                  /* sysfunc.c */
extern void Or(void);                   /* sysfunc.c */
extern void Not(void);                  /* sysfunc.c */

#endif
