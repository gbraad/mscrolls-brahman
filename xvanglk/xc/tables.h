
/* Copyright 2016, 2017 Marnix van den Bos */

#if !defined(__tables)
#define __tables


#define TERMINATOR   "******"
#define UNKNOWN      -1

/*************/
/* externals */
/*************/

extern int32_t  line_num;
extern int32_t  first_direction_id;
extern dirData  dir_data;                          /* fileio.c  */

extern int32_t  ParseDSys(char*, extendedSysDescr*);  /*prsedsys.c */
extern void     ErrHdr(void);                         /* output.c  */

#endif
