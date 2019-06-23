
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

#if !defined(__prsefun)
#define __prsefun

#define MAX_COND_LEN    100  /* also defined in prsetrig.h, fndctrig.h and prsassgn.h */

/*************/
/* externals */
/*************/

extern int32_t       line_num;
extern int32_t       nr_of_words;
extern wordTable     *word_table;

extern void          ErrHdr(void);                                                  /* errors.c   */
extern void          PrintError(int16_t, resultStruct*, char*);                     /* errors.c   */
extern void          NrErr(char*, char*);                                           /* errors.c   */
extern void          TypeErr(int32_t, char*, char*);                                /* errors.c   */
extern char          *TranslateKeyword(char*);                                      /* errors.c   */
extern void          InitActionRec(actionRec*);                                     /* prsesntc.c */
extern int32_t       ParseSentence(char*, actionRec*);                              /* prsesntc.c */
extern char          *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);  /* fileio.c   */
extern int32_t       ParseLeftBanana(int32_t*, int32_t*);
extern int32_t       ParseRightBanana(int32_t*, int32_t*);
extern int32_t       StringToNum(char*, int32_t*);
extern int32_t       GetDescrId(char*, int32_t*, int32_t, int32_t);
extern int32_t       GetLocationId(char*, int32_t*, int32_t, int64_t);
extern int32_t       GetObjectId(char*, int32_t*, int32_t, int64_t);
extern resultStruct  GetFlagId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t);
extern resultStruct  GetTriggerId(char*, int32_t, int32_t*, int32_t, int32_t);      /* tables.c */
extern int32_t       GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t); /* 10march2017 */
extern int32_t       GetTimerId(char*, int32_t*, int32_t);
extern int32_t       LookUpId(char*);
extern int32_t       IsLocId(int32_t);
extern int32_t       IsObjId(int32_t);
extern int32_t       IsCDescrId(int32_t);
extern int32_t       IsLDescrId(int32_t);
extern int32_t       IsCAttrId(int32_t);
extern int32_t       IsLAttrId(int32_t);
extern int32_t       IsLRelId(int32_t);
extern int32_t       IsCFlagId(int32_t);
extern int32_t       IsLFlagId(int32_t);
extern int32_t       IsCTriggId(int32_t);
extern int32_t       IsLTriggId(int32_t);
extern int32_t       IsTimerId(int32_t);
extern int32_t       IsWordId(int32_t);
extern int32_t       IsIntAct(int32_t);
extern int32_t       IsSpecId(int32_t);

#endif
