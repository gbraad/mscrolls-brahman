#if !defined(__trnslate)
#define __trnslate


/**************************/
/* External declarations. */
/**************************/

extern dirInfo *loc_dir;
extern dirInfo *obj_dir;

extern int32_t capital;       /* letsplay.c */
extern int32_t article;       /* letsplay.c */
extern int32_t nr_of_locs;    /* fileio.c   */
extern int32_t nr_of_objs;    /* fileio.c   */

extern char *outputline;      /* init.c   */

extern int32_t curr_loc;      /* the current location.     */  /* letsplay.c */
extern int32_t direction;     /* the current direction.    */  /* letsplay.c */
extern int32_t prepos;        /* the lst preposition.      */  /* letsplay.c */
extern int32_t action;        /* the current action.       */  /* letsplay.c */
extern int32_t actor;         /* the current actor.        */  /* letsplay.c */
extern int32_t subject;       /* the current subject.      */  /* letsplay.c */
extern int32_t specifier;     /* the current specifier.    */  /* letsplay.c */
extern int32_t value;         /* the current value.        */  /* letsplay.c */
extern int32_t ordinal;       /* the current ordinal value */  /* letsplay.c */


extern void     Output(char*);                            /* output.c  */
extern int32_t  IsLocId(int32_t);
extern int32_t  ParseDSys(char*, extendedSysDescr*);
extern int32_t  LookUpId(char*);
extern int32_t  TestCFlag(int32_t, int32_t);
extern int32_t  HasLight(int32_t);
extern int32_t  IsLit(int32_t);                           /* sysfunc.c */
extern int32_t  CanSee(int32_t, int32_t);                 /* sysfunc.c */
extern int32_t  Owns(int32_t, int32_t);                   /* sysfunc.c */
extern void     PrintId(int32_t);                         /* output.c  */
extern void     PrintExtendedSysDescr(extendedSysDescr*); /* output.c  */
extern void     PrintString(char*);                       /* output.c  */
extern void     GetInput(char*, char*);
extern int32_t  ApplyParserRules(parsedInput*, match*, match*, match*);     /* parser.h  */

extern void PrintUsrActionRec(usrActionRec*);              /* DELETE THIS ONE */
#endif
