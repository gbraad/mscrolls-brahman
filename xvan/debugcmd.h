
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


#if !defined(__debugcmd)
#define __debugcmd

/*************************/
/* External declarations */
/*************************/

extern short   debug_level;

extern int32_t IsVerbId(int32_t);     /* execute.c */
extern void    PrintString(char*, int);    /* output.c  */
extern int32_t IsCTriggId(int32_t);   /* execute.c */
extern int32_t IsLTriggId(int32_t);   /* execute.c */
extern int32_t IsCAttrId(int32_t);    /* execute.c */
extern int32_t IsLAttrId(int32_t);    /* execute.c */
extern void    PrintId(int32_t, int);      /* output.c  */
extern int32_t IsTimerId(int32_t);    /* execute.c */
extern void    PrintNumber(int32_t, int);  /* output.c  */
extern void    PrintWord(int32_t, int);    /* output.c  */
extern void    PrintError(int16_t, resultStruct*, char*);
extern void    Output(void);               /* output.c */

extern int32_t IsLocId(int32_t);
extern int32_t IsObjId(int32_t);
extern int32_t IsCFlagId(int32_t);
extern int32_t IsLFlagId(int32_t);
extern int32_t TestCFlag(int32_t, int32_t);
extern int32_t TestLFlag(int32_t);

#endif
