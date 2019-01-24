
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


#if !defined(__disambig)
#define __disambig

/*************************/
/* External declarations */
/*************************/

extern verbInfo      **verbs;
extern int32_t       action;
extern int32_t       actor;
extern int32_t       subject;
extern int32_t       specifier;

extern int32_t       InMem(int32_t);                                         /* fileio.c  */
extern int32_t       Load(int32_t);                                          /* fileio.c  */
extern resultStruct  Execute(int32_t*, usrActionRec*, int32_t, int32_t*);    /* execute.c */
extern void          PrintError(int16_t, resultStruct*, char*);              /* errors.c  */
extern void          PrintParserActionRec(parserActionRec*);                 /* debug.c   */
extern int32_t       MatchActionRec(compActionRec*, usrActionRec*, int32_t);

#endif
