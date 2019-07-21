
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

#if !defined(__keyword)
#define __keyword

/* On dec 31 03 changed max_id_length from 12 to 20           */
/* On dec 20 07 changed max_word_len from 8 to 15             */
/* On dec 21 07 added SAVE keyword (for save() function)      */
/* On jun 23 15 increased local attributes from 40 to 100     */
/* On aug 19 15 increased MAX_TRIGG_LEN from 200 to 500       */
/* On aug 26 15 increased MAX_CONTAINED_OBJECTS from 10 to 25 */
/* On jan 21 16 increased MAX_WORD_LEN from 15 to 20          */

#define WORD_LEN              16 /* needed to address flagbits     */
#define MAX_ID_LENGTH         20 /* identifiers will be truncated  */
                                 /* after 20 chars                 */
#define MAX_WORD_LEN          20 /* words will be truncated after  */
                                 /* 15 chars                       */
#define MAX_TITLE_LEN        100 /* must be the same as input_line_len */
#define MAX_VERSION_LEN      100 /* must be the same as input_line_len */
#define MAX_FILENAME_LEN     100 /* must be the same as input_line_len */
#define MAX_COMPILER_LEN     100 /* must be the same as input_line_len */
#define MAX_TRIGG_LEN       1000
#define MAX_PROMPT_LEN        10
#define INPUT_LINE_LEN       100 /* for interpreter 08mrt2018:     */
                                 /* changed to 100 for IFI-version */
#define OUTPUT_LINE_LEN       79 /* for interpreter                */
                                 /* 79 in case a line ends with a  */
                                 /* special character.             */
                                 /* a special character            */
#define MAX_DSYS              10 /* Max 5 system descriptions per  */
                                 /* location or object.            */
#define MAX_TYPES              4 /* a word can be of max 3 types   */
                                 /* 21nov2016: set to 4 because of */
                                 /* CONNECT_PREPOSi type. Max nr   */
                                 /* of types perceived by user is  */
                                 /* still 3.                       */
#define MAX_PARSE_ADJ          4 /* The interpreter will parse max */
                                 /* 4 adjectives per noun.         */
#define MAX_PARSE_ADVERBS      4 /* The interpreter will parse max */
                                 /* 4 adverbs per verb.            */
#define MAX_PARSE_PREPOS       4
#define MAX_CONTAINED_OBJECTS 50
#define MAX_HITS               6 /* Max number of matching locs or  */
                                 /* objects when translating sys    */
                                 /* descrs to ids.                  */

#define MAX_SUBJECTS           5 /* Maximum number of subjects in   */
                                 /* user input (used only by        */
                                 /* the interpreter).               */

#define NR_OF_SPECIAL_IDS     10 /* For save.c and restore.c        */
#define DEFAULT_PARSER_SCORE   0 /* For parserActionRec type        */
#define MAX_AMBIGUITY        100 /* do not try to resolve if more   */
                                 /* than 100 possibilities for      */
                                 /* mapping objects                 */

/***********************/
/* keyword definitions */
/***********************/

/* Make sure keywords are defined within the correct */
/* range. If not, the compiler may screw up.		 */


/*************************************/
/* keywords that are used internally */
/* by the compiler and interpreter.  */
/*************************************/

#define ERROR            0   /* do not change the OK and ERROR values */
#define OK               1
#define NO_KEYWORD       2
#define VALUE            4
#define ARROW            5
#define KEYWORD_TAG      6
#define PAR_LIST_TAG     7
#define LOC_ID           8
#define OBJ_ID           9
#define TRIGG_ID         10
#define FLAG_ID          11
#define ATTR_ID          12  /* added on July 1st 2015, for change to prefix attributes with r_ */
#define STRING           13
#define NUMBER           14  /* e.g. 'take 3 pebbles, set dial to 123' */
#define TIM_ID           15
#define DESCR_ID         16
#define FLAG_VALUES      17
#define LOC_DIR          18
#define OBJ_DIR          19
#define NO_TYPE          20
#define VOCABULARY       21
#define ACTION_REC       22
#define WORD_ID          23  /* For parameter typechecking. */
#define XVAN_LANGUAGE    24
#define STORY_LANGUAGE   25
#define ENG              26
#define NL               27
#define PRIMARY_COLOR    28
#define INTERPRETER_MODE 29
#define CHOICE_MODE      30
#define HYBRID_MODE      31


/*******************************************/
/* keywords that are used in the inputfile */
/*******************************************/

#define LOWER_BOUND_TESTFUN         100
#define CANSEE           101
#define OWNS             102
#define ISLIT            103
#define TESTFLAG         104
#define EQUAL            105
#define LESS_THAN        106
#define GREATER_THAN     107
#define VALID_DIRECTION  108
#define EXIT             109
#define TRIGGER          110
#define YES_NO           111
#define RUNVERB          112
#define RUNCOMMON        113
#define TRY              114
#define ISOBJECT         115
#define UPPER_BOUND_TESTFUN         200

#define LOWER_BOUND_INT_ACT	    201
#define MOVE            202
#define OWNER           203
#define PRINT           204
#define PRINTBOLD       205
#define PRINTITALIC     206
#define PRINTCR         207
#define PRINTCRBOLD     208
#define PRINTCRITALIC   209
#define SETFLAG         210
#define CLEARFLAG       211
#define SETATTRIBUTE    212
#define SETTIMER        213
#define STARTTIMER      214
#define STOPTIMER       215
#define WAIT            216
#define SYNCHRONIZE     217
#define AGREE           218
#define DISAGREE        219
#define NOMATCH         220  /* Has return value NO_MATCH. */
#define INDENT          221
#define QUIT            222
#define ENTRANCE        223
#define CONTENTS        224
#define NEW_EXIT        225
#define BLOCK_EXIT      226
#define GET_SUBJECT     227
#define GET_SPECIFIER   228
#define GO_TO           229
#define SAVE		    230  /* Dec 20 07: For save() function    */
#define RESTORE		    231  /* Dec 20 07: For restore() function */
#define ADD             232
#define EQ_ADD          233
#define SUB             234
#define EQ_SUB          235
#define ASTERIX         236
#define EQ_MUL          237
#define QUOT            238
#define REM             239
#define RAND            240
#define BACKGROUND      241
#define TEXT            242
#define BOLD            243
#define ITALIC          244
#define UNDERLINE       245
#define TRANSCRIPT      246
#define TESTMODE        247
#define DEBUG           248
#define DISTANCE        249
#define FIRSTDIR        250
#define DEST            251
#define SHUFFLE         252
#define COUNT           253
#define PRINTSTATUS     254
#define PRINTCRSTATUS   255
#define SETCURSOR       256
#define CLEARSTATUS     257
#define CLEARSCREEN     258
#define HITANYKEY       259
#define SCORE           260
#define CLEARJSON       261
#define ADDJSON         262
#define SENDJSON        263
#define NOTIMERS        264
#define RESTART         265
#define PICKONE         266
#define NEWDSYS         267
#define ADDCHOICE       268
#define PLAYMODE        269
#define UPPER_BOUND_INT_ACT         400

#define LOWER_BOUND_SPECIAL_CHAR    401
#define LEFT_PAR        402
#define RIGHT_PAR       403
#define EQUAL_SIGN	    404
#define PLUS		    405
#define MINUS		    406
#define COMMA		    407
#define DOT             408
#define PAR_DELIMITER   409
#define UPPER_BOUND_SPECIAL_CHAR     500

#define LOWER_BOUND_COMPILER_KEYWORD 501
#define MIN_VALID_COMPILER_KEYWORD 502  /* 4sep2017 for detecting errors */
#define LOCATION          502
#define OBJECT            503
#define TIMERS            504
#define END_OBJ           505
#define END_LOC           506
#define COMMON_DESCRS     507
#define COMMON_FLAGS      508
#define COMMON_ATTRS      509
#define COMMON_TRIGGERS   510
#define REDEFINE_TRIGGERS 511
#define REDEFINE_FLAGS    512
#define REDEFINE_ATTRS    513
#define STORY_INFO        514
#define MAX_VALID_COMPILER_KEYWORD 514  /* 4sep2017 for detecting errors */
#define EXITS           515
#define AND             516
#define OR              517
#define NOT             518
#define IF              519
#define THEN            520
#define ELSE            521
#define ENDIF           522
#define UP              523
#define DOWN            524
#define TITLE           525
#define VERSION         526
#define VOCAB           527
#define DESCRIPTIONS    528
#define CONTAINED       529
#define ATTRIBUTES      530
#define FLAGS           531
#define TRIGGERS        532
#define INIT            533 /* for timer's initial value */
#define STEP            534
#define INTERVAL        535
#define DIRECTION       536 /* Used with timers. */
#define STATE           537
#define GO              538
#define STOP            539
#define TRIGGER_AT      540
#define EXECUTE         541 /* Used with timers. */
#define OR_MORE         542
#define OR_LESS         543
#define EXACT           544
#define THIS            545
#define NONE            546
#define PREPOS          547
#define DIR             548
#define ARTICLE_A       549
#define ARTICLE_THE     550
#define ORDINAL         551 /* special sort of NUMBER e.g. 'take pebble 3' */
#define FUNCTION        552
#define DISAMBIG_RULES  553
#define END_RULES       554
#define LIBRARY         555 /* a library contains definitions that may be overwritten */
#define INSERT          556 /* switch to a new inputfile */
#define AUTHOR          557
#define ORGANIZATION    558
#define COVERTEXT       559
#define CREDITS         560
#define ANDROID_MARKET  561
#define IOS_MARKET      562
#define BACKIMAGE       563
#define EFFECT          564
#define NO_SIDEBAR      565
#define NO_TEXTINPUT    566
#define NO_COMPASS      567
#define AUTOLINK        568
#define UPPER_BOUND_COMPILER_KEYWORD 700


/**************************************************/
/* Keywords that are used in the vocabulary file  */
/**************************************************/

#define LOWER_BOUND_VOC_KEYWORD      701
#define MIN_VALID_VOC_KEYWORD        702  /* 4sep2017 for detecting errors */
#define VERB                         702
#define REDEFINE_VERB                703
#define ENDVERB                      704
#define NOUNS                        705
#define ADJECTIVES                   706
#define PREPOSITIONS                 707
#define ADVERBS                      708
#define Q_WORDS                      709  /* question words */
#define DIRECTIONS                   710
#define ARTICLES                     711
#define CONJUNCTION                  712
#define MAX_VALID_VOC_KEYWORD        713  /* 4sep2017 for detecting errors */
#define CONNECT_PREPOSITIONS         714  /* for extended system description */
#define SYNONYM                      715
#define SCOPE                        716
#define ACTOR_ONLY                   717
#define CURR_LOC_ONLY                718
#define ALL_LOCS                     719
#define DEFAULT                      720
#define PROLOGUE                     721
#define EPILOGUE                     722
#define UPPER_BOUND_VOC_KEYWORD      800

/*************************************/
/* keywords that are used internally */
/* by the compiler and interpreter.  */
/*************************************/

#define XEQ                          801
#define NO_XEQ                       802
#define SKIP                         803
#define END_OF_CODE                  804
#define END_OF_PAR                   805
#define MATCH                        806
#define NO_MATCH                     807
#define OVERFLOW                     808
#define CONTINUE                     809
#define GET_ANSWER                   810
#define ANSWER                       811
#define LINE                         812
#define SPACE                        ' '
#define ACTOR_ERROR                  813 /* 03dec2016     */
#define SUBJECT_ERROR                814 /* for translate */
#define SPECIFIER_ERROR              815 /* errors        */
#define NEXT_SENTENCE                816
#define DYN_DSYS                     817 /* dynamic d_sys 10may2019 */


/***********************************************/
/* Identifier range definitions                */
/*                                             */
/* We can tell the type of an id by its value. */
/***********************************************/

#define CURR_LOC                 4201 /* current location            */
#define FIRST_LOCATION_ID        4202
#define LAST_LOCATION_ID         4499 /* 297 locations               */

#define SUBJECT                  4500 /* denotes the subject of      */
                                      /* current action              */
#define ACTOR                    4501 /* denotes the current actor   */
#define SPECIFIER                4502 /* current specifier           */
#define PLAYER                   4503 /* Each story has a player     */
#define NO_SUCH_THING            4504 /* For disambiguation          */
#define IT                       4505 /* for it object               */
#define FIRST_OBJECT_ID          4503 /* This value is not a mistake.*/
#define LAST_OBJECT_ID           4799 /* 296 user definable objects  */
#define FIRST_COMMON_FLAG_ID     4800
#define LIT                      4800 /* f_lit is a system defined   */
                                      /* flag.                       */
#define OPAQUE                   4801 /* same for f_opaque.          */
#define BYPASS                   4802 /* same for f_bypass.          */
#define HIDDEN                   4803 /* same for f_hidden.          */
#define SWAP_SYS_DESCR           4804 /* same for f_swap_sys_descr   */
#define LAST_COMMON_FLAG_ID      4924 /* 124 common flags.           */
#define FIRST_LOCAL_FLAG_ID      4925
#define JSON_MODE                4925 /* system defined local flag  */  /* 20oct2017 */
#define LAST_LOCAL_FLAG_ID      5399  /* 474 local flags.           */
#define FIRST_COMMON_TRIGGER_ID 5400
#define ENTR                    5400 /* t_entrance is a system      */
                                     /* defined trigger.            */
#define EXITT                   5401 /* same for t_exit.            */
#define DEFAULT_TRIGG           5402 /* and for t_default           */  /* added on sep 3rd 2015 for Cloak of Darkness */
#define CHOICE                  5403 /* and for t_choice            */  /* added on jul 17th 2019 for choice IF        */
#define LAST_COMMON_TRIGGER_ID  5508 /* 100 common triggers.        */
#define FIRST_LOCAL_TRIGGER_ID  5509
#define LAST_LOCAL_TRIGGER_ID   6198 /* 689 local triggers.         */
#define FIRST_TIMER_ID          6199
#define LAST_TIMER_ID           6298 /* 99 timers.                  */
#define FIRST_COMMON_DESCR_ID   6299
#define DSYS                    6299 /* d_sys is a system defined   */
                                     /* common description.         */
#define LAST_COMMON_DESCR_ID    6323 /* 25 common descriptions.     */
#define FIRST_LOCAL_DESCR_ID    7100
#define LAST_LOCAL_DESCR_ID     8100 /* 474 descriptions.           */
#define FIRST_COMMON_ATTR_ID    9100
#define LAST_COMMON_ATTR_ID     10100 /* 100 common attributes.     */
#define IT_ATTR                 11100
#define FIRST_LOCAL_ATTR_ID     11100
#define LAST_LOCAL_ATTR_ID      12100 /* 100 local attributes       */
#define ACTION                  13100 /* denotes the current verb.  */
#define FIRST_VERB_ID           13101
#define LAST_VERB_ID            15100 /* 2000 verbs */
#define FIRST_WORD_ID           15101 /* verbs and words must be one range */
#define LAST_WORD_ID            21100 /* 6000 other words */
#define NO_ID                   21101
#define UNKNOWN_WORD            21102
#define EOS                     21109 /* End Of String; make sure */
                                      /* this value is NOT within */
                                      /* word/verb id range.      */
#endif
