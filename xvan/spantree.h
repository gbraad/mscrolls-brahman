
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


#if !defined(__spantree)
#define __spantree

/*************/
/* Externals */
/*************/

extern locationInfo **locations;                               /* fileio.c */
extern objectInfo   **objects;                                 /* fileio.c */

extern int32_t      *exit_data;                                /* fileio.c */
extern int32_t      nr_of_locs;                                /* fileio.c */

extern int32_t      nr_of_directions;                          /* fileio.c */
extern int32_t      first_direction_id;                        /* fileio.c */

extern void         PrintError(int16_t, resultStruct*, char*); /* errors.c */

#endif
