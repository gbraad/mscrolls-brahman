
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__spantree)
#define __spantree

/*************/
/* Externals */
/*************/

extern locationInfo **locations;   /* fileio.c */
extern objectInfo   **objects;     /* fileio.c */


extern int32_t *exit_data;         /* fileio.c   */

extern int32_t nr_of_locs;         /* fileio.c   */

extern int32_t nr_of_directions;   /* fileio.c   */
extern int32_t first_direction_id; /* fileio.c   */

extern char *outputline;           /* init.c    */
extern void Output(char*);         /* output.c  */

extern void PrintString(char*);    /* output.c  */
#endif
