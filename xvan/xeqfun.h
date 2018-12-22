
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018, 2019 Marnix van den Bos.             */
/*                                                                      */
/* <marnix.home@gmail.com>                                              */
/*                                                                      */
/* This file is part of XVAN, an Interactive Fiction authoring system.  */
/*                                                                      */
/* XVAN is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, version 3.                             */
/*                                                                      */
/* XVAN is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with XVAN.  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                      */
/************************************************************************/


#if !defined(__xeqfun)
#define __xeqfun

/***********/
/* Defines */
/***********/

#define DEBUGFILE      "debug.txt"
#define TESTFILE       "testinput.txt"
#define TRANSCRIPTFILE "transcript.txt"
#define SAVEFILE       "save.dat"

/*************************/
/* External declarations */
/*************************/

extern FILE      *testfile;            /* for testinput  */
extern FILE      *transcriptfile;      /* for transcript */
extern FILE      *debugfile;           /* for debug dump */

extern ifiStats  ifi_stats;            /* init.c */
extern storyInfo story_info;           /* init.c */
extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;
extern int32_t   *exit_data;
extern timerInfo *timers;

extern short     testmode;             /* letsplay.c */
extern short     transcript;           /* letsplay.c */
extern short     debug_mode;           /* letsplay.c */
extern short     fire_timers;          /* timers.c   */

extern int32_t   curr_loc;             /* letsplay.c */
extern int32_t   action;               /* letsplay.c */
extern int32_t   actor;                /* letsplay.c */
extern int32_t   subject;              /* letsplay.c */
extern int32_t   specifier;            /* letsplay.c */
extern int32_t   prepos;               /* letsplay.c */
extern int32_t   direction;            /* letsplay.c */
extern int32_t   value;                /* letsplay.c */
extern int32_t   ordinal;              /* letsplay.c */
extern int32_t   indent;               /* letsplay.c */
extern int32_t   muted;                /* letsplay.c */

extern char      *outputline;          /* init.c     */
extern char      *json_msg_from_story; /* init.c     */

extern int32_t   disambig_score;       /* disambig.c */

extern int32_t  nr_of_locs;            /* fileio.c   */
extern int32_t  nr_of_directions;      /* fileio.c   */
extern int32_t  first_direction_id;    /* fileio.c   */
extern int32_t  nr_of_cattrs;
extern attrInfo *c_loc_attrs;
extern attrInfo *c_obj_attrs;
extern attrInfo *local_attrs;

extern void     ifi_emitResponse(const char*);
extern int32_t  CheckIFI(char*);

extern char     *ReadJsonString(char*, int*);

extern char     *TranslateKeyword(char*);                                        /* errors.c     */
extern void     PrintError(int16_t, resultStruct*, char*);                       /* errors.c     */
extern void     Output(char*, int);                                              /* output.c     */
extern char     *xv_strlwr(char*);                                               /* syntax.c     */
extern int32_t  InitUsrActionRec(usrActionRec*);                                 /* syntax.c     */
extern int32_t  GetPar(int32_t*, int32_t*, int32_t*, char**, int32_t**);         /* sysfunc.c    */
extern int32_t  GetLvaluePar(int32_t*, int32_t*, int32_t*, char**, int32_t**);   /* sysfunc.c    */
extern int32_t  GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);        /* sysfunc.c    */
extern int32_t  GetActionRecPar(usrActionRec*, int32_t**);                       /* sysfunc.c    */
extern int32_t  CheckPars(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t); /* checkpar.c   */

extern int32_t  Restart(void);                                                   /* restart.c    */

extern int32_t  XeqEqual(int32_t**);                                             /* arithmtc.c   */
extern int32_t  XeqLtGt(int32_t, int32_t**);                                     /* arithmtc.c   */
extern int32_t  XeqBasicOperator(int32_t, int32_t**);                            /* arithmtc.c   */
extern int32_t  XeqRnd(int32_t**);                                               /* arithmtc.c   */
extern int32_t  XeqSetTimer(int32_t**);                                          /* arithmtc.c   */

extern void     Debug(void);                                                     /* debug.c      */

extern int32_t  NL_XeqYesNo(void);                                               /* NL-xeqfun.c  */
extern int32_t  ENG_XeqYesNo(void);                                              /* ENG-xeqfun.c */

extern char*    AddToString(char*, char*);                                       /* descr2str.c  */
extern int32_t  NextOpcode(int32_t**);
extern void     ConvSpecId(int32_t*, int32_t*);                                  /* sysfunc.c    */
extern void     ProcCFlagVal(int32_t, int32_t, int32_t);
extern void     ProcLFlagVal(int32_t, int32_t);
extern int32_t  TestCFlag(int32_t, int32_t);
extern int32_t  TestLFlag(int32_t);
extern int32_t  Exit(int32_t, usrActionRec*, int32_t);                           /* sysfunc.c    */
extern int32_t  Entrance(int32_t, usrActionRec*, int32_t);                       /* sysfunc.c    */
extern int32_t  Contents(int32_t, usrActionRec*, int32_t);                       /* sysfunc.c    */
extern int32_t  IsLit(int32_t);                                                  /* sysfunc.c    */
extern int32_t  CountObjects(int32_t, int32_t, int32_t, int32_t);                /* sysfunc.c    */
extern int32_t  Synchronize(int32_t, int32_t, int32_t, int32_t, int32_t,
                            usrActionRec*, int32_t);                             /* sysfunc.c    */
extern int32_t  XeqTrigger(int32_t, int32_t, usrActionRec*, int32_t);            /* execute.c    */
extern int32_t  CanSee(int32_t, int32_t);                                        /* sysfunc.c    */
extern int32_t  Move(int32_t, int32_t);                                          /* sysfunc.c    */
extern int32_t  Owns(int32_t, int32_t, int32_t);                                 /* spantree.c   */
extern int32_t  InitSpanTree(spanTree**, int32_t**);                             /* sysfunc.c    */
extern int32_t  Shuffle(int32_t);                                                /* sysfunc.c    */
extern int32_t  ExpandTree(int32_t, spanTree*);                                  /* spantree.c   */
extern int32_t  BuildRoute(int32_t, int32_t*, spanTree*);                        /* spantree.c   */
extern int32_t  SpanTree(int32_t, int32_t, int32_t, spanTree*);                  /* sysfunc.c    */
extern int32_t  LetsTry(int32_t, usrActionRec);                                  /* lets-try.c   */
extern char     *GetDescr(int32_t, int32_t);                                     /* execute.c    */
extern int32_t  CheckDoTimers(void);                                             /* timers.c     */
extern int32_t  HandleTimers(usrActionRec*, int32_t);                            /* timers.c     */
extern void     PrintString(char*, int);                                         /* output.c     */
extern void     PrintWord(int32_t, int);                                         /* output.c     */
extern void     PrintNumber(int32_t, int);                                       /* output.c     */
extern int32_t  XeqVerbDefault(usrActionRec*, int32_t);                          /* execute.c    */
extern int32_t  IsWordId(int32_t);                                               /* execute.c    */
extern int32_t  Execute(int32_t*, usrActionRec*, int32_t, int32_t*);             /* execute.c    */
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
extern void     PrintId(int32_t, int);                                           /* output.c     */
extern int32_t  LookUpId(char*);                                                 /* syntax.c     */
extern void     PrintId(int32_t, int);                                           /* output.c     */

extern char *Base64Save(char*);

/* Debug functions */
extern int32_t  PrintSpecialIds(void);     /* debug.c */
extern void PrintLocationDirectory(void);  /* debug.c */
extern void PrintObjectDirectory(void);    /* debug.c */
extern void PrintExits(void);              /* debug.c */
extern void PrintCommonAttributes(void);   /* debug.c */
extern void PrintLocalAttributes(void);    /* debug.c */
extern void PrintCommonFlags(void);        /* debug.c */
extern void PrintLocalFlags(void);         /* debug.c */
extern void PrintAllTimers(void);          /* debug.c */
/* end of debug functions */

#endif
