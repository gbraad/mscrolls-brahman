
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
          int16_t play_mode;
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


typedef struct {
          char     *dynamic;
          sysDescr part1;          /* This struct is used to  */
          int32_t  connect_prepos; /* store a description of  */
          sysDescr part2;          /* an object or location.  */
        } extendedSysDescr;        /* It is used by the       */
                                   /* interpreter to track    */
                                   /* down object and loc ids */
                                   /* and print the article   */


typedef struct cd {
          int32_t nr_of_objects;
          int32_t object_ids[MAX_CONTAINED_OBJECTS];
        } contData;


/* 20 jun 2015: struct attrData removed for it wasn't used */


typedef struct {
          int32_t          nr_of_dsys;
          extendedSysDescr descr[MAX_DSYS];  /* May change during game.      */
          contData         contained_objs;
          int32_t          held_by;          /* Tells the containing object. */
                                             /* Not used by locations.       */
          int64_t  offset;
        } dirInfo;


typedef struct {
          int32_t scope;
          int64_t offset;
        } verbDir;


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
          int64_t counter_data_offset; /* start of initial ctr info    */
          int64_t attribute_data_offset;
        } dirData;


typedef struct {
          int32_t type;
          int32_t value;
          int32_t value_owner; /* 10march2017: for description types */
        } attrInfo;


typedef struct wt {
          char word[MAX_WORD_LEN+1];
          char print_word[MAX_WORD_LEN+1];
          int32_t id;
          int32_t single_id;   /* @!@ */
          int32_t types[MAX_TYPES];
        } wordTable;


typedef struct {
          char keyword_eng[MAX_WORD_LEN+1];
          char keyword_nl[MAX_WORD_LEN+1];
  } kwTable;


typedef struct tc {
          int32_t       id;
          int32_t       *code;
          struct tc *next;
        } triggerInfo;


typedef struct dc {
          int32_t       id;
          char      *text;
          struct dc *next;
        } descrInfo;


typedef struct ti {
          int32_t   value;
          int32_t   step;
          int32_t   interval;
          int32_t   update;    /* Tells when timer value must be updated. */
          int16_t   direction;
          int16_t   state;
          int32_t   trigger_at;
          int32_t   trigger_spec;
          int32_t   execute[2];
        } timerInfo;


typedef struct {
          int32_t nr_of_adverbs;
          int32_t adverb[MAX_PARSE_ADVERBS];
        } adverbInfo;


typedef struct {
          int32_t nr_of_prepositions;
          int32_t preposition[MAX_PARSE_PREPOS];
        } preposInfo;


typedef struct pi {
          /* 26sep19 we've added plurality. with plural[] we  */   /* @!@ */
          /* denote for each subject's par 1 nounn whether    */
          /* it must be matched as plural or single. We need  */
          /* this because for regular plurality, NextWordId() */
          /* will return the single form with type PLURAL.    */
          extendedSysDescr actor;
          int32_t          action1;
          int32_t          action2;
          int32_t          scope;   /* Scope of action (this loc or all) */
          adverbInfo       adverbs;
          int32_t          q_word;
          int32_t          direction;
          extendedSysDescr subject[MAX_SUBJECTS];
          int32_t          single[MAX_SUBJECTS]; /* single id for plural noun */  /* @!@ */
          extendedSysDescr specifier;
          preposInfo       prepositions;
          int32_t          value;         /* make this a long. */
          int32_t          ordinal;
          struct pi        *next;         /* DO WE REALLY NEED THIS?? */
        } parsedInput;


typedef struct ar3 {
    /* used to create action records by the interpreter        */
    /* these action records are matched against compActionrec  */
    /* but they also contain a score field for disambiguation. */  /* @!@ */
          int32_t    actor;
          int32_t    action1;
          int32_t    action2;
          adverbInfo adverbs;
          int32_t    q_word;
          int32_t    direction;
          int32_t    subject;
          int32_t    specifier;
          preposInfo prepositions;
          int32_t    value;     /* make this a long. */
          int32_t    ordinal;
          int32_t    execute[2];
          int32_t    score;
          struct ar3 *next;    /* This must remain the last field. */
        } parserActionRec;


typedef struct ar1 {
   /* Used to store compiler generated action records read from */
   /* the datafile. These action records have only one subject. */
          int32_t    actor;
          int32_t    action1;
          int32_t    action2;
          adverbInfo adverbs;
          int32_t    q_word;
          int32_t    direction;
          int32_t    subject;
          int32_t    specifier;
          preposInfo prepositions;
          int32_t    value;        /* make this a long. */
          int32_t    ordinal;
          int32_t    execute[2];
          struct ar1 *next;    /* This must remain the last field. */
        } compActionRec;


typedef struct ar2 {
   /* Used to store action records generated from user input */  /* @!@ */
   /* These action records may have more than one subject.   */
   /* 26sep2019 we introduced plurality. A subject can be    */
   /* plural as denoted by PLURAL keyword in the subject     */
   /* In that case the plural_subjects field contains the    */
   /* relevant object ids. The max number of object ids is   */
   /* equal to MAX_SUBJECTS because we wil copy them to an   */
   /* action record for execution.                           */
          int32_t    actor;
          int32_t    action1;
          int32_t    action2;
          adverbInfo adverbs;
          int32_t    q_word;
          int32_t    direction;
          int32_t    subject[MAX_SUBJECTS];
          int32_t    plural_subjects[MAX_SUBJECTS];  /* @!@ */
          int32_t    specifier;
          preposInfo prepositions;
          int32_t    value;    /* make this a long. */
          int32_t    ordinal;
          struct ar2 *next;    /* This must remain the last field. */
        } usrActionRec;


typedef struct vi {
          compActionRec *action_rec;
          int32_t       *code;
          int32_t       *parser_rules;
          struct vi     *next;
        } verbInfo;


typedef struct lc {
          descrInfo     *descriptions;
          compActionRec *action_recs;
          triggerInfo   *triggers;
        } locationInfo;


typedef struct oc {
          descrInfo     *descriptions;
          compActionRec *action_recs;
          triggerInfo   *triggers;
        } objectInfo;


typedef struct {
          char     *name;
          int32_t  owner;
        } debugInfo;


typedef struct {
          int32_t nr_of_hits;
          int32_t *matched_objs;
        } match;


typedef struct {
          int32_t   from_loc;
          int32_t   from_dir;
          int16_t   level;
        } spanTree;


/* 19nov2016, added resultStruct for functions that */
/* may return a status code as well as a value (in  */
/* case a value is returned that is equal to a      */
/* status code, we have ambiguity.                  */


typedef struct {
          int32_t tag;
          int32_t owner; /* 02may19, for descriptions */
          int32_t value;
        } resultStruct;

#endif
