
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
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


#if !defined(__letsplay)
#define __letsplay

/*************************/
/* External declarations */
/*************************/

extern short        debug_info;  /* init.c   */
extern dirInfo      *obj_dir;                                       /* fileio.c */
extern storyInfo    story_info;                                     /* init.c   */

extern int32_t      write_undo;                                     /* undo.c   */  /* @!@ */

extern char         *outputline;                                    /* init.c   */

extern void         Output(void);                                   /* output.c   */
extern void         PrintString(char*, int);                        /* output.c   */
extern int32_t      ProcessInput(char*);                            /* input.c    */
extern int32_t      HandleTimers(usrActionRec*, int32_t);           /* timers.c   */

#endif
