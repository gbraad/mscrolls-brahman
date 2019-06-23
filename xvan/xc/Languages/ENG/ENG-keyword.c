
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.             */
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "keyword.h"


/*************************/
/* function declarations */
/*************************/

int32_t ENG_CheckIfKeyword(char*);


/************************/
/* function definitions */
/************************/

int32_t ENG_CheckIfKeyword(char *word_lwr)
{
  /* In ReadStoryInfo() the languages are read. But */
  /* we don't know where the languages are changed  */
  /* in the story info section. Therefore,          */
  /* ENG_ChekIfKeyword() must know the story info   */
  /* keywords in both languages.                    */

  /* story info keywords */
  if (strncmp(word_lwr, "story_info", MAX_WORD_LEN) == 0)
    return(STORY_INFO);
  if (strncmp(word_lwr, "verhaal_info", MAX_WORD_LEN) == 0)
    return(STORY_INFO);
  if (strncmp(word_lwr, "xvan_language", MAX_WORD_LEN) == 0)
    return(XVAN_LANGUAGE);
  if (strncmp(word_lwr, "xvan_taal", MAX_WORD_LEN) == 0)
    return(XVAN_LANGUAGE);
  if (strncmp(word_lwr, "story_language", MAX_WORD_LEN) == 0)
    return(STORY_LANGUAGE);
  if (strncmp(word_lwr, "story_taal", MAX_WORD_LEN) == 0)
    return(STORY_LANGUAGE);
  if (strncmp(word_lwr, "english", MAX_WORD_LEN) == 0)
    return(ENG);
  if (strncmp(word_lwr, "eng", MAX_WORD_LEN) == 0)
    return(ENG);
  if (strncmp(word_lwr, "nederlands", MAX_WORD_LEN) == 0)
    return(NL);
  if (strncmp(word_lwr, "nl", MAX_WORD_LEN) == 0)
    return(NL);
  if (strncmp(word_lwr, "dutch", MAX_WORD_LEN) == 0)
    return(NL);
  if (strncmp(word_lwr, "title", MAX_WORD_LEN) == 0)
   return(TITLE);
  if (strncmp(word_lwr, "titel", MAX_WORD_LEN) == 0)
    return(TITLE);
  if (strncmp(word_lwr, "version", MAX_WORD_LEN) == 0)
    return(VERSION);
  if (strncmp(word_lwr, "versie", MAX_WORD_LEN) == 0)
    return(VERSION);
  if (strncmp(word_lwr, "author", MAX_WORD_LEN) == 0)
    return(AUTHOR);
  if (strncmp(word_lwr, "auteur", MAX_WORD_LEN) == 0)
    return(AUTHOR);
 if (strncmp(word_lwr, "organization", MAX_WORD_LEN) == 0)
    return(ORGANIZATION);
 if (strncmp(word_lwr, "organisatie", MAX_WORD_LEN) == 0)
    return(ORGANIZATION);
  if (strncmp(word_lwr, "covertext", MAX_WORD_LEN) == 0)
    return(COVERTEXT);
  if (strncmp(word_lwr, "kafttekst", MAX_WORD_LEN) == 0)
    return(COVERTEXT);
  if (strncmp(word_lwr, "credits", MAX_WORD_LEN) == 0)
    return(CREDITS);
  if (strncmp(word_lwr, "android_market", MAX_WORD_LEN) == 0)
    return(ANDROID_MARKET);
  if (strncmp(word_lwr, "ios_market", MAX_WORD_LEN) == 0)
    return(IOS_MARKET);
  if (strncmp(word_lwr, "backimage", MAX_WORD_LEN) == 0)
    return(BACKIMAGE);
  if (strncmp(word_lwr, "achtergrond", MAX_WORD_LEN) == 0)
    return(BACKIMAGE);
  if (strncmp(word_lwr, "effect", MAX_WORD_LEN) == 0)
    return(EFFECT);
  if (strncmp(word_lwr, "no_sidebar", MAX_WORD_LEN) == 0)
    return(NO_SIDEBAR);
  if (strncmp(word_lwr, "geen_sidebar", MAX_WORD_LEN) == 0)
    return(NO_SIDEBAR);
  if (strncmp(word_lwr, "no_textinput", MAX_WORD_LEN) == 0)
    return(NO_TEXTINPUT);
  if (strncmp(word_lwr, "geen_tekstinvoer", MAX_WORD_LEN) == 0)
    return(NO_TEXTINPUT);
  if (strncmp(word_lwr, "no_compass", MAX_WORD_LEN) == 0)
    return(NO_COMPASS);
  if (strncmp(word_lwr, "geen_kompas", MAX_WORD_LEN) == 0)
    return(NO_COMPASS);
  if (strncmp(word_lwr, "autolink", MAX_WORD_LEN) == 0)
    return(AUTOLINK);
  if (strncmp(word_lwr, "primary_color", MAX_WORD_LEN) == 0)
    return(PRIMARY_COLOR);
  if (strncmp(word_lwr, "hoofdkleur", MAX_WORD_LEN) == 0)
    return(PRIMARY_COLOR);

  /* end of story info keywords */

  if (strncmp(word_lwr, "cansee", MAX_WORD_LEN) == 0)
    return(CANSEE);
  if (strncmp(word_lwr, "owns", MAX_WORD_LEN) == 0)
    return(OWNS);
  if (strncmp(word_lwr, "islit", MAX_WORD_LEN) == 0)
    return(ISLIT);
  if (strncmp(word_lwr, "count", MAX_WORD_LEN) == 0)
    return(COUNT);
  if (strncmp(word_lwr, "testflag", MAX_WORD_LEN) == 0)
    return(TESTFLAG);
  if (strncmp(word_lwr, "equal", MAX_WORD_LEN) == 0)
    return(EQUAL);
  if (strncmp(word_lwr, "lt", MAX_WORD_LEN) == 0)
    return(LESS_THAN);
  if (strncmp(word_lwr, "gt", MAX_WORD_LEN) == 0)
    return(GREATER_THAN);
  if (strncmp(word_lwr, "valdir", MAX_WORD_LEN) == 0)
    return(VALID_DIRECTION);
  if (strncmp(word_lwr, "yesno", MAX_WORD_LEN) == 0)
    return(YES_NO);
  if (strncmp(word_lwr, "runverb", MAX_WORD_LEN) == 0)    /* 05oct2017 */
    return(RUNVERB);
  if (strncmp(word_lwr, "runcommon", MAX_WORD_LEN) == 0)  /* 05oct2017 */
    return(RUNCOMMON);
  if (strncmp(word_lwr, "try", MAX_WORD_LEN) == 0)  /* 21jan2018 */
    return(TRY);
  if (strncmp(word_lwr, "hitanykey", MAX_WORD_LEN) == 0)
    return(HITANYKEY);
  if (strncmp(word_lwr, "add", MAX_WORD_LEN) == 0)
    return(ADD);
  if (strncmp(word_lwr, "eq_add", MAX_WORD_LEN) == 0)
    return(EQ_ADD);
  if (strncmp(word_lwr, "exit", MAX_WORD_LEN) == 0)
    return(EXIT);
  if (strncmp(word_lwr, "setflag", MAX_WORD_LEN) == 0)
    return(SETFLAG);
  if (strncmp(word_lwr, "clearflag", MAX_WORD_LEN) == 0)
    return(CLEARFLAG);
  if (strncmp(word_lwr, "setattribute", MAX_WORD_LEN) == 0)
    return(SETATTRIBUTE);
  if (strncmp(word_lwr, "settimer", MAX_WORD_LEN) == 0)
    return(SETTIMER);
  if (strncmp(word_lwr, "starttimer", MAX_WORD_LEN) == 0)
    return(STARTTIMER);
  if (strncmp(word_lwr, "stoptimer", MAX_WORD_LEN) == 0)
    return(STOPTIMER);
  if (strncmp(word_lwr, "move", MAX_WORD_LEN) == 0)
    return(MOVE);
  if (strncmp(word_lwr, "owner", MAX_WORD_LEN) == 0)
    return(OWNER);
  if (strncmp(word_lwr, "print", MAX_WORD_LEN) == 0)
    return(PRINT);
  if (strncmp(word_lwr, "printbold", MAX_WORD_LEN) == 0)
    return(PRINTBOLD);
  if (strncmp(word_lwr, "printitalic", MAX_WORD_LEN) == 0)
    return(PRINTITALIC);
  if (strncmp(word_lwr, "printcr", MAX_WORD_LEN) == 0)
    return(PRINTCR);
  if (strncmp(word_lwr, "printcrbold", MAX_WORD_LEN) == 0)
    return(PRINTCRBOLD);
  if (strncmp(word_lwr, "printcritalic", MAX_WORD_LEN) == 0)
    return(PRINTCRITALIC);
  if (strncmp(word_lwr, "printstatus", MAX_WORD_LEN) == 0)
    return(PRINTSTATUS);
  if (strncmp(word_lwr, "printcrstatus", MAX_WORD_LEN) == 0)
    return(PRINTCRSTATUS);
  if (strncmp(word_lwr, "setcursor", MAX_WORD_LEN) == 0)
    return(SETCURSOR);
  if (strncmp(word_lwr, "clearstatus", MAX_WORD_LEN) == 0)
    return(CLEARSTATUS);
  if (strncmp(word_lwr, "clearscreen", MAX_WORD_LEN) == 0)
    return(CLEARSCREEN);
  if (strncmp(word_lwr, "wait", MAX_WORD_LEN) == 0)
    return(WAIT);
  if (strncmp(word_lwr, "synchronize", MAX_WORD_LEN) == 0)
    return(SYNCHRONIZE);
  if (strncmp(word_lwr, "agree", MAX_WORD_LEN) == 0)
    return(AGREE);
  if (strncmp(word_lwr, "disagree", MAX_WORD_LEN) == 0)
    return(DISAGREE);
  if (strncmp(word_lwr, "nomatch", MAX_WORD_LEN) == 0)
    return(NOMATCH);
  if (strncmp(word_lwr, "indent", MAX_WORD_LEN) == 0)
    return(INDENT);
  if (strncmp(word_lwr, "quit", MAX_WORD_LEN) == 0)
    return(QUIT);
  if (strncmp(word_lwr, "contents", MAX_WORD_LEN) == 0)
    return(CONTENTS);
  if (strncmp(word_lwr, "entrance", MAX_WORD_LEN) == 0)
    return(ENTRANCE);
  if (strncmp(word_lwr, "trigger", MAX_WORD_LEN) == 0)
    return(TRIGGER);
  if (strncmp(word_lwr, "newexit", MAX_WORD_LEN) == 0)
    return(NEW_EXIT);
  if (strncmp(word_lwr, "blockexit", MAX_WORD_LEN) == 0)
    return(BLOCK_EXIT);
  if (strncmp(word_lwr, "getsubject", MAX_WORD_LEN) == 0)
    return(GET_SUBJECT);
  if (strncmp(word_lwr, "getspec", MAX_WORD_LEN) == 0)
    return(GET_SPECIFIER);
  if (strncmp(word_lwr, "getanswer", MAX_WORD_LEN) == 0)
    return(GET_ANSWER);
  if (strncmp(word_lwr, "goto", MAX_WORD_LEN) == 0)
    return(GO_TO);
  if (strncmp(word_lwr, "background", MAX_WORD_LEN) == 0)
   return(BACKGROUND);
  if (strncmp(word_lwr, "text", MAX_WORD_LEN) == 0)
    return(TEXT);
  if (strncmp(word_lwr, "bold", MAX_WORD_LEN) == 0)
    return(BOLD);
  if (strncmp(word_lwr, "italic", MAX_WORD_LEN) == 0)
    return(ITALIC);
  if (strncmp(word_lwr, "underline", MAX_WORD_LEN) == 0)
    return(UNDERLINE);
  if (strncmp(word_lwr, "testmode", MAX_WORD_LEN) == 0)
    return(TESTMODE);
  if (strncmp(word_lwr, "transcript", MAX_WORD_LEN) == 0)
    return(TRANSCRIPT);
  if (strncmp(word_lwr, "debug", MAX_WORD_LEN) == 0)
    return(DEBUG);
  if (strncmp(word_lwr, "distance", MAX_WORD_LEN) == 0)
    return(DISTANCE);
  if (strncmp(word_lwr, "firstdir", MAX_WORD_LEN) == 0)
    return(FIRSTDIR);
  if (strncmp(word_lwr, "dest", MAX_WORD_LEN) == 0)
    return(DEST);
  if (strncmp(word_lwr, "shuffle", MAX_WORD_LEN) == 0)
    return(SHUFFLE);
  if (strncmp(word_lwr, "restart", MAX_WORD_LEN) == 0)
    return(RESTART);
  if (strncmp(word_lwr, "pickone", MAX_WORD_LEN) == 0)
    return(PICKONE);
  if (strncmp(word_lwr, "(", MAX_WORD_LEN) == 0)
    return(LEFT_PAR);
  if (strncmp(word_lwr, ")", MAX_WORD_LEN) == 0)
    return(RIGHT_PAR);
  if (strncmp(word_lwr, "=", MAX_WORD_LEN) == 0)
    return(EQUAL_SIGN);
  if (strncmp(word_lwr, "+", MAX_WORD_LEN) == 0)
    return(PLUS);
  if (strncmp(word_lwr, "-", MAX_WORD_LEN) == 0)
    return(MINUS);
  if (strncmp(word_lwr, "*", MAX_WORD_LEN) == 0)
    return(ASTERIX);
  if (strncmp(word_lwr, "div", MAX_WORD_LEN) == 0)
    return(QUOT);
  if (strncmp(word_lwr, "rem", MAX_WORD_LEN) == 0)
    return(REM);
  if (strncmp(word_lwr, "rnd", MAX_WORD_LEN) == 0)
    return(RAND);
  if (strncmp(word_lwr, ",", MAX_WORD_LEN) == 0)
    return(COMMA);
  if (strncmp(word_lwr, ".", MAX_WORD_LEN) == 0)
    return(DOT);
  if (strncmp(word_lwr, "%", MAX_WORD_LEN) == 0)
    return(PAR_DELIMITER);
  if (strncmp(word_lwr, "->", MAX_WORD_LEN) == 0)
    return(ARROW);
  if (strncmp(word_lwr, "and", MAX_WORD_LEN) == 0)
    return(AND);
  if (strncmp(word_lwr, "or", MAX_WORD_LEN) == 0)
    return(OR);
  if (strncmp(word_lwr, "not", MAX_WORD_LEN) == 0)
    return(NOT);
  if (strncmp(word_lwr, "if", MAX_WORD_LEN) == 0)
    return(IF);
  if (strncmp(word_lwr, "then", MAX_WORD_LEN) == 0)
    return(THEN);
  if (strncmp(word_lwr, "else", MAX_WORD_LEN) == 0)
    return(ELSE);

  if (strncmp(word_lwr, "l_", 2) == 0) {
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(LOC_ID);
  }
  if (strncmp(word_lwr, "o_", 2) == 0) {
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(OBJ_ID);
  }
  if (strncmp(word_lwr, "t_", 2) == 0) {
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(TRIGG_ID);
  }
  if (strncmp(word_lwr, "f_", 2) == 0) {
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(FLAG_ID);
  }
  if (strncmp(word_lwr, "r_", 2) == 0) {    /* added on July 1st 2015 */
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(ATTR_ID);
  }
  if (strncmp(word_lwr, "m_", 2) == 0) {
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(TIM_ID);
  }
  if (strncmp(word_lwr, "d_", 2) == 0) {
    if (strlen(word_lwr) > MAX_ID_LENGTH)
      word_lwr[MAX_ID_LENGTH] = '\0';
    return(DESCR_ID);
  }
  if (strncmp(word_lwr, "endif", MAX_WORD_LEN) == 0)
    return(ENDIF);
  if (strncmp(word_lwr, "up", MAX_WORD_LEN) == 0)
    return(UP);
  if (strncmp(word_lwr, "down", MAX_WORD_LEN) == 0)
    return(DOWN);
  if (strncmp(word_lwr, "title", MAX_WORD_LEN) == 0)
    return(TITLE);
  if (strncmp(word_lwr, "version", MAX_WORD_LEN) == 0)
    return(VERSION);
  if (strncmp(word_lwr, "vocab", MAX_WORD_LEN) == 0)
    return(VOCAB);
  if (strncmp(word_lwr, "$location", MAX_WORD_LEN) == 0)
    return(LOCATION);
  if (strncmp(word_lwr, "end_loc", MAX_WORD_LEN) == 0)
    return(END_LOC);
  if (strncmp(word_lwr, "$object", MAX_WORD_LEN) == 0)
    return(OBJECT);
  if (strncmp(word_lwr, "end_obj", MAX_WORD_LEN) == 0)
    return(END_OBJ);
  if (strncmp(word_lwr, "$timers", MAX_WORD_LEN) == 0)
    return(TIMERS);
  if (strncmp(word_lwr, "descriptions", MAX_WORD_LEN) == 0)
    return(DESCRIPTIONS);
  if (strncmp(word_lwr, "exits", MAX_WORD_LEN) == 0)
    return(EXITS);
  if (strncmp(word_lwr, "$common_descrs", MAX_WORD_LEN) == 0)
    return(COMMON_DESCRS);
  if (strncmp(word_lwr, "$common_flags", MAX_WORD_LEN) == 0)
    return(COMMON_FLAGS);
  if (strncmp(word_lwr, "$redefine_flags", MAX_WORD_LEN) == 0)
    return(REDEFINE_FLAGS);
  if (strncmp(word_lwr, "$common_attributes", MAX_WORD_LEN) == 0)
    return(COMMON_ATTRS);
  if (strncmp(word_lwr, "$redefine_attributes", MAX_WORD_LEN) == 0)
    return(REDEFINE_ATTRS);
  if (strncmp(word_lwr, "$common_triggers", MAX_WORD_LEN) == 0)
    return(COMMON_TRIGGERS);
  if (strncmp(word_lwr, "$redefine_triggers", MAX_WORD_LEN) == 0)
    return(REDEFINE_TRIGGERS);
  if (strncmp(word_lwr, "attributes", MAX_WORD_LEN) == 0)
    return(ATTRIBUTES);
  if (strncmp(word_lwr, "flags", MAX_WORD_LEN) == 0)
    return(FLAGS);
  if (strncmp(word_lwr, "triggers", MAX_WORD_LEN) == 0)
    return(TRIGGERS);
  if (strncmp(word_lwr, "contained", MAX_WORD_LEN) == 0)
    return(CONTAINED);
  if (strncmp(word_lwr, "init", MAX_WORD_LEN) == 0)
    return(INIT);
  if (strncmp(word_lwr, "step", MAX_WORD_LEN) == 0)
    return(STEP);
  if (strncmp(word_lwr, "interval", MAX_WORD_LEN) == 0)
    return(INTERVAL);
  if (strncmp(word_lwr, "direction", MAX_WORD_LEN) == 0)
    return(DIRECTION);
  if (strncmp(word_lwr, "state", MAX_WORD_LEN) == 0)
    return(STATE);
  if (strncmp(word_lwr, "trigger_at", MAX_WORD_LEN) == 0)
    return(TRIGGER_AT);

  /***********************/
  /*** wildcards start ***/
  /***********************/

  if (strncmp(word_lwr, "%action", MAX_WORD_LEN) == 0)
    return(ACTION);
  if (strncmp(word_lwr, "%this", MAX_WORD_LEN) == 0)
    return(THIS);
  if (strncmp(word_lwr, "%none", MAX_WORD_LEN) == 0)
    return(NONE);
  if (strncmp(word_lwr, "%prepos", MAX_WORD_LEN) == 0)
    return(PREPOS);
  if (strncmp(word_lwr, "%dir", MAX_WORD_LEN) == 0)
    return(DIR);
  if (strncmp(word_lwr, "%value", MAX_WORD_LEN) == 0)
    return(VALUE);
  if (strncmp(word_lwr, "%ord", MAX_WORD_LEN) == 0)
    return(ORDINAL);
  if (strncmp(word_lwr, "%a", MAX_WORD_LEN) == 0)
    return(ARTICLE_A);
  if (strncmp(word_lwr, "%the", MAX_WORD_LEN) == 0)
    return(ARTICLE_THE);

  /*********************/
  /*** wildcards end ***/
  /*********************/

  if (strncmp(word_lwr, "exact", MAX_WORD_LEN) == 0)
    return(EXACT);
  if (strncmp(word_lwr, "or_more", MAX_WORD_LEN) == 0)
    return(OR_MORE);
  if (strncmp(word_lwr, "or_less", MAX_WORD_LEN) == 0)
    return(OR_LESS);
  if (strncmp(word_lwr, "execute", MAX_WORD_LEN) == 0)
    return(EXECUTE);
  if (strncmp(word_lwr, "go", MAX_WORD_LEN) == 0)
    return(GO);
  if (strncmp(word_lwr, "stop", MAX_WORD_LEN) == 0)
    return(STOP);
  if (strncmp(word_lwr, "$verb", MAX_WORD_LEN) == 0)
    return(VERB);
  if (strncmp(word_lwr, "$redefine_verb", MAX_WORD_LEN) == 0)
    return(REDEFINE_VERB);
  if (strncmp(word_lwr, "endverb", MAX_WORD_LEN) == 0)
    return(ENDVERB);
  if (strncmp(word_lwr, "$nouns", MAX_WORD_LEN) == 0)
    return(NOUNS);
  if (strncmp(word_lwr, "$adjectives", MAX_WORD_LEN) == 0)
    return(ADJECTIVES);
  if (strncmp(word_lwr, "$prepositions", MAX_WORD_LEN) == 0)
    return(PREPOSITIONS);
  if (strncmp(word_lwr, "$adverbs", MAX_WORD_LEN) == 0)
    return(ADVERBS);
  if (strncmp(word_lwr, "$q_words", MAX_WORD_LEN) == 0)
    return(Q_WORDS);
  if (strncmp(word_lwr, "$conjunction", MAX_WORD_LEN) == 0)
    return(CONJUNCTION);
  if (strncmp(word_lwr, "$directions", MAX_WORD_LEN) == 0)
    return(DIRECTIONS);
  if (strncmp(word_lwr, "$articles", MAX_WORD_LEN) == 0)
    return(ARTICLES);
  if (strncmp(word_lwr, "synonym", MAX_WORD_LEN) == 0)
    return(SYNONYM);
  if (strncmp(word_lwr, "scope", MAX_WORD_LEN) == 0)
    return(SCOPE);
  if (strncmp(word_lwr, "actor_only", MAX_WORD_LEN) == 0)
    return(ACTOR_ONLY);
  if (strncmp(word_lwr, "curr_loc_only", MAX_WORD_LEN) == 0)
    return(CURR_LOC_ONLY);
  if (strncmp(word_lwr, "all_locs", MAX_WORD_LEN) == 0)
    return(ALL_LOCS);
  if (strncmp(word_lwr, "default", MAX_WORD_LEN) == 0)
    return(DEFAULT);
  if (strncmp(word_lwr, "prologue", MAX_WORD_LEN) == 0)
    return(PROLOGUE);
  if (strncmp(word_lwr, "epilogue", MAX_WORD_LEN) == 0)
    return(EPILOGUE);
  if (strncmp(word_lwr, "save", MAX_WORD_LEN) == 0)
    return(SAVE);
  if (strncmp(word_lwr, "restore", MAX_WORD_LEN) == 0)
    return(RESTORE);
  if (strncmp(word_lwr, "disambiguation_rules", MAX_WORD_LEN) == 0)
    return(DISAMBIG_RULES);
  if (strncmp(word_lwr, "end_rules", MAX_WORD_LEN) == 0)
    return(END_RULES);
  if (strncmp(word_lwr, "score", MAX_WORD_LEN) == 0)
    return(SCORE);
  if (strncmp(word_lwr, "$insert", MAX_WORD_LEN) == 0)
    return(INSERT);
  if (strncmp(word_lwr, "$library", MAX_WORD_LEN) == 0)
    return(LIBRARY);
  if (strncmp(word_lwr, "isobject", MAX_WORD_LEN) == 0)
    return(ISOBJECT);
  if (strncmp(word_lwr, "addjson", MAX_WORD_LEN) == 0)
    return(ADDJSON);
  if (strncmp(word_lwr, "clearjson", MAX_WORD_LEN) == 0)
    return(CLEARJSON);
  if (strncmp(word_lwr, "sendjson", MAX_WORD_LEN) == 0)
    return(SENDJSON);
  if (strncmp(word_lwr, "notimers", MAX_WORD_LEN) == 0)
    return(NOTIMERS);
  return(NO_KEYWORD);
}
