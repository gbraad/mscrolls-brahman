
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


#if !defined(__restart)
#define __restart

/*************************/
/* External declarations */
/*************************/

extern dirData      dirs;                        /* dirs.c   */
extern dirInfo      *loc_dir;                    /* fileio.c */
extern dirInfo      *obj_dir;                    /* fileio.c */
extern locationInfo **locations;                 /* fileio.c */
extern objectInfo   **objects;                   /* fileio.c */
extern int32_t      *exit_data;                  /* fileio.c */
extern int32_t      *com_loc_flags;              /* fileio.c */
extern int32_t      *com_obj_flags;              /* fileio.c */
extern int32_t      *local_flags;                /* fileio.c */
extern attrInfo     *c_loc_attrs;                /* fileio.c */
extern attrInfo     *c_obj_attrs;                /* fileio.c */
extern attrInfo     *local_attrs;                /* fileio.c */
extern timerInfo    *timers;                     /* fileio.c */

extern int32_t      curr_loc;                    /* letsplay.c */

extern void     ResetGlobals(void);              /* resglobs.c */
extern void     FreeAllLocations(void);          /* cleanup.c  */
extern void     FreeAllObjects(void);            /* cleanup.c  */
extern int32_t  InitVerbs(void);                 /* fileio.c   */
extern int32_t  InitLocations(void);             /* fileio.c   */
extern int32_t  InitObjects(void);               /* fileio.c   */
extern int32_t  InitAttributes(void);            /* fileio.c   */
extern void     ProcLFlagVal(int32_t, int32_t);  /* sysfunc.c  */
extern int32_t  ReadLocDir(int64_t);             /* fileio.c   */
extern int32_t  ReadObjDir(int64_t);             /* fileio.c   */
extern int32_t  ReadMapData(int64_t);            /* fileio.c   */
extern int32_t  ReadFlags(int64_t);              /* fileio.c   */
extern int32_t  ReadTimers(int64_t);             /* fileio.c   */

#endif
