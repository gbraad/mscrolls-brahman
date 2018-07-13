
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

#if !defined(__prsetrig)
#define __prsetrig

#define MAX_COND_LEN		100 /* also defined in prsefun.h and prsassgn.h */


/*************/
/* Externals */
/*************/

extern int32_t     line_num;
extern FILE    *datafile;
extern dirData dir_data;

extern int32_t       ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);             /* fileio.c */
extern char          *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);                        /* fileio.c */
extern int32_t       GetDescrId(char*, int32_t*, int32_t, int32_t);                                       /* tables.c */
extern int32_t       GetLocationId(char*, int32_t*, int32_t, int64_t);                                    /* tables.c */
extern int32_t       GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t);             /* tables.c */ /* 10march2017 */
extern int32_t       GetObjectId(char*, int32_t*, int32_t, int64_t);                                      /* tables.c */
extern resultStruct  GetFlagId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t);                      /* tables.c */
extern resultStruct  GetTriggerId(char*, int32_t, int32_t*, int32_t, int32_t);                            /* tables.c */
extern void          InitActionRec(actionRec*);
extern int32_t       SkipCommonTrigger(char**, int32_t*, FILE**, fileList**);                             /* fndctrig.c */
extern int32_t       ParseSentence(char*, actionRec*);
extern int32_t       ParseAssignment(char**, int32_t*, int32_t*, int32_t*, int32_t, FILE**, fileList**);  /*prsassgn.c */
extern int32_t       ParsePar(char**, int32_t*, int32_t*, int32_t*, int32_t, int32_t, FILE**, fileList**);
extern int32_t       ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**, fileList**);
extern int32_t       ProcLocDSys(char*, int32_t);
extern int32_t       ProcObjDSys(char*, int32_t);
extern int32_t       StoreKeyword(int32_t);
extern int32_t       StoreFun(int32_t*, int32_t);
extern int32_t       IsTestFun(int32_t);
extern int32_t       IsDirection(char*, int32_t*);
extern int32_t       IsIntAct(int32_t);
extern int32_t       IsLocId(int32_t);
extern int32_t       LookUpId(char*);
extern int32_t       StoreKeyword(int32_t);
extern int32_t       StoreId(int32_t);
extern int32_t       StoreString(char*);
extern int32_t       StoreInt32(int32_t);
extern int32_t       StoreActionRec(actionRec*);
extern void          ErrHdr(void);                                /* errors.c */
extern void          PrintError(int16_t, resultStruct*, char*);   /* errors.c */
extern char          *TranslateKeyword(char*);                    /* errors.c */

#endif
