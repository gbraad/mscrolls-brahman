
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


#if !defined(__letsplay)
#define __letsplay

/*************************/
/* External declarations */
/*************************/

<<<<<<< HEAD
=======
extern short        debug_info;  /* init.c   */
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
extern dirInfo      *obj_dir;                                       /* fileio.c */
extern storyInfo    story_info;                                     /* init.c   */

extern char         *outputline;                                    /* init.c   */

<<<<<<< HEAD
extern void         Output(char*, int);                             /* output.c   */
=======
extern void         Output(void);                                   /* output.c   */
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
extern void         PrintString(char*, int);                        /* output.c   */
extern int32_t      ProcessInput(char*);                            /* input.c    */
extern int32_t      HandleTimers(usrActionRec*, int32_t);           /* timers.c   */

#endif
