#if !defined(__readverb)
#define __readverb


/*************************/
/* External declarations */
/*************************/

extern FILE *datafile;

extern int32_t           GetNextCode32(int32_t*);
extern compActionRec *ReadActionRec(int32_t*);
extern int32_t           *RdTrCode(int32_t*);
extern char          *outputline;                /* init.c   */

extern void PrintVerb(verbInfo*);
extern void PrintCompActionRec(compActionRec*);
extern void Output(char*);                       /* output.c */



#endif
