
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


#if !defined(__execute)
#define __execute

/***********/
/* Defines */
/***********/

#define EVAL_ARR_SIZE   20  /* Max 20 boolean operators and      */
                            /* values (1 or 0) in one condition. */

/*************************/
/* External declarations */
/*************************/

extern verbInfo     **verbs;
extern int32_t      nr_of_locs;                                                       /* fileio.c  */
extern int32_t      nr_of_objs;                                                       /* fileio.c  */
extern int32_t      nr_of_directions;
extern int32_t      first_direction_id;
extern locationInfo **locations;
extern objectInfo   **objects;
extern dirInfo      *loc_dir;
extern dirInfo      *obj_dir;
extern int32_t      *exit_data;                                                       /* fileio.c   */
extern int32_t      **common_trigs;                                                   /* fileio.c   */
extern int32_t      *trigg_owners;
extern int32_t      *descr_owners;
extern timerInfo    *timers;

extern int32_t      active_entity;                                                    /* letsplay.c */
extern int32_t      curr_loc;                                                         /* letsplay.c */
extern int32_t      subject;                                                          /* letsplay.c */

extern void         PrintError(int16_t, resultStruct*, char*);                        /* errors.c   */
extern int32_t      InMem(int32_t);                                                   /* fileio.c   */
extern int32_t      Load(int32_t);                                                    /* fileio.c   */
extern void         SkipFun(int32_t**);                                               /* sysfunc.c  */
extern int32_t      LookUpId(char*);                                                  /* syntax.c   */
extern int32_t      Exit(int32_t, usrActionRec*, int32_t);                            /* sysfunc.c  */
extern int32_t      Entrance(int32_t, usrActionRec*, int32_t);                        /* sysfunc.c  */
extern int32_t      Move(int32_t, int32_t);                                           /* sysfunc.c  */
extern int32_t      XeqTestFun(int32_t, int32_t**, usrActionRec*, int32_t, int32_t*); /* xeqfun.c   */
extern int32_t      XeqIntAct(int32_t, int32_t**, usrActionRec*, int32_t);            /* xeqfun.c   */
extern void         SkipIntAct(int32_t**);                                            /* xeqfun.c   */
extern void         PrintId(int32_t, int);                                            /* output.c   */
extern int32_t      GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);         /* sysfunc.h  */

/***********************************/
/* System function implementations */
/***********************************/

extern void         Push(int32_t);                                                    /* sysfunc.c */
extern int32_t      Pop(void);                                                        /* sysfunc.c */

extern void         And(void);                                                        /* sysfunc.c */
extern void         Or(void);                                                         /* sysfunc.c */
extern void         Not(void);                                                        /* sysfunc.c */

#endif
