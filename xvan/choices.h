
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


#if !defined(__choices)
#define __choices

/**************************/
/* External declarations */
/*************************/

extern int32_t      nr_of_locs;                                            /* fileio.c   */
extern int32_t      nr_of_objs;                                            /* fileio.c   */
extern int32_t      curr_loc;                                              /* letsplay.c */
extern storyInfo    story_info;                                            /* init.c     */
extern char         *json_msg_from_story;

extern int32_t      ContList(int32_t, int32_t*, int32_t);                  /* sysfunc.c  */
extern resultStruct XeqTrigger(int32_t, int32_t, usrActionRec*, int32_t);  /* execute.c  */
extern void         GetAddtlInput(kvPair*, char*, int32_t, int);           /* input.c    */
extern void         PrintString(char*, int);                               /* output.c   */
extern char         *ResetString(char*);                                   /* jsonbld.c  */

#endif
