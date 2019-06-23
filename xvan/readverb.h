
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


#if !defined(__readverb)
#define __readverb

/*************************/
/* External declarations */
/*************************/

extern FILE          *datafile;

extern int32_t       GetNextCode32(int32_t*);
extern compActionRec *ReadActionRec(int32_t*);
extern int32_t       *RdTrCode(int32_t*);

extern void          PrintVerb(verbInfo*);
extern void          PrintCompActionRec(compActionRec*);
extern void          PrintError(int16_t, resultStruct*, char*);  /* errors.c */

#endif
