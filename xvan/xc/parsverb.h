
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

#if !defined(__parsverb)
#define __parsverb

/*************************/
/* External declarations */
/*************************/

extern int32_t  line_num;
extern FILE     *datafile;
extern verbDir  *verb_dir;

extern void     PrintActionRec(actionRec*);
extern char     *GetNextWord(int32_t*,int32_t, int32_t, FILE**, fileList**);
extern int32_t  LookUpId(char*);
extern int32_t  ScanWordTable(char*, wordTable*, int32_t, int32_t);  /* prsedys.c */
extern int32_t  ParseSentence(char*, actionRec*);
extern void     InitVerbActionRec(actionRec*);
extern int32_t  StoreActionRec(actionRec*);
extern int32_t  IsIntAct(int32_t);
extern int32_t  ParseAssignment(char**, int32_t*, int32_t*, int32_t*, int32_t, FILE**, fileList**);
extern int32_t  ParseIfThenElse(char**, int32_t*, int32_t, int32_t, int32_t, int32_t, FILE**, fileList**);
extern int32_t  ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**, fileList**);
extern int32_t  StoreKeyword(int32_t);
extern int32_t  StoreFun(int32_t*, int32_t);
extern void     ErrHdr(void);                                /* errors.c */
extern void     PrintError(int16_t, resultStruct*, char*);   /* errors.c */
extern char     *TranslateKeyword(char*);                    /* errors.c */

#endif
