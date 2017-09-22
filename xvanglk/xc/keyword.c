
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "keyword.h"


/*************************/
/* function declarations */
/*************************/

char    *strlwr(char*);
int32_t IsTestFun(int32_t);
int32_t IsIntAct(int32_t);
int32_t IsValidKeyword(int32_t);
int32_t CheckIfKeyword(char*);


char *strlwr(str)
 char *str;
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower(*p);
      p++;
  }
  return str;
}

int32_t IsTestFun(code)
 int32_t code;
{
  if ((code > LOWER_BOUND_TESTFUN) && (code < UPPER_BOUND_TESTFUN))
    return (OK);
  else
    return (ERROR);
}

int32_t IsIntAct(code)
 int32_t code;
{
  if ((code > LOWER_BOUND_INT_ACT) && (code < UPPER_BOUND_INT_ACT))
    return (OK);
  else
    return (ERROR);
}


int32_t IsValidKeyword(kw)
 int32_t kw;
{
  /* added on sep 4th 2017 */
  /* This function checks whether kw is within 2    */
  /* ranges. If so, the keyword is a valid keyword  */
  /* that may follow a section that was just parsed */
  /* We need this because we no longer have one     */
  /* predefined flow with a state machine now that  */
  /* we allow inserted files.                       */

 return(
  (kw == EOF)
  ||
   (kw >= MIN_VALID_COMPILER_KEYWORD && kw <= MAX_VALID_COMPILER_KEYWORD)
  ||
  (kw >= MIN_VALID_VOC_KEYWORD && kw <= MAX_VALID_VOC_KEYWORD)
  );
}


int32_t CheckIfKeyword(word)
 char *word;
{
  char word_lwr[MAX_WORD_LEN+1];

  /* all words are converted to lowercase, but */
  /* we must preserve the original case for    */
  /* printing the word by the interpreter.     */

  strncpy(word_lwr, word, MAX_WORD_LEN);
  strlwr(word_lwr);

  if (strcmp(word_lwr, "cansee") == 0)
    return(CANSEE);
  if (strcmp(word_lwr, "owns") == 0)
    return(OWNS);
  if (strcmp(word_lwr, "islit") == 0)
    return(ISLIT);
  if (strcmp(word_lwr, "count") == 0)
    return(COUNT);
  if (strcmp(word_lwr, "testflag") == 0)
    return(TESTFLAG);
  if (strcmp(word_lwr, "equal") == 0)
    return(EQUAL);
  if (strcmp(word_lwr, "lt") == 0)
    return(LESS_THAN);
  if (strcmp(word_lwr, "gt") == 0)
    return(GREATER_THAN);
  if (strcmp(word_lwr, "valdir") == 0)
    return(VALID_DIRECTION);
  if (strcmp(word_lwr, "yesno") == 0)
    return(YES_NO);
  if (strcmp(word_lwr, "hitanykey") == 0)
    return(HITANYKEY);
  if (strcmp(word_lwr, "add") == 0)
    return(ADD);
  if (strcmp(word_lwr, "eq_add") == 0)
    return(EQ_ADD);
  if (strcmp(word_lwr, "exit") == 0)
    return(EXIT);
  if (strcmp(word_lwr, "setflag") == 0)
    return(SETFLAG);
  if (strcmp(word_lwr, "clearflag") == 0)
    return(CLEARFLAG);
  if (strcmp(word_lwr, "setattribute") == 0)
    return(SETATTRIBUTE);
  if (strcmp(word_lwr, "settimer") == 0)
    return(SETTIMER);
  if (strcmp(word_lwr, "starttimer") == 0)
    return(STARTTIMER);
  if (strcmp(word_lwr, "stoptimer") == 0)
    return(STOPTIMER);
  if (strcmp(word_lwr, "move") == 0)
    return(MOVE);
  if (strcmp(word_lwr, "owner") == 0)
    return(OWNER);
  if (strcmp(word_lwr, "print") == 0)
    return(PRINT);
  if (strcmp(word_lwr, "printbold") == 0)
    return(PRINTBOLD);
  if (strcmp(word_lwr, "printitalic") == 0)
    return(PRINTITALIC);
  if (strcmp(word_lwr, "printcr") == 0)
    return(PRINTCR);
  if (strcmp(word_lwr, "printcrbold") == 0)
    return(PRINTCRBOLD);
  if (strcmp(word_lwr, "printcritalic") == 0)
    return(PRINTCRITALIC);
  if (strcmp(word_lwr, "printstatus") == 0)
    return(PRINTSTATUS);
  if (strcmp(word_lwr, "printcrstatus") == 0)
    return(PRINTCRSTATUS);
  if (strcmp(word_lwr, "setcursor") == 0)
    return(SETCURSOR);
  if (strcmp(word_lwr, "clearstatus") == 0)
    return(CLEARSTATUS);
  if (strcmp(word_lwr, "clearscreen") == 0)
    return(CLEARSCREEN);
  if (strcmp(word_lwr, "wait") == 0)
    return(WAIT);
  if (strcmp(word_lwr, "synchronize") == 0)
    return(SYNCHRONIZE);
  if (strcmp(word_lwr, "agree") == 0)
    return(AGREE);
  if (strcmp(word_lwr, "disagree") == 0)
    return(DISAGREE);
  if (strcmp(word_lwr, "nomatch") == 0)
    return(NOMATCH);
  if (strcmp(word_lwr, "indent") == 0)
    return(INDENT);
  if (strcmp(word_lwr, "quit") == 0)
    return(QUIT);
  if (strcmp(word_lwr, "contents") == 0)
    return(CONTENTS);
  if (strcmp(word_lwr, "entrance") == 0)
    return(ENTRANCE);
  if (strcmp(word_lwr, "trigger") == 0)
    return(TRIGGER);
  if (strcmp(word_lwr, "newexit") == 0)
    return(NEW_EXIT);
  if (strcmp(word_lwr, "blockexit") == 0)
    return(BLOCK_EXIT);
  if (strcmp(word_lwr, "getsubject") == 0)
    return(GET_SUBJECT);
  if (strcmp(word_lwr, "getspec") == 0)
    return(GET_SPECIFIER);
  if (strcmp(word_lwr, "getanswer") == 0)
    return(GET_ANSWER);
  if (strcmp(word_lwr, "goto") == 0)
    return(GO_TO);
  if (strcmp(word_lwr, "background") == 0)
   return(BACKGROUND);
  if (strcmp(word_lwr, "text") == 0)
    return(TEXT);
  if (strcmp(word_lwr, "bold") == 0)
    return(BOLD);
  if (strcmp(word_lwr, "italic") == 0)
    return(ITALIC);
  if (strcmp(word_lwr, "underline") == 0)
    return(UNDERLINE);
  if (strcmp(word_lwr, "testmode") == 0)
    return(TESTMODE);
  if (strcmp(word_lwr, "transcript") == 0)
    return(TRANSCRIPT);
  if (strcmp(word_lwr, "debug") == 0)
    return(DEBUG);
  if (strcmp(word_lwr, "distance") == 0)
    return(DISTANCE);
  if (strcmp(word_lwr, "firstdir") == 0)
    return(FIRSTDIR);
  if (strcmp(word_lwr, "dest") == 0)
    return(DEST);
  if (strcmp(word_lwr, "shuffle") == 0)
    return(SHUFFLE);
  if (strcmp(word_lwr, "(") == 0)
    return(LEFT_PAR);
  if (strcmp(word_lwr, ")") == 0)
    return(RIGHT_PAR);
  if (strcmp(word_lwr, "=") == 0)
    return(EQUAL_SIGN);
  if (strcmp(word_lwr, "+") == 0)
    return(PLUS);
  if (strcmp(word_lwr, "-") == 0)
    return(MINUS);
  if (strcmp(word_lwr, "*") == 0)
    return(ASTERIX);
  if (strcmp(word_lwr, "div") == 0)
    return(QUOT);
  if (strcmp(word_lwr, "rem") == 0)
    return(REM);
  if (strcmp(word_lwr, "rnd") == 0)
    return(RAND);
  if (strcmp(word_lwr, ",") == 0)
    return(COMMA);
  if (strcmp(word_lwr, ".") == 0)
    return(DOT);
  if (strcmp(word_lwr, "%") == 0)
    return(PAR_DELIMITER);
  if (strcmp(word_lwr, "->") == 0)
    return(ARROW);
  if (strcmp(word_lwr, "and") == 0)
    return(AND);
  if (strcmp(word_lwr, "or") == 0)
    return(OR);
  if (strcmp(word_lwr, "not") == 0)
    return(NOT);
  if (strcmp(word_lwr, "if") == 0)
    return(IF);
  if (strcmp(word_lwr, "then") == 0)
    return(THEN);
  if (strcmp(word_lwr, "else") == 0)
    return(ELSE);

  if (strncmp(word_lwr, "l_", 2) == 0) {
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(LOC_ID);
  }
  if (strncmp(word_lwr, "o_", 2) == 0) {
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(OBJ_ID);
  }
  if (strncmp(word_lwr, "t_", 2) == 0) {
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(TRIGG_ID);
  }
  if (strncmp(word_lwr, "f_", 2) == 0) {
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(FLAG_ID);
  }
  if (strncmp(word_lwr, "r_", 2) == 0) {    /* added on July 1st 2015 */
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(ATTR_ID);
  }
  if (strncmp(word_lwr, "m_", 2) == 0) {
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(TIM_ID);
  }
  if (strncmp(word_lwr, "d_", 2) == 0) {
    if (strlen(word) > MAX_ID_LENGTH)
      word[MAX_ID_LENGTH] = '\0';
    return(DESCR_ID);
  }
  if (strcmp(word_lwr, "endif") == 0)
    return(ENDIF);
  if (strcmp(word_lwr, "up") == 0)
    return(UP);
  if (strcmp(word_lwr, "down") == 0)
    return(DOWN);
  if (strcmp(word_lwr, "title") == 0)
    return(TITLE);
  if (strcmp(word_lwr, "version") == 0)
    return(VERSION);
  if (strcmp(word_lwr, "vocab") == 0)
    return(VOCAB);
  if (strcmp(word_lwr, "$location") == 0)
    return(LOCATION);
  if (strcmp(word_lwr, "end_loc") == 0)
    return(END_LOC);
  if (strcmp(word_lwr, "$object") == 0)
    return(OBJECT);
  if (strcmp(word_lwr, "end_obj") == 0)
    return(END_OBJ);
  if (strcmp(word_lwr, "$timers") == 0)
    return(TIMERS);
  if (strcmp(word_lwr, "descriptions") == 0)
    return(DESCRIPTIONS);
  if (strcmp(word_lwr, "exits") == 0)
    return(EXITS);
  if (strcmp(word_lwr, "$common_descrs") == 0)
    return(COMMON_DESCRS);
  if (strcmp(word_lwr, "$common_flags") == 0)
    return(COMMON_FLAGS);
  if (strcmp(word_lwr, "$common_attributes") == 0)
    return(COMMON_ATTRS);
  if (strcmp(word_lwr, "$common_triggers") == 0)
    return(COMMON_TRIGGERS);
  if (strcmp(word_lwr, "attributes") == 0)
    return(ATTRIBUTES);
  if (strcmp(word_lwr, "flags") == 0)
    return(FLAGS);
  if (strcmp(word_lwr, "triggers") == 0)
    return(TRIGGERS);
  if (strcmp(word_lwr, "contained") == 0)
    return(CONTAINED);
  if (strcmp(word_lwr, "init") == 0)
    return(INIT);
  if (strcmp(word_lwr, "step") == 0)
    return(STEP);
  if (strcmp(word_lwr, "interval") == 0)
    return(INTERVAL);
  if (strcmp(word_lwr, "direction") == 0)
    return(DIRECTION);
  if (strcmp(word_lwr, "state") == 0)
    return(STATE);
  if (strcmp(word_lwr, "trigger_at") == 0)
    return(TRIGGER_AT);

  /***********************/
  /*** wildcards start ***/
  /***********************/

  if (strcmp(word_lwr, "%action") == 0)
    return(ACTION);
  if (strcmp(word_lwr, "%this") == 0)
    return(THIS);
  if (strcmp(word_lwr, "%none") == 0)
    return(NONE);
  if (strcmp(word_lwr, "%prepos") == 0)
    return(PREPOS);
  if (strcmp(word_lwr, "%dir") == 0)
    return(DIR);
  if (strcmp(word_lwr, "%value") == 0)
    return(VALUE);
  if (strcmp(word_lwr, "%ord") == 0)
    return(ORDINAL);
  if (strcmp(word_lwr, "%a") == 0)
    return(ARTICLE_A);
  if (strcmp(word_lwr, "%the") == 0)
    return(ARTICLE_THE);

  /*********************/
  /*** wildcards end ***/
  /*********************/

  if (strcmp(word_lwr, "exact") == 0)
    return(EXACT);
  if (strcmp(word_lwr, "or_more") == 0)
    return(OR_MORE);
  if (strcmp(word_lwr, "or_less") == 0)
    return(OR_LESS);
  if (strcmp(word_lwr, "execute") == 0)
    return(EXECUTE);
  if (strcmp(word_lwr, "go") == 0)
    return(GO);
  if (strcmp(word_lwr, "stop") == 0)
    return(STOP);
  if (strcmp(word_lwr, "$verb") == 0)
    return(VERB);
  if (strcmp(word_lwr, "endverb") == 0)
    return(ENDVERB);
  if (strcmp(word_lwr, "$nouns") == 0)
    return(NOUNS);
  if (strcmp(word_lwr, "$adjectives") == 0)
    return(ADJECTIVES);
  if (strcmp(word_lwr, "$prepositions") == 0)
    return(PREPOSITIONS);
  if (strcmp(word_lwr, "$adverbs") == 0)
    return(ADVERBS);
  if (strcmp(word_lwr, "$q_words") == 0)
    return(Q_WORDS);
  if (strcmp(word_lwr, "$conjunction") == 0)
    return(CONJUNCTION);
  if (strcmp(word_lwr, "$directions") == 0)
    return(DIRECTIONS);
  if (strcmp(word_lwr, "$articles") == 0)
    return(ARTICLES);
  if (strcmp(word_lwr, "synonym") == 0)
    return(SYNONYM);
  if (strcmp(word_lwr, "scope") == 0)
    return(SCOPE);
  if (strcmp(word_lwr, "actor_only") == 0)
    return(ACTOR_ONLY);
  if (strcmp(word_lwr, "curr_loc_only") == 0)
    return(CURR_LOC_ONLY);
  if (strcmp(word_lwr, "all_locs") == 0)
    return(ALL_LOCS);
  if (strcmp(word_lwr, "default") == 0)
    return(DEFAULT);
  if (strcmp(word_lwr, "prologue") == 0)
    return(PROLOGUE);
  if (strcmp(word_lwr, "epilogue") == 0)
    return(EPILOGUE);
  if (strcmp(word_lwr, "save") == 0)
    return(SAVE);
  if (strcmp(word_lwr, "restore") == 0)
    return(RESTORE);
  if (strcmp(word_lwr, "disambiguation_rules") == 0)
    return(DISAMBIG_RULES);
  if (strcmp(word_lwr, "end_rules") == 0)
    return(END_RULES);
  if (strcmp(word_lwr, "score") == 0)
    return(SCORE);
  if (strcmp(word_lwr, "$insert") == 0)
    return(INSERT);
  if (strcmp(word_lwr, "$library") == 0)
    return(LIBRARY);
  return(NO_KEYWORD);
}

