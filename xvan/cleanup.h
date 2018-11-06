
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


#if !defined(__cleanup)
#define __cleanup

extern int32_t nr_of_locs;               /* fileio.c */
extern int32_t nr_of_objs;               /* fileio.c */
extern int32_t nr_of_verbs;              /* fileio.h */

extern locationInfo **locations;     /* fileio.c */
extern objectInfo   **objects;       /* fileio.c */
extern verbInfo     **verbs;         /* fileio.c */
extern timerInfo    *timers;         /* fileio.c */
extern int32_t      **common_trigs;  /* fileio.c */
extern int32_t      *exit_data;      /* fileio.c */
extern int32_t      *com_loc_flags;  /* fileio.c */
extern int32_t      *com_obj_flags;  /* fileio.c */
extern int32_t      *local_flags;    /* fileio.c */
extern wordTable    *word_table;     /* fileio.c */
extern verbDir      *verb_dir;       /* fileio.c */
extern dirInfo      *loc_dir;        /* fileio.c */
extern dirInfo      *obj_dir;        /* fileio.c */
extern attrInfo     *c_loc_attrs;    /* fileio.c */
extern attrInfo     *c_obj_attrs;    /* fileio.c */
extern attrInfo     *local_attrs;    /* fileio.c */
extern int32_t      *trigg_owners;   /* fileio.c */
extern int32_t      *descr_owners;   /* fileio.c */
extern int32_t      *stack;          /* init.c   */

/* extern char         *outputline; */     /* init.c   */  /* @@@@@ */
extern char         *outputbuffer;   /* init.c   */  /* @@@@@ */

extern char         *ResetString(char*); /* output.c */  /* @@@@@ */


#endif
