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
#define MAX_TITLE_LEN         40
#define MAX_VERSION_LEN       10
#define MAX_FILENAME_LEN      20
#define MAX_COMPILER_LEN      10
#define MAX_TRIGG_LEN       1000
#define INPUT_LINE_LEN        80 /* for interpreter                */
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
#define UPPER_BOUND_TESTFUN         42

#define LOWER_BOUND_INT_ACT	        45
#define MOVE            46
#define OWNER           47
#define PRINT           48
#define PRINTBOLD       49
#define PRINTITALIC     50
#define PRINTCR         51
#define PRINTCRBOLD     52
#define PRINTCRITALIC   53
#define SETFLAG         54
#define CLEARFLAG       55
#define SETATTRIBUTE    56
#define SETTIMER        57
#define STARTTIMER      58
#define STOPTIMER       59
#define WAIT            60
#define SYNCHRONIZE     61
#define AGREE           62
#define DISAGREE        63
#define NOMATCH         64  /* Has return value NO_MATCH. */
#define INDENT          65
#define QUIT            66
#define ENTRANCE        67
#define CONTENTS        68
#define NEW_EXIT        69
#define BLOCK_EXIT      70
#define GET_SUBJECT     71
#define GET_SPECIFIER   72
#define GO_TO           73
#define SAVE			74  /* Dec 20 07: For save() function    */
#define RESTORE			75  /* Dec 20 07: For restore() function */
#define ADD             76
#define EQ_ADD          77
#define SUB             78
#define EQ_SUB          79
#define ASTERIX         80
#define EQ_MUL          81
#define QUOT            82
#define REM             83
#define RAND            84
#define BACKGROUND      85
#define TEXT            86
#define BOLD            87
#define ITALIC          88
#define UNDERLINE       89
#define TRANSCRIPT      90
#define TESTMODE        91
#define DEBUG           92
#define DISTANCE        93
#define FIRSTDIR        94
#define DEST            95
#define SHUFFLE         96
#define COUNT           97
#define PRINTSTATUS     98
#define PRINTCRSTATUS   99
#define SETCURSOR      100
#define CLEARSTATUS    101
#define CLEARSCREEN    102
#define HITANYKEY      103
#define SCORE          104
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
#define LOCATION        128
#define OBJECT          129
#define TIMERS          130
#define END_OBJ         131
#define END_LOC         132
#define COMMON_DESCRS   133
#define COMMON_FLAGS    134
#define COMMON_ATTRS    135
#define COMMON_TRIGGERS 136
#define MAX_VALID_COMPILER_KEYWORD 136  /* 4sep2017 for detecting errors */
#define EXITS           137
#define AND             138
#define OR              139
#define NOT             140
#define IF              141
#define THEN            142
#define ELSE            143
#define ENDIF           144
#define UP              145
#define DOWN            146
#define TITLE           147
#define VERSION         148
#define VOCAB           149
#define DESCRIPTIONS    150
#define CONTAINED       151
#define ATTRIBUTES      152
#define FLAGS           153
#define TRIGGERS        154
#define INIT            155 /* for timer's initial value */
#define STEP            156
#define INTERVAL        157
#define DIRECTION       158 /* Used with timers. */
#define STATE           159
#define GO              160
#define STOP            161
#define TRIGGER_AT      162
#define EXECUTE         163 /* Used with timers. */
#define OR_MORE         164
#define OR_LESS         165
#define EXACT           166
#define THIS            167
#define NONE            168
#define PREPOS          169
#define DIR             170
#define ARTICLE_A       171
#define ARTICLE_THE     172
#define ORDINAL         173 /* special sort of NUMBER e.g. 'take pebble 3' */
#define FUNCTION        174
#define DISAMBIG_RULES  175
#define END_RULES       176
#define LIBRARY         177 /* a library contains definitions that may be overwritten */
#define INSERT          178 /* switch to a new inputfile */
#define UPPER_BOUND_COMPILER_KEYWORD 199


/**************************************************/
/* Keywords that are used in the vocabulary file  */
/**************************************************/

#define LOWER_BOUND_VOC_KEYWORD      200
#define MIN_VALID_VOC_KEYWORD        201  /* 4sep2017 for detecting errors */
#define VERB                         201
#define ENDVERB                      202
#define NOUNS                        203
#define ADJECTIVES                   204
#define PREPOSITIONS                 205
#define ADVERBS                      206
#define Q_WORDS                      207  /* question words */
#define DIRECTIONS                   208
#define ARTICLES                     209
#define CONJUNCTION                  210
#define MAX_VALID_VOC_KEYWORD        210  /* 4sep2017 for detecting errors */
#define CONNECT_PREPOSITIONS         211  /* for extended system description */
#define SYNONYM                      212
#define SCOPE                        213
#define ACTOR_ONLY                   214
#define CURR_LOC_ONLY                215
#define ALL_LOCS                     216
#define DEFAULT                      217
#define PROLOGUE                     218
#define EPILOGUE                     219
#define UPPER_BOUND_VOC_KEYWORD      220

/*************************************/
/* keywords that are used internally */
/* by the compiler and interpreter.  */
/*************************************/

#define XEQ                          221
#define NO_XEQ                       222
#define SKIP                         223
#define END_OF_CODE                  224
#define END_OF_PAR                   225
#define MATCH                        226
#define NO_MATCH                     227
#define OVERFLOW                     228
#define CONTINUE                     229
#define GET_ANSWER                   230
#define ANSWER                       231
#define LINE                         232
#define SPACE                        ' '
#define ACTOR_ERROR                  222 /* 03dec2016     */
#define SUBJECT_ERROR                223 /* for translate */
#define SPECIFIER_ERROR              224 /* errors        */
#define NEXT_SENTENCE                225


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
#define LAST_LOCAL_FLAG_ID      5399 /* 474 local flags.            */
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
#define FIRST_LOCAL_DESCR_ID    7100 /*6224*/
#define LAST_LOCAL_DESCR_ID     8100 /*6698*/ /* 474 descriptions.           */
#define FIRST_COMMON_ATTR_ID    9100 /*6699*/
#define LAST_COMMON_ATTR_ID     10100 /*6807*/ /* 100 common attributes.      */
#define FIRST_LOCAL_ATTR_ID     11100 /*6808*/
#define LAST_LOCAL_ATTR_ID      12100 /*6907*/ /* 100 local attributes        */  /* was 2649 till june 23 2015 */
#define ACTION                  13100 /*6908*/ /* denotes the current verb.   */  /* 2650 */
#define FIRST_VERB_ID           13101 /*6909*/                                    /* 2651 */
#define LAST_VERB_ID            14100 /*7908*/ /* 1000 verbs.                 */  /* 2750 */
#define FIRST_WORD_ID           14101 /*8008*/ /* verbs and words must be     */  /* 2751 */
                                     /* one range. */
#define LAST_WORD_ID           15100 /*11306*/ /* 3000 other words.           */  /* 3050 */
#define NO_ID                  15101 /*11307*/                                    /* 3051 */
#define UNKNOWN_WORD           15102 /*11308*/                                    /* 3052 */
#define EOS                    15109 /*11309*/ /* End Of String; make sure    */  /* 3053 */
                                     /* this value is NOT within    */
                                     /* word/verb id range.         */
#endif
