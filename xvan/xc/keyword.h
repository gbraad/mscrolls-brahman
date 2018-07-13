
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
#define MAX_TITLE_LEN        100 /* must be the same as input_line_len */ /* @@@ */
#define MAX_VERSION_LEN      100 /* must be the same as input_line_len */ /* @@@ */
#define MAX_FILENAME_LEN     100 /* must be the same as input_line_len */ /* @@@ */
#define MAX_COMPILER_LEN     100 /* must be the same as input_line_len */ /* @@@ */
#define MAX_TRIGG_LEN       1000
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

#define ERROR           0   /* do not change the OK and ERROR values */
#define OK              1
#define NO_KEYWORD      2
#define VALUE           4
#define ARROW           5
#define KEYWORD_TAG     6
#define PAR_LIST_TAG    7
#define LOC_ID          8
#define OBJ_ID          9
#define TRIGG_ID        10
#define FLAG_ID         11
#define ATTR_ID         12  /* added on July 1st 2015, for change to prefix attributes with r_ */
#define STRING          13
#define NUMBER          14  /* e.g. 'take 3 pebbles, set dial to 123' */
#define TIM_ID          15
#define DESCR_ID        16
#define FLAG_VALUES     17
#define LOC_DIR         18
#define OBJ_DIR         19
#define NO_TYPE         20
#define VOCABULARY      21
#define ACTION_REC      22
#define WORD_ID         23  /* For parameter typechecking. */
#define XVAN_LANGUAGE   24
#define STORY_LANGUAGE  25
#define ENG             26
#define NL              27
#define PRIMARY_COLOR   28


/*******************************************/
/* keywords that are used in the inputfile */
/*******************************************/

#define LOWER_BOUND_TESTFUN         30
#define CANSEE           31
#define OWNS             32
#define ISLIT            33
#define TESTFLAG         34
#define EQUAL            35
#define LESS_THAN        36
#define GREATER_THAN     37
#define VALID_DIRECTION  38
#define EXIT             39
#define TRIGGER          40
#define YES_NO           41
#define RUNVERB          42
#define RUNCOMMON        43
#define TRY              44
#define ISOBJECT         45
#define UPPER_BOUND_TESTFUN         50

#define LOWER_BOUND_INT_ACT	        51
#define MOVE            52
#define OWNER           53
#define PRINT           54
#define PRINTBOLD       55
#define PRINTITALIC     56
#define PRINTCR         57
#define PRINTCRBOLD     58
#define PRINTCRITALIC   59
#define SETFLAG         60
#define CLEARFLAG       61
#define SETATTRIBUTE    62
#define SETTIMER        63
#define STARTTIMER      64
#define STOPTIMER       65
#define WAIT            66
#define SYNCHRONIZE     67
#define AGREE           68
#define DISAGREE        69
#define NOMATCH         70  /* Has return value NO_MATCH. */
#define INDENT          71
#define QUIT            72
#define ENTRANCE        73
#define CONTENTS        74
#define NEW_EXIT        75
#define BLOCK_EXIT      76
#define GET_SUBJECT     77
#define GET_SPECIFIER   78
#define GO_TO           79
#define SAVE			80  /* Dec 20 07: For save() function    */
#define RESTORE			81  /* Dec 20 07: For restore() function */
#define ADD             82
#define EQ_ADD          83
#define SUB             84
#define EQ_SUB          85
#define ASTERIX         86
#define EQ_MUL          87
#define QUOT            88
#define REM             89
#define RAND            90
#define BACKGROUND      91
#define TEXT            92
#define BOLD            93
#define ITALIC          94
#define UNDERLINE       95
#define TRANSCRIPT      96
#define TESTMODE        97
#define DEBUG           98
#define DISTANCE        99
#define FIRSTDIR       100
#define DEST           101
#define SHUFFLE        102
#define COUNT          103
#define PRINTSTATUS    104
#define PRINTCRSTATUS  105
#define SETCURSOR      106
#define CLEARSTATUS    107
#define CLEARSCREEN    108
#define HITANYKEY      109
#define SCORE          110
#define CLEARJSON      111  /* @@@ */
#define ADDJSON        112  /* @@@ */
#define SENDJSON       113  /* @@@ */
#define NOTIMERS       114
#define UPPER_BOUND_INT_ACT         116

#define LOWER_BOUND_SPECIAL_CHAR    117
#define LEFT_PAR        118
#define RIGHT_PAR       119
#define EQUAL_SIGN		120
#define PLUS			121
#define MINUS			122
#define COMMA			123
#define DOT             124
#define PAR_DELIMITER   125
#define UPPER_BOUND_SPECIAL_CHAR     126

#define LOWER_BOUND_COMPILER_KEYWORD 127
#define MIN_VALID_COMPILER_KEYWORD 128  /* 4sep2017 for detecting errors */
#define LOCATION          128
#define OBJECT            129
#define TIMERS            130
#define END_OBJ           131
#define END_LOC           132
#define COMMON_DESCRS     133
#define COMMON_FLAGS      134
#define COMMON_ATTRS      135
#define COMMON_TRIGGERS   136
#define REDEFINE_TRIGGERS 137
#define REDEFINE_FLAGS    138
#define REDEFINE_ATTRS 139
#define STORY_INFO        140
#define MAX_VALID_COMPILER_KEYWORD 140  /* 4sep2017 for detecting errors */
#define EXITS           141
#define AND             142
#define OR              143
#define NOT             144
#define IF              145
#define THEN            146
#define ELSE            147
#define ENDIF           148
#define UP              149
#define DOWN            150
#define TITLE           151
#define VERSION         152
#define VOCAB           153
#define DESCRIPTIONS    154
#define CONTAINED       155
#define ATTRIBUTES      156
#define FLAGS           157
#define TRIGGERS        158
#define INIT            159 /* for timer's initial value */
#define STEP            160
#define INTERVAL        161
#define DIRECTION       162 /* Used with timers. */
#define STATE           163
#define GO              164
#define STOP            165
#define TRIGGER_AT      166
#define EXECUTE         167 /* Used with timers. */
#define OR_MORE         168
#define OR_LESS         169
#define EXACT           170
#define THIS            171
#define NONE            172
#define PREPOS          173
#define DIR             174
#define ARTICLE_A       175
#define ARTICLE_THE     176
#define ORDINAL         177 /* special sort of NUMBER e.g. 'take pebble 3' */
#define FUNCTION        178
#define DISAMBIG_RULES  179
#define END_RULES       180
#define LIBRARY         181 /* a library contains definitions that may be overwritten */
#define INSERT          182 /* switch to a new inputfile */
#define AUTHOR          183
#define ORGANIZATION    184
#define COVERTEXT       185
#define CREDITS         186
#define ANDROID_MARKET  187
#define IOS_MARKET      188
#define BACKIMAGE       189
#define EFFECT          190
#define NO_SIDEBAR      191
#define NO_TEXTINPUT    192
#define NO_COMPASS      193
#define AUTOLINK        194  /* @@@ */
#define UPPER_BOUND_COMPILER_KEYWORD 199


/**************************************************/
/* Keywords that are used in the vocabulary file  */
/**************************************************/

#define LOWER_BOUND_VOC_KEYWORD      200
#define MIN_VALID_VOC_KEYWORD        201  /* 4sep2017 for detecting errors */
#define VERB                         201
#define REDEFINE_VERB                202
#define ENDVERB                      203
#define NOUNS                        204
#define ADJECTIVES                   205
#define PREPOSITIONS                 206
#define ADVERBS                      207
#define Q_WORDS                      208  /* question words */
#define DIRECTIONS                   209
#define ARTICLES                     210
#define CONJUNCTION                  211
#define MAX_VALID_VOC_KEYWORD        211  /* 4sep2017 for detecting errors */
#define CONNECT_PREPOSITIONS         212  /* for extended system description */
#define SYNONYM                      213
#define SCOPE                        214
#define ACTOR_ONLY                   215
#define CURR_LOC_ONLY                216
#define ALL_LOCS                     217
#define DEFAULT                      218
#define PROLOGUE                     219
#define EPILOGUE                     220
#define UPPER_BOUND_VOC_KEYWORD      221

/*************************************/
/* keywords that are used internally */
/* by the compiler and interpreter.  */
/*************************************/

#define XEQ                          321
#define NO_XEQ                       322
#define SKIP                         323
#define END_OF_CODE                  324
#define END_OF_PAR                   325
#define MATCH                        326
#define NO_MATCH                     327
#define OVERFLOW                     328
#define CONTINUE                     329
#define GET_ANSWER                   330
#define ANSWER                       331
#define LINE                         332
#define SPACE                        ' '
#define ACTOR_ERROR                  333 /* 03dec2016     */
#define SUBJECT_ERROR                334 /* for translate */
#define SPECIFIER_ERROR              335 /* errors        */
#define NEXT_SENTENCE                336


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
