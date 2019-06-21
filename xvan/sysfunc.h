
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


#if !defined(__sysfunc)
#define __sysfunc

/***********/
/* Defines */
/***********/

#define STACK_SIZE   50   /* Also defined in init.h */

/*************************/
/* External declarations */
/*************************/

extern dirInfo *loc_dir;            /* fileio.c */
extern dirInfo *obj_dir;            /* fileio.c */

extern locationInfo **locations;    /* fileio.c */
extern objectInfo   **objects;      /* fileio.c */

extern int32_t   *com_loc_flags;    /* fileio.c */
extern int32_t   *com_obj_flags;    /* fileio.c */
extern int32_t   *local_flags;      /* fileio.c */

extern int32_t   nr_of_cflags;      /* fileio.c */

extern attrInfo  *c_loc_attrs;
extern attrInfo  *c_obj_attrs;
extern attrInfo  *local_attrs;

extern int32_t   nr_of_cattrs;

extern int32_t   *stack;
extern int32_t   sp;

extern int32_t   *exit_data;        /* fileio.c   */

extern wordTable *word_table;       /* fileio.c */

extern int32_t nr_of_locs;          /* fileio.c   */
extern int32_t nr_of_objs;          /* fileio.c   */

extern int32_t nr_of_words;         /* fileio.c */

extern int32_t nr_of_directions;    /* fileio.c   */
extern int32_t first_direction_id;  /* fileio.c   */

extern int32_t active_entity;       /* letsplay.c */
extern int32_t curr_loc;            /* letsplay.c */
extern int32_t direction;           /* letsplay.c */
extern int32_t prepos;              /* letsplay.c */
extern int32_t action;              /* letsplay.c */
extern int32_t actor;               /* letsplay.c */
extern int32_t subject;             /* letsplay.c */
extern int32_t specifier;           /* letsplay.c */
extern int32_t value;               /* letsplay.c */
extern int32_t ordinal;             /* letsplay.c */

extern void         PrintError(int16_t, resultStruct*, char*);                        /* errors.c  */
extern int32_t      NextOpcode(int32_t**);
extern resultStruct XeqTrigger(int32_t, int32_t, usrActionRec*, int32_t);             /* execute.c */
extern int32_t      XeqTestFun(int32_t, int32_t**, usrActionRec*, int32_t, int32_t*); /* xeqfun.c  */
extern resultStruct XeqIntAct(int32_t, int32_t**, usrActionRec*, int32_t);            /* xeqfun.c  */
extern int32_t      IsWordId(int32_t);
extern int32_t      IsLocId(int32_t);
extern int32_t      IsObjId(int32_t);
extern int32_t      IsTimerId(int32_t);
extern int32_t      IsTestFun(int32_t);
extern int32_t      IsIntAct(int32_t);
extern int32_t      IsCDescrId(int32_t);
extern int32_t      IsLDescrId(int32_t);
extern int32_t      IsCFlagId(int32_t);
extern int32_t      IsLFlagId(int32_t);
extern int32_t      IsCTriggId(int32_t);
extern int32_t      IsLTriggId(int32_t);
extern int32_t      IsCAttrId(int32_t);
extern int32_t      IsLAttrId(int32_t);
extern int32_t      IsVerbId(int32_t);             /* execute.c */

#endif
