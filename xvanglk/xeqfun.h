
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__xeqfun)
#define __xeqfun

#define DEBUGFILE      "debug.txt"
#define TESTFILE       "testinput.txt"
#define TRANSCRIPTFILE "transcript.txt"
#define SAVEFILE       "save.dat"

/*************************/
/* External declarations */
/*************************/

extern FILE *testfile;       /* for testinput  */
extern FILE *transcriptfile; /* for transcript */
extern FILE *debugfile;      /* for debug dump */

extern winid_t mainwin;      /* init.c */
extern winid_t statuswin;    /* init.c */

extern storyInfo story_info;
extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;
extern int32_t   *exit_data;
extern timerInfo *timers;

extern short testmode;       /* letsplay.c */
extern short transcript;     /* letsplay.c */
extern short debug_mode;     /* letsplay.c */

extern int32_t curr_loc;               /* letsplay.c */
extern int32_t indent;                 /* letsplay.c */

extern char *outputline;                 /* init.c     */

/* next line commented out at 8 dec 03 */
/* extern int32_t     nr_of_locations;*/     /* fileio.c   */

/* next line inserted at 8 dec 03 */
extern int32_t   nr_of_locs;             /* fileio.c   */

extern int32_t   nr_of_directions;       /* fileio.c   */

extern int32_t  first_direction_id;      /* fileio.c   */
extern int32_t  nr_of_cattrs;
extern attrInfo *c_loc_attrs;
extern attrInfo *c_obj_attrs;
extern attrInfo *local_attrs;

extern void     Output(char*);                                                  /* output.c   */
extern char     *strlwr(char*);                                                 /* syntax.c   */
extern int32_t  GetPar(int32_t*, int32_t*, int32_t*, char**, int32_t**);        /* sysfunc.c  */
extern int32_t  GetLvaluePar(int32_t*, int32_t*, int32_t*, char**, int32_t**);  /* sysfunc.c  */
extern int32_t  GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);       /* sysfunc.c  */
extern int32_t  CheckPars(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t); /* checkpar.c */

extern int32_t  XeqEqual(int32_t**);                                /* arithmtc.c */
extern int32_t  XeqLtGt(int32_t, int32_t**);                        /* arithmtc.c */
extern int32_t  XeqBasicOperator(int32_t, int32_t**);               /* arithmtc.c */
extern int32_t  XeqRnd(int32_t**);                                  /* arithmtc.c */
extern int32_t  XeqSetTimer(int32_t**);                             /* arithmtc.c */

extern void     Debug(void);   /* debug.c */

extern int32_t  NextOpcode(int32_t**);
extern void     ConvSpecId(int32_t*, int32_t*);                     /* sysfunc.c  */
extern void     ProcCFlagVal(int32_t, int32_t, int32_t);
extern void     ProcLFlagVal(int32_t, int32_t);
extern int32_t  TestCFlag(int32_t, int32_t);
extern int32_t  TestLFlag(int32_t);
extern int32_t  Entrance(int32_t);
extern int32_t  Exit(int32_t);                                            /* sysfunc.c  */
extern int32_t  Contents(int32_t);                                        /* sysfunc.c  */
extern int32_t  IsLit(int32_t);                                           /* sysfunc.c  */
extern int32_t  CountObjects(int32_t, int32_t, int32_t, int32_t);         /* sysfunc.c  */
extern int32_t  Synchronize(int32_t, int32_t, int32_t, int32_t, int32_t); /* sysfunc.c */
extern int32_t  XeqTrigger(int32_t, int32_t);                             /* execute.c  */
extern int32_t  CanSee(int32_t, int32_t);                                 /* sysfunc.c  */
extern int32_t  Move(int32_t, int32_t);                                   /* sysfunc.c  */
extern int32_t  Owns(int32_t, int32_t, int32_t);                          /* spantree.c */
extern int32_t  InitSpanTree(spanTree**, int32_t**);                      /* sysfunc.c  */
extern int32_t  Shuffle(int32_t);                                         /* sysfunc.c  */
extern int32_t  ExpandTree(int32_t, spanTree*);                           /* spantree.c */
extern int32_t  BuildRoute(int32_t, int32_t*, spanTree*);                 /* spantree.c */
extern int32_t  SpanTree(int32_t, int32_t, int32_t, spanTree*);           /* sysfunc.c  */
extern char     *GetDescr(int32_t, int32_t);                              /* execute.c  */
extern int32_t  HandleTimers(void);                                       /* timers.c   */
extern void     PrintString(char*);                                       /* output.c   */
extern void     PrintWord(int32_t);                                       /* output.c   */
extern void     PrintNumber(int32_t);                                     /* output.c   */
extern int32_t  IsWordId(int32_t);                                        /* execute.c  */
extern int32_t  IsLocId(int32_t);
extern int32_t  IsObjId(int32_t);
extern int32_t  IsCDescrId(int32_t);
extern int32_t  IsLDescrId(int32_t);
extern int32_t  IsCFlagId(int32_t);
extern int32_t  IsLFlagId(int32_t);
extern int32_t  IsTimerId(int32_t);
extern int32_t  IsTestFun(int32_t);
extern int32_t  IsIntAct(int32_t);
extern int32_t  IsCAttrId(int32_t);
extern int32_t  IsLAttrId(int32_t);
extern void     PrintId(int32_t);                   /* output.c */
extern int32_t  LookUpId(char*);                    /* syntax.c */
extern void     PrintId(int32_t);                   /* output.c */

/* Story progress save functions */
extern int32_t SaveStoryInfo(FILE*);
extern int32_t SaveSpecialIds(FILE*);
extern int32_t SaveLocationDirectory(FILE*);
extern int32_t SaveObjectDirectory(FILE*);
extern int32_t SaveExits(FILE*);
extern int32_t SaveCommonAttributes(FILE*);
extern int32_t SaveLocalAttributes(FILE*);
extern int32_t SaveCommonFlags(FILE*);
extern int32_t SaveLocalFlags(FILE*);
extern int32_t SaveAllTimers(FILE*);
/* end of story progress save functions */

/* Story progress restore functions */
extern int32_t RestoreStoryInfo(storyInfo*, FILE*);
extern int32_t RestoreSpecialIds(FILE*);
extern int32_t RestoreLocationDirectory(FILE*);
extern int32_t RestoreObjectDirectory(FILE*);
extern int32_t RestoreExits(FILE*);
extern int32_t RestoreCommonAttributes(FILE*);
extern int32_t RestoreLocalAttributes(FILE*);
extern int32_t RestoreCommonFlags(FILE*);
extern int32_t RestoreLocalFlags(FILE*);
extern int32_t RestoreTimers(FILE*);
/* end of story progress restore functions */


/* Debug functions */
extern int32_t  PrintSpecialIds(void);              /* debug.c */
extern void PrintLocationDirectory(void);       /* debug.c */
extern void PrintObjectDirectory(void);         /* debug.c */
extern void PrintExits(void);                   /* debug.c */
extern void PrintCommonAttributes(void);         /* debug.c */
extern void PrintLocalAttributes(void);          /* debug.c */
extern void PrintCommonFlags(void);             /* debug.c */
extern void PrintLocalFlags(void);              /* debug.c */
extern void PrintAllTimers(void);               /* debug.c */
/* end of debug functions */

#endif
