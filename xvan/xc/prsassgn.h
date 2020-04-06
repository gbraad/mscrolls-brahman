
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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

#if !defined(__prsassgn)
#define __prsassgn

#define MAX_COND_LEN		100 /* also defined in prsefun.h, fndctrig.h and prsetrig.h*/


/*************/
/* Externals */
/*************/

extern int32_t     line_num;
extern FILE    *datafile;
extern dirData dir_data;

extern void     PrintError(int16_t, resultStruct*, char*);                                /* errors.c   */
extern int32_t  ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**, fileList**);       /* prsefun.c  */
extern int32_t  CheckPars(int32_t*);                                                      /* prsefun.c  */
extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);  /* fileio.c   */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);             /* fileio.c   */
extern int32_t  IsSpecId(int32_t);                                                        /* fileio.c   */
extern int32_t  IsIntAct(int32_t);                                                        /* keyword.c  */
extern int32_t  LookUpId(char*);                                                          /* prsesntc.c */
extern int32_t  IsDirection(char*, int32_t*);                                             /* prsesntc.c */
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);                         /* tables.c   */
extern int32_t  GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t);  /* tables.c   */ /* 10march2017 */
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);                           /* tables.c   */
extern int32_t  GetTimerId(char*, int32_t*, int32_t);                                     /* tables.c   */
extern int32_t  IsLAttrId(int32_t);                                                       /* tables.c   */
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);                            /* tables.c   */
extern int32_t  IsLDescrId(int32_t);

extern int32_t  StringToNum(char*, int32_t*);                                             /* prsetrig.c */
extern void ErrHdr(void);                                                                 /* output.c   */

#endif
