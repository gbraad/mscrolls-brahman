
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


#if !defined(__dirs)
#define __dirs

/*************************/
/* External declarations */
/*************************/

extern wordTable *word_table;              /* fileio.c */
extern verbDir   *verb_dir;                /* fileio.c */
extern dirInfo   *loc_dir;                 /* fileio.c */
extern dirInfo   *obj_dir;                 /* fileio.c */
extern int32_t   *trigg_owners;            /* fileio.c */
extern int32_t   *descr_owners;            /* fileio.c */
extern int32_t   *exit_data;               /* fileio.c */
extern int32_t   *com_loc_flags;           /* fileio.c */
extern int32_t   *com_obj_flags;           /* fileio.c */
extern int32_t   *local_flags;             /* fileio.c */
extern timerInfo *timers;                  /* fileio.c */


extern int32_t   ReadDirOffsets(dirData*); /* fileio.c */
extern int32_t   ReadVoc(int64_t);         /* fileio.c */
extern int32_t   ReadTimers(int64_t);      /* fileio.c */
extern int32_t   RdTrigOwners(int64_t);    /* fileio.c */
extern int32_t   RdDescOwners(int64_t);    /* fileio.c */
extern int32_t   ReadMapData(int64_t);     /* fileio.c */
extern int32_t   ReadFlags(int64_t);       /* fileio.c */
extern int32_t   ReadLocDir(int64_t);      /* fileio.c */
extern int32_t   ReadObjDir(int64_t);      /* fileio.c */
extern int32_t   RdCTriggs(int64_t);       /* fileio.c */

#endif
