
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


#if !defined(__input)
#define __input

/**************************/
/* External declarations */
/*************************/

extern FILE *transcriptfile;                             /* init.c     */

extern short testmode;                                   /* letsplay.c */
extern short transcript;                                 /* letsplay.c */
extern char  *outputline;                                /* output.c   */
extern FILE  *testfile;                                  /* init.c     */

extern void  Output(char*, int);                         /* init.c      */
extern void  PrintError(int16_t, resultStruct*, char*);  /* errors.c    */
extern char  *AddToString(char*, char*);                 /* descr2str.c */
extern char  *ResetString(char*);                        /* descr2str.c */

/* IFI and JSON stuff */
extern int32_t    ValidateJson(char*);
extern int32_t    ProcessJson(char*, char*);
extern const char *ifi_getRequest(void);
extern void       ifi_emitResponse(const char*);
extern int32_t    GetNextKVpair(char*, int*, kvPair*);   /* json.c */
extern int32_t    CheckIFI(char*);                       /* ifi.c  */
extern int32_t    XeqIFIrequest(int32_t, jsonValue*);    /* ifi.c  */
extern void       SendIFIerror(char*, char*);

#endif
