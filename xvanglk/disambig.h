#if !defined(__parser)
#define __parser


/*************************/
/* External declarations */
/*************************/

extern verbInfo **verbs;
extern int32_t  action;
extern int32_t  actor;
extern int32_t  subject;
extern int32_t  specifier;

extern int32_t  InMem(int32_t);      /* fileio.c  */
extern int32_t  Load(int32_t);       /* fileio.c  */
extern int32_t  Execute(int32_t*);   /* execute.c */
extern void     PrintString(char*);  /* output.c  */
extern void     PrintParserActionRec(parserActionRec*);  /* debug.c */
extern int32_t  MatchActionRec(compActionRec*, usrActionRec*, int32_t);




#endif
