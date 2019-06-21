
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


#if !defined(__restore)
#define __restore

/*************************/
/* External declarations */
/*************************/

extern int32_t      nr_of_locs;
extern int32_t      nr_of_objs;
extern int32_t      nr_of_cflags;
extern int32_t      loc_flags_string_len;
extern int32_t      nr_of_cattrs;
extern int32_t      nr_of_lattrs;
extern int32_t      nr_of_directions;
extern int32_t      nr_of_timers;

extern storyInfo    story_info;
extern dirInfo      *loc_dir;
extern dirInfo      *obj_dir;
extern attrInfo     *c_loc_attrs;
extern attrInfo     *c_obj_attrs;
extern attrInfo     *local_attrs;
extern int32_t      *exit_data;
extern int32_t      *com_loc_flags; /* Common location flags.   */
extern int32_t      *com_obj_flags; /* Common object flags.     */
extern int32_t      *local_flags;   /* Local flags.             */
extern verbInfo     **verbs;        /* Default verb code array. */
extern locationInfo **locations;    /* Location pointer array.  */
extern objectInfo   **objects;      /* Object pointer array.    */
extern timerInfo    *timers;        /* Timer array.             */

/* Special Ids for save() and restore() functions */
extern int32_t      action;                                                  /* letsplay.c */
extern int32_t      prepos;                                                  /* letsplay.c */
extern int32_t      direction;                                               /* letsplay.c */
extern int32_t      active_entity;                                           /* letsplay.c */
extern int32_t      curr_loc;                                                /* letsplay.c */
extern int32_t      actor;                                                   /* letsplay.c */
extern int32_t      subject;                                                 /* letsplay.c */
extern int32_t      specifier;                                               /* letsplay.c */
extern int32_t      value;                                                   /* letsplay.c */
extern int32_t      ordinal;                                                 /* letsplay.c */
/* End of special Ids for save() and restore() functions */

extern void         PrintError(int16_t, resultStruct*, char*);               /* errors.c   */
extern int16_t      xv_ntohs(int16_t);                                       /* endian.c   */
extern int32_t      xv_ntohl(int32_t);                                       /* endian.c   */
extern int64_t      xv_ntohll(int64_t);                                      /* endian.c   */

extern int32_t      Base64ToBytes(char*, int*, int, int8_t*, int*, int8_t*); /* base64.c   */
extern char         *ResetString(char*);

#endif
