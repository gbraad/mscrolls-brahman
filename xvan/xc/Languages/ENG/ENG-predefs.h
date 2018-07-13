
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

#if !defined(__ENG_predefs)
#define __ENG_predefs


/*************/
/* Externals */
/*************/

extern char          entrance_trigger[];  /* predefs.c */
extern char          exit_trigger[];      /* predefs.c */
extern char          default_trigger[];   /* predefs.c */

extern int32_t       GetObjectId(char*, int32_t*, int32_t, int64_t);                           /* tables.c */
extern int32_t       GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t);  /* tables.c */
extern resultStruct  GetFlagId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t);           /* tables.c */
extern int32_t       GetDescrId(char*, int32_t*, int32_t, int32_t);                            /* tables.c */
extern resultStruct  GetTriggerId(char*, int32_t, int32_t*, int32_t, int32_t);                 /* tables.c */

#endif
