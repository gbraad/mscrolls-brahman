
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


#if !defined(__trnslate)
#define __trnslate


/**************************/
/* External declarations. */
/**************************/

extern storyInfo story_info;  /* init.c */
extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;
extern wordTable *word_table;

extern int32_t capital;       /* letsplay.c */
extern int32_t article;       /* letsplay.c */
extern int32_t nr_of_locs;    /* fileio.c   */
extern int32_t nr_of_objs;    /* fileio.c   */
extern int32_t nr_of_words;

extern char *outputline;      /* init.c   */

extern int32_t curr_loc;      /* the current location.     */  /* letsplay.c */
extern int32_t direction;     /* the current direction.    */  /* letsplay.c */
extern int32_t prepos;        /* the lst preposition.      */  /* letsplay.c */
extern int32_t action;        /* the current action.       */  /* letsplay.c */
extern int32_t actor;         /* the current actor.        */  /* letsplay.c */
extern int32_t subject;       /* the current subject.      */  /* letsplay.c */
extern int32_t specifier;     /* the current specifier.    */  /* letsplay.c */
extern int32_t value;         /* the current value.        */  /* letsplay.c */
extern int32_t ordinal;       /* the current ordinal value */  /* letsplay.c */

extern void NL_MoreInfo(extendedSysDescr*, match*, char*);   /* nl-moreinfo.c  */
extern void NL_PrintNotFound(extendedSysDescr*);             /* nl-moreinfo.c  */
extern void ENG_MoreInfo(extendedSysDescr*, match*, char*);  /* eng-moreinfo.c */
extern void ENG_PrintNotFound(extendedSysDescr*);            /* eng-moreinfo.c */

extern void     Output(char*, int);                           /* output.c  */
extern void     PrintError(int16_t, resultStruct*, char*);    /* errors.c */
extern int32_t  IsLocId(int32_t);
extern int32_t  ParseDSys(char*, extendedSysDescr*);
extern int32_t  LookUpId(char*);
extern int32_t  TestCFlag(int32_t, int32_t);
extern int32_t  HasLight(int32_t);
extern int32_t  IsLit(int32_t);                                             /* sysfunc.c */
extern int32_t  CanSee(int32_t, int32_t);                                   /* sysfunc.c */
extern int32_t  Owns(int32_t, int32_t);                                     /* sysfunc.c */
extern void     PrintId(int32_t, int);                                      /* output.c  */
extern void     PrintExtendedSysDescr(extendedSysDescr*, int);              /* output.c  */
extern void     PrintString(char*, int);                                    /* output.c  */
extern int32_t  ApplyParserRules(parsedInput*, match*, match*, match*);     /* parser.h  */

extern void PrintUsrActionRec(usrActionRec*);              /* DELETE THIS ONE */
#endif
