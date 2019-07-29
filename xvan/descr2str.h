
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


#if !defined(__descr2str)
#define __descr2str

/*************************/
/* External declarations */
/*************************/

extern int32_t   nr_of_words;
extern wordTable *word_table;
extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;

extern void      Log(char*, char*, char*);

extern void      ConvSpecId(int32_t*, int32_t*);                /* sysfunc.c */
int32_t          ConvertDynamicDSys(char*, extendedSysDescr*);  /* trnslate.c */
extern int32_t   IsLocId(int32_t);                              /* execute.c */
extern int32_t   IsObjId(int32_t);                              /* execute.c */
extern char      *ExpandString(char*, int);

#endif
