
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


#if !defined(__resglobs)
#define __resglobs

extern short debug_mode;
extern short testmode;

extern int32_t nr_of_locs;
extern int32_t nr_of_objs;
extern int32_t nr_of_cflags;
extern int32_t loc_flags_string_len;
extern int32_t nr_of_cattrs;
extern int32_t nr_of_lattrs;
extern int32_t nr_of_directions;
extern int32_t nr_of_timers;

extern int32_t sp;

extern int32_t active_entity;

extern int32_t curr_loc;
extern int32_t direction;
extern int32_t prepos;
extern int32_t action;
extern int32_t actor;
extern int32_t subject;
extern int32_t specifier;
extern int32_t value;
extern int32_t ordinal;

extern int32_t indent;
extern int32_t capital;
extern int32_t article;
extern int32_t muted;

extern int32_t disambig_score;

extern short   fire_timers;

#endif
