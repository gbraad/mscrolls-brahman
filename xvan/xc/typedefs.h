
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

#if !defined(__typedefs)
#define __typedefs

typedef struct fl {
          char      name[MAX_FILENAME_LEN+1];
          int32_t   current_line;
          int32_t   lines;
          FILE      *fp;
          struct fl *previous;
        } fileList;

typedef struct {
          char    title[MAX_TITLE_LEN];
          char    author[INPUT_LINE_LEN];
          char    organization[INPUT_LINE_LEN];
          char    cover_text[INPUT_LINE_LEN];
          char    credits[INPUT_LINE_LEN];
          char    version[MAX_VERSION_LEN];
          char    android_mkt[INPUT_LINE_LEN];
          char    ios_mkt[INPUT_LINE_LEN];
          char    backimage[MAX_FILENAME_LEN];
          char    effect[MAX_FILENAME_LEN];
          char    primary_color[INPUT_LINE_LEN];
          int16_t ui_sidebar;
          int16_t ui_textinput;
          int16_t ui_compass;
          int16_t autolink;
          char    compiler_version[MAX_COMPILER_LEN];
          int16_t xvan_language;
          int16_t story_language;
          int16_t debug;
        } storyInfo;

/*************************************************************/
/* ******************* nov 18 2016 ***************************/
/*************************************************************/
/* System descriptions used to have the following syntax:    */
/* [article] [0 or more adjectives] <noun>,                  */
/* e.g. "the old man" or "man".                              */
/* Syntax was changed to:                                    */
/* [article] [0 or more adjectives] <noun>                   */
/* <preposition>                                             */
/* [article] [0 or more adjectives] <noun>                   */
/* e.g. "the old man with the shiny sword" or                */
/* "gallery of paintings", "well of eternal youth".          */

/* The old system description was stored in struct sysDescr. */
/* The new system description is stored in struct            */
/* extendedSysDescr, which is a struct containing 2 sysDescr */
/* structs and a preposition.                                */
/*************************************************************/

typedef struct {
          int32_t article;                    /* This struct is used to store */
          int32_t nr_of_adjectives;           /* part of a description of an  */
          int32_t adjectives[MAX_PARSE_ADJ];  /* object or location.          */
          int32_t noun;
        } sysDescr;

/* 2019may12 we added dynamic system descriptions. A dynamic  */
/* system description contains one or more attributes whose   */
/* value(s) can only be determined at runtime when the system */
/* description is referenced. Therefore we do not parse the   */
/* description at compile time, but store the string for real */
/* time parsing by the interpreter. Compiler and interpreter  */
/* will test for dynamic == NULL to determine if a system     */
/* description is dynamic.                                    */

typedef struct {
          char     *dynamic;       /* This struct is used to  */
          sysDescr part1;          /* store a description of  */
          int32_t  connect_prepos; /* an object or location.  */
          sysDescr part2;          /* It is used by the       */
        } extendedSysDescr;        /* interpreter to track    */
                                   /* down object and loc ids */
                                   /* and print the article   */

typedef struct {
          int32_t nr_of_objects;
          int32_t object_ids[MAX_CONTAINED_OBJECTS];
        } contData;

typedef struct {
          int32_t          nr_of_dsys;
          extendedSysDescr descr[MAX_DSYS]; /* May change during game.      */
          contData         contained_objs;
          int32_t          held_by;         /* Tells the containing object. */
                                            /* Not used by locations.       */
          int64_t          offset;
        } dirInfo;

typedef struct {           /* It is important to store the scope in   */
          int32_t scope;   /* the verb dir and NOT in the word_table  */
          int64_t offset;  /* because of problems that may arise with */
        } verbDir;         /* parsing synonyms. The verb_dir does not */
                           /* distinguish synonyms.                   */

typedef struct sl {         /* Needed to keep track of synonym verbs.  */
          int32_t id;       /* Because of the redefine functionality   */
          struct  sl *next; /* for verbs, the synonyms no longer get   */
        } synList;          /* the same id as the original verb. They  */
                            /* are now also in the verb directory.     */
                            /* This type is only used in ParseVerb()   */

typedef struct ld {
          char       l_word[MAX_ID_LENGTH+1]; /* include '\0' */
          int32_t    l_id;
          int32_t    exists;
          dirInfo    dir_info;  /* Is later stored in datafile */
          struct ld  *next;
        } locationData;

typedef struct od {
          char      o_word[MAX_ID_LENGTH+1];
          int32_t   o_id;
          int32_t   exists;
          dirInfo   dir_info;  /* Is later stored in datafile */
          struct od *next;
        } objectData;

typedef struct fd {
          char f_word[MAX_ID_LENGTH+1];
          int32_t  owner;
          int32_t  f_id;
          int32_t  value;
          int32_t  exists;
          int32_t  common;
          int32_t  redefine;
          struct fd *next;
        } flagData;

typedef struct rd {
          char r_word[MAX_ID_LENGTH+1];
          int32_t  owner;  /* attribute owner */
          int32_t  r_id;
          int32_t  type;   /* used to distinguish numbers. */
          int32_t  value;
          int32_t  value_owner; /* for type descr_id */
          int32_t  exists;
          int32_t  common;
          int32_t  redefine;
          struct rd *next;
        } attrData;

typedef struct td {
          char t_word[MAX_ID_LENGTH+1];
          int32_t  owner;
          int32_t  t_id;
          int32_t  exists;
          int32_t  common;
          int32_t  redefine;
          struct td *next;
        } triggerData;

typedef struct md {
          char m_word[MAX_ID_LENGTH+1];
          int32_t m_id;
          int32_t exists;
          struct md *next;
        } timerData;

typedef struct dd {
          char    d_word[MAX_ID_LENGTH+1];
          int32_t owner;
          int32_t d_id;
          int32_t exists;
          int32_t common;
          struct  dd *next;
        } descrData;

typedef struct ti {
          int32_t   id;
          int32_t   value;
          int32_t   step;
          int32_t   interval;
          int32_t   update;  /* Tells when time value must be updated. */
          int16_t   direction;
          int16_t   state;
          int32_t   trigger_at;
          int32_t   trigger_spec;
          int32_t   execute[2];
          struct ti *next;
        } timerInfo;

typedef struct {  /* directory offsets in datafile */
          int64_t story_info_offset;   /* story_info struct offset     */
		  int64_t word_data_offset;    /* start of vocabulary info     */
          int64_t loc_dir_offset;      /* start of location dir        */
          int64_t obj_dir_offset;      /* start of object dir          */
          int64_t common_descr_offset; /* start of common descriptions */
          int64_t common_trigg_offset; /* start of common triggers     */
          int64_t exit_data_offset;    /* start of map info            */
          int64_t flag_data_offset;    /* start of initial flag values */
          int64_t trigg_owners_offset;
          int64_t descr_owners_offset;
          int64_t counter_data_offset; /* start of initial timer info  */
          int64_t attribute_data_offset;
        } dirData;

typedef struct {
          int32_t type;
          int32_t value;
          int32_t owner; /* 10march2017: for description types */
        } attrInfo;

typedef struct wi {
          char    word[MAX_WORD_LEN+1]; /* include the '\0' char */
          char    print_word[MAX_WORD_LEN+1];
          int32_t id;
          int32_t types[MAX_TYPES];
          int32_t redefine;
          struct  wi *next;
        } wordInfo;

typedef struct wt {
          char     word[MAX_WORD_LEN+1];
          char     print_word[MAX_WORD_LEN+1];
          int32_t  id;
          int32_t  types[MAX_TYPES];
          int32_t  redefine;
        } wordTable;


typedef struct {
          char keyword_eng[MAX_WORD_LEN+1];
          char keyword_nl[MAX_WORD_LEN+1];
  } kwTable;

typedef struct {
          int32_t nr_of_adverbs;
          int32_t adverb[MAX_PARSE_ADVERBS];
        } adverbInfo;

typedef struct {
          int32_t nr_of_prepositions;
          int32_t preposition[MAX_PARSE_PREPOS];
        } preposInfo;

typedef struct {
          char     name[MAX_ID_LENGTH+1];
          int32_t  owner;
        } debugInfo;

/* 24jan2018: if this struct is changed, make sure  */
/* to also change the struct size in functions      */
/* ParsePar(), NextPar() and StoreParList()         */
/* see comments in ParsePar() for more details.     */
typedef struct {
          int32_t    actor;
          int32_t    action1;
          int32_t    action2;
          adverbInfo adverbs;
          int32_t    q_word;
          int32_t    direction;
          int32_t    subject;
          int32_t    specifier;
          preposInfo prepositions;
          int32_t    value;         /* make this a long. */
          int32_t    ordinal;       /* ordinal number    */
          int32_t    execute[2];
        } actionRec;

/* 19nov2016, added resultStruct for functions that */
/* may return a status code as well as a value (in  */
/* case a value is returned that is equal to a      */
/* status code, we have ambiguity.                  */

typedef struct {
          int32_t tag;
          int32_t value;
        } resultStruct;
#endif
