
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


#if !defined(__readloc)
#define __readloc

/*************************/
/* External declarations */
/*************************/

extern FILE          *datafile;

extern char          *outputline;                                /* init.c    */

extern void          PrintError(int16_t, resultStruct*, char*);  /* errors.c  */
extern void          PrintCompActionRec(compActionRec*);         /* debug.c   */
extern void          PrintLocation(locationInfo*);               /* debug.c   */
extern int32_t       GetNextCode32(int32_t*);                    /* fileio.c  */
extern descrInfo     *ReadDescr(int32_t*);
extern triggerInfo   *ReadTrigger(int32_t*);
extern compActionRec *ReadActionRec(int32_t*);
extern int32_t       IsDescrId(int32_t);                         /* execute.c */
extern int32_t       IsTriggId(int32_t);                         /* execute.c */

#endif
