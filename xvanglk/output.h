#if !defined(__output)
#define __output


/*************************/
/* External declarations */
/*************************/

extern winid_t mainwin;            /* init.c */
extern winid_t statuswin;          /* init.c */

extern FILE *transcriptfile;       /* init.c     */
extern FILE *debugfile;            /* init.c     */


extern char *outputline;           /* init.c     */

extern short     transcript;       /* letsplay.c */
extern short     debug_mode;       /* letsplay.c */
extern int32_t   nr_of_words;
extern int32_t   capital;          /* letsplay.c */
extern wordTable *word_table;
extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;
extern timerInfo *timers;

extern void     ConvSpecId(int32_t*, int32_t*); /* sysfunc.c */
extern int32_t  IsLocId(int32_t);               /* execute.c */
extern int32_t  IsObjId(int32_t);
extern int32_t  IsTimerId(int32_t);
extern int32_t  IsCDescrId(int32_t);            /* execute.c */
extern int32_t  IsLDescrId(int32_t);            /* execute.c */
extern int32_t  IsCAttrId(int32_t);             /* execute.c */
extern int32_t  IsLAttrId(int32_t);             /* execute.c */
extern int32_t  IsWordId(int32_t);              /* execute.c */
extern char     *GetDescr(int32_t, int32_t);    /* execute.c */
extern int32_t  GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);  /* sysfunc.c */

extern int32_t article;                /* letsplay.c */
extern int32_t capital;                /* letsplay.c */

#endif
