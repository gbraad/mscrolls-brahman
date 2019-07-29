
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


#if !defined(__output)
#define __output

/*************************/
/* External declarations */
/*************************/

extern FILE      *transcriptfile;                                          /* init.c        */
extern FILE      *debugfile;                                               /* init.c        */

extern storyInfo story_info;                                               /* init.c        */

extern char      *outputline;                                              /* init.c        */
extern char      *json_msg_from_story;                                     /* init.c        */

extern short     transcript;                                               /* letsplay.c    */
extern short     debug_mode;                                               /* letsplay.c    */
extern int32_t   nr_of_words;
extern int32_t   muted;                                                    /* letsplay.c    */
extern int32_t   capital;                                                  /* letsplay.c    */
extern wordTable *word_table;
extern dirInfo   *loc_dir;
extern dirInfo   *obj_dir;
extern timerInfo *timers;

extern int32_t   article;                                                  /* letsplay.c    */
extern int32_t   capital;                                                  /* letsplay.c    */

extern void      PrintError(int16_t, resultStruct*, char*);                /* errors.c      */
extern void      ConvSpecId(int32_t*, int32_t*);                           /* sysfunc.c     */
extern int32_t   ConvertDynamicDSys(char*, extendedSysDescr*);             /* trnslate.c    */
extern void      NL_PrintArticle(sysDescr*, int32_t, int);                 /* nl-article.c  */
extern void      ENG_PrintArticle(sysDescr*, int32_t, int);                /* eng-article.c */
extern int32_t   IsLocId(int32_t);                                         /* execute.c     */
extern int32_t   IsObjId(int32_t);
extern int32_t   IsTimerId(int32_t);
extern int32_t   IsCDescrId(int32_t);                                      /* execute.c     */
extern int32_t   IsLDescrId(int32_t);                                      /* execute.c     */
extern int32_t   IsCAttrId(int32_t);                                       /* execute.c     */
extern int32_t   IsLAttrId(int32_t);                                       /* execute.c     */
extern int32_t   IsWordId(int32_t);                                        /* execute.c     */
extern char      *GetDescr(int32_t, int32_t);                              /* execute.c     */
extern int32_t   GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*); /* sysfunc.c     */
extern void      ifi_emitResponse(const char*);
extern char      *AddToString(char*, char*);                               /* descr2str.c   */
extern char      *ResetString(char*);                                      /* jsonbld.c     */

#endif
