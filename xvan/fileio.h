
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


#if !defined(__fileio)
#define __fileio

/***********/
/* defines */
/***********/

#define MAX_VERBS_IN_MEM    10
#define MAX_LOCS_IN_MEM     10
#define MAX_OBJS_IN_MEM     10

/*************************/
/* External declarations */
/*************************/

extern FILE    *datafile;
extern dirData dirs;

extern uint16_t     xv_htons(uint16_t);                         /* endian.c   */
extern uint16_t     xv_ntohs(uint16_t);                         /* endian.c   */
extern uint32_t     xv_htonl(uint32_t);                         /* endian.c   */
extern uint32_t     xv_ntohl(uint32_t);                         /* endian.c   */
extern uint64_t     xv_htonll(uint64_t);                        /* endian.c   */
extern uint64_t     xv_ntohll(uint64_t);                        /* endian.c   */

extern verbInfo     *RdVerb(int64_t);                           /* readverb.c */
extern locationInfo *ReadLocation(int64_t);                     /* readloc.c  */
extern objectInfo   *ReadObject(int64_t);                       /* readobj.c  */
extern int32_t      *RdTrCode(int32_t*);                        /* readtrig.c */
extern void         PrintWTable(void);                          /* debug.c    */
extern void         PrintVerb(verbInfo*);                       /* debug.c    */
extern void         PrintLocation(locationInfo*);               /* debug.c    */
extern void         PrintObject(objectInfo*);                   /* debug.c    */
extern void         PrintError(int16_t, resultStruct*, char*);  /* errors.c   */

extern int32_t      IsVerbId(int32_t);                           /* execute.c  */
extern int32_t      IsLocId(int32_t);                            /* execute.c  */
extern int32_t      IsObjId(int32_t);                            /* execute.c  */
extern int32_t      IsCTriggId(int32_t);                         /* execute.c  */

/* Special Ids for save() and restore() functions */
extern int32_t      action;                                      /* letsplay.c */
extern int32_t      prepos;                                      /* letsplay.c */
extern int32_t      direction;                                   /* letsplay.c */
extern int32_t      active_entity;                               /* letsplay.c */
extern int32_t      curr_loc;                                    /* letsplay.c */
extern int32_t      actor;                                       /* letsplay.c */
extern int32_t      subject;                                     /* letsplay.c */
extern int32_t      specifier;                                   /* letsplay.c */
extern int32_t      value;                                       /* letsplay.c */
/* End of special Ids for save() and restore() functiona */

#endif
