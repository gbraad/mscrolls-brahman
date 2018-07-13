
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
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

#if !defined(__init)
#define __init

/*************/
/* Externals */
/*************/

extern int32_t       endian;
extern uint32_t      xv_htonl(uint32_t);                            /* endian.c      */
extern uint32_t      xv_ntohl(uint32_t);                            /* endian.c      */

extern int32_t       ReadStoryInfo(storyInfo*, char*, fileList**);  /* storyinf.c    */
extern int32_t       StoreStoryInfo(storyInfo*);                    /* storyinf.c    */
extern int32_t       SetUpTables(void);                             /* tables.c      */
extern int32_t       InitFileList(fileList**, char*, FILE*);        /* tables.c      */
extern void          FreeFileList(fileList**);                      /* tables.c      */
extern void          InitOffsets(void);                             /* tables.c      */
extern int32_t       StoreOffsets(void);                            /* fileio.c      */
extern int32_t       ENG_PreDefs(void);                             /* ENG-predefs.c */
extern int32_t       NL_PreDefs(void);                              /* NL-predefs.c  */
extern int32_t       CheckTables(void);                             /* tables.c      */
extern int32_t       FreeTables(void);                              /* tables.c      */
extern int32_t       StoreDirs(void);                               /* fileio.c      */
extern int32_t       ParseStory(FILE*, fileList**);                 /* prsstory.c    */
extern int32_t       PrintWordList(void);                           /* tables.c      */
extern int32_t       CreateWordTable(void);                         /* tables.c      */
extern int32_t       LookUpId(char*);                               /* prsesntc.c    */
extern int32_t       GetObjectId(char*, int32_t*, int32_t, int64_t);
extern int32_t       GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t);  /* tables.c */ /* 10march2017 */
extern resultStruct  GetFlagId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t);
extern int32_t       GetDescrId(char*, int32_t*, int32_t, int32_t);
extern resultStruct  GetTriggerId(char*, int32_t, int32_t*, int32_t, int32_t);  /* tables.c */
extern void          ErrHdr(void);                                              /* errors.c */
extern void          PrintError(int16_t, resultStruct*, char*);                 /* errors.c */
extern char          *TranslateKeyword(char*);                                  /* errors.c */

#endif
