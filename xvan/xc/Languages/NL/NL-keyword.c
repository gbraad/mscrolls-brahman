
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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

int32_t NL_CheckIfKeyword(char*);

/************************/
/* function definitions */
/************************/

int32_t NL_CheckIfKeyword(char *word_lwr)
{
  /* 10may2018: story info keywords added, because if the */
  /* language is changed, the keywords won't be recognized */

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
  if (strncmp(word_lwr, "choice_mode", MAX_WORD_LEN) == 0)
    return(CHOICE_MODE);
  if (strncmp(word_lwr, "keuzemodus", MAX_WORD_LEN) == 0)
    return(CHOICE_MODE);
  if (strncmp(word_lwr, "hybrid_mode", MAX_WORD_LEN) == 0)
    return(HYBRID_MODE);
  if (strncmp(word_lwr, "hybridemodus", MAX_WORD_LEN) == 0)
    return(HYBRID_MODE);

  /* end of story info keywords */

  if (strncmp(word_lwr, "kanzien", MAX_WORD_LEN) == 0)
    return(CANSEE);
  if (strncmp(word_lwr, "heeft", MAX_WORD_LEN) == 0)
    return(OWNS);
  if (strncmp(word_lwr, "verlicht", MAX_WORD_LEN) == 0)
    return(ISLIT);
  if (strncmp(word_lwr, "tel", MAX_WORD_LEN) == 0)
    return(COUNT);
  if (strncmp(word_lwr, "iswaar", MAX_WORD_LEN) == 0)
    return(TESTFLAG);
  if (strncmp(word_lwr, "hetzelfde", MAX_WORD_LEN) == 0)
    return(EQUAL);
  if (strncmp(word_lwr, "kleiner", MAX_WORD_LEN) == 0)
    return(LESS_THAN);
  if (strncmp(word_lwr, "groter", MAX_WORD_LEN) == 0)
    return(GREATER_THAN);
  if (strncmp(word_lwr, "uitgang", MAX_WORD_LEN) == 0)
    return(VALID_DIRECTION);
  if (strncmp(word_lwr, "janee", MAX_WORD_LEN) == 0)
    return(YES_NO);
  if (strncmp(word_lwr, "doe_werkwoord", MAX_WORD_LEN) == 0) /* 05oct2017 */
    return(RUNVERB);
  if (strncmp(word_lwr, "doe_algemeen", MAX_WORD_LEN) == 0)  /* 05oct2017 */
    return(RUNCOMMON);
  if (strncmp(word_lwr, "probeer", MAX_WORD_LEN) == 0)  /* 05oct2017 */
    return(TRY);
  if (strncmp(word_lwr, "toets", MAX_WORD_LEN) == 0)
    return(HITANYKEY);
  if (strncmp(word_lwr, "add", MAX_WORD_LEN) == 0)
    return(ADD);
  if (strncmp(word_lwr, "eq_add", MAX_WORD_LEN) == 0)
    return(EQ_ADD);
  if (strncmp(word_lwr, "verlaat", MAX_WORD_LEN) == 0)
    return(EXIT);
  if (strncmp(word_lwr, "maakwaar", MAX_WORD_LEN) == 0)
    return(SETFLAG);
  if (strncmp(word_lwr, "maakonwaar", MAX_WORD_LEN) == 0)
    return(CLEARFLAG);
  if (strncmp(word_lwr, "setattribute", MAX_WORD_LEN) == 0)
    return(SETATTRIBUTE);
  if (strncmp(word_lwr, "zettimer", MAX_WORD_LEN) == 0)
    return(SETTIMER);
  if (strncmp(word_lwr, "starttimer", MAX_WORD_LEN) == 0)
    return(STARTTIMER);
  if (strncmp(word_lwr, "stoptimer", MAX_WORD_LEN) == 0)
    return(STOPTIMER);
  if (strncmp(word_lwr, "verplaats", MAX_WORD_LEN) == 0)
    return(MOVE);
  if (strncmp(word_lwr, "eigenaar", MAX_WORD_LEN) == 0)
    return(OWNER);
  if (strncmp(word_lwr, "print", MAX_WORD_LEN) == 0)
    return(PRINT);
  if (strncmp(word_lwr, "printvet", MAX_WORD_LEN) == 0)
    return(PRINTBOLD);
  if (strncmp(word_lwr, "printschuin", MAX_WORD_LEN) == 0)
    return(PRINTITALIC);
  if (strncmp(word_lwr, "printcr", MAX_WORD_LEN) == 0)
    return(PRINTCR);
  if (strncmp(word_lwr, "printcrvet", MAX_WORD_LEN) == 0)
    return(PRINTCRBOLD);
  if (strncmp(word_lwr, "printcrschuin", MAX_WORD_LEN) == 0)
    return(PRINTCRITALIC);
  if (strncmp(word_lwr, "printstatus", MAX_WORD_LEN) == 0)
    return(PRINTSTATUS);
  if (strncmp(word_lwr, "printcrstatus", MAX_WORD_LEN) == 0)
    return(PRINTCRSTATUS);
  if (strncmp(word_lwr, "plaatscursor", MAX_WORD_LEN) == 0)
    return(SETCURSOR);
  if (strncmp(word_lwr, "wisstatus", MAX_WORD_LEN) == 0)
    return(CLEARSTATUS);
  if (strncmp(word_lwr, "wisscherm", MAX_WORD_LEN) == 0)
    return(CLEARSCREEN);
  if (strncmp(word_lwr, "wacht", MAX_WORD_LEN) == 0)
    return(WAIT);
  if (strncmp(word_lwr, "samen", MAX_WORD_LEN) == 0)
    return(SYNCHRONIZE);
  if (strncmp(word_lwr, "klaar", MAX_WORD_LEN) == 0)
    return(AGREE);
  if (strncmp(word_lwr, "stop", MAX_WORD_LEN) == 0)
    return(DISAGREE);
  if (strncmp(word_lwr, "oeps", MAX_WORD_LEN) == 0)
    return(NOMATCH);
  if (strncmp(word_lwr, "inspringen", MAX_WORD_LEN) == 0)
    return(INDENT);
  if (strncmp(word_lwr, "quit", MAX_WORD_LEN) == 0)
    return(QUIT);
  if (strncmp(word_lwr, "inhoud", MAX_WORD_LEN) == 0)
    return(CONTENTS);
  if (strncmp(word_lwr, "naarbinnen", MAX_WORD_LEN) == 0)
    return(ENTRANCE);
  if (strncmp(word_lwr, "doe", MAX_WORD_LEN) == 0)
    return(TRIGGER);
  if (strncmp(word_lwr, "maakuitgang", MAX_WORD_LEN) == 0)
    return(NEW_EXIT);
  if (strncmp(word_lwr, "verwijderuitgang", MAX_WORD_LEN) == 0)
    return(BLOCK_EXIT);
  if (strncmp(word_lwr, "vraagding1", MAX_WORD_LEN) == 0)
    return(GET_SUBJECT);
  if (strncmp(word_lwr, "vraagding2", MAX_WORD_LEN) == 0)
    return(GET_SPECIFIER);
  if (strncmp(word_lwr, "getanswer", MAX_WORD_LEN) == 0)
    return(GET_ANSWER);
  if (strncmp(word_lwr, "ganaar", MAX_WORD_LEN) == 0)
    return(GO_TO);
  if (strncmp(word_lwr, "achtergrondkleur", MAX_WORD_LEN) == 0)
   return(BACKGROUND);
  if (strncmp(word_lwr, "tekstkleur", MAX_WORD_LEN) == 0)
    return(TEXT);
  if (strncmp(word_lwr, "vet", MAX_WORD_LEN) == 0)
    return(BOLD);
  if (strncmp(word_lwr, "schuin", MAX_WORD_LEN) == 0)
    return(ITALIC);
  if (strncmp(word_lwr, "onderstreept", MAX_WORD_LEN) == 0)
    return(UNDERLINE);
  if (strncmp(word_lwr, "testmode", MAX_WORD_LEN) == 0)
    return(TESTMODE);
  if (strncmp(word_lwr, "verslag", MAX_WORD_LEN) == 0)
    return(TRANSCRIPT);
  if (strncmp(word_lwr, "debug", MAX_WORD_LEN) == 0)
    return(DEBUG);
  if (strncmp(word_lwr, "afstand", MAX_WORD_LEN) == 0)
    return(DISTANCE);
  if (strncmp(word_lwr, "eersterichting", MAX_WORD_LEN) == 0)
    return(FIRSTDIR);
  if (strncmp(word_lwr, "bestemming", MAX_WORD_LEN) == 0)
    return(DEST);
  if (strncmp(word_lwr, "hussel", MAX_WORD_LEN) == 0)
    return(SHUFFLE);
  if (strncmp(word_lwr, "herstart", MAX_WORD_LEN) == 0)
    return(RESTART);
  if (strncmp(word_lwr, "eenvan", MAX_WORD_LEN) == 0)
    return(PICKONE);
  if (strncmp(word_lwr, "nieuwekeus", MAX_WORD_LEN) == 0)
    return(ADDCHOICE);
  if (strncmp(word_lwr, "speelmodus", MAX_WORD_LEN) == 0)
    return(PLAYMODE);
  if (strncmp(word_lwr, "herstel", MAX_WORD_LEN) == 0)  /* @!@ */
    return(UNDO);
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
  if (strncmp(word_lwr, "en", MAX_WORD_LEN) == 0)
    return(AND);
  if (strncmp(word_lwr, "of", MAX_WORD_LEN) == 0)
    return(OR);
  if (strncmp(word_lwr, "niet", MAX_WORD_LEN) == 0)
    return(NOT);
  if (strncmp(word_lwr, "als", MAX_WORD_LEN) == 0)
    return(IF);
  if (strncmp(word_lwr, "dan", MAX_WORD_LEN) == 0)
    return(THEN);
  if (strncmp(word_lwr, "anders", MAX_WORD_LEN) == 0)
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
  if (strncmp(word_lwr, "einde_als", MAX_WORD_LEN) == 0)
    return(ENDIF);
  if (strncmp(word_lwr, "op", MAX_WORD_LEN) == 0)
    return(UP);
  if (strncmp(word_lwr, "af", MAX_WORD_LEN) == 0)
    return(DOWN);
  if (strncmp(word_lwr, "titel", MAX_WORD_LEN) == 0)
    return(TITLE);
  if (strncmp(word_lwr, "versie", MAX_WORD_LEN) == 0)
    return(VERSION);
  if (strncmp(word_lwr, "vocab", MAX_WORD_LEN) == 0)
    return(VOCAB);
  if (strncmp(word_lwr, "$plaats", MAX_WORD_LEN) == 0)
    return(LOCATION);
  if (strncmp(word_lwr, "einde_plaats", MAX_WORD_LEN) == 0)
    return(END_LOC);
  if (strncmp(word_lwr, "$ding", MAX_WORD_LEN) == 0)
    return(OBJECT);
  if (strncmp(word_lwr, "einde_ding", MAX_WORD_LEN) == 0)
    return(END_OBJ);
  if (strncmp(word_lwr, "$timers", MAX_WORD_LEN) == 0)
    return(TIMERS);
  if (strncmp(word_lwr, "beschrijvingen", MAX_WORD_LEN) == 0)
    return(DESCRIPTIONS);
  if (strncmp(word_lwr, "uitgangen", MAX_WORD_LEN) == 0)
    return(EXITS);
  if (strncmp(word_lwr, "$alg_beschrijvingen", MAX_WORD_LEN) == 0)
    return(COMMON_DESCRS);
  if (strncmp(word_lwr, "$alg_vlaggen", MAX_WORD_LEN) == 0)
    return(COMMON_FLAGS);
  if (strncmp(word_lwr, "$vervang_vlaggen", MAX_WORD_LEN) == 0)
    return(REDEFINE_FLAGS);
  if (strncmp(word_lwr, "$alg_attributen", MAX_WORD_LEN) == 0)
    return(COMMON_ATTRS);
  if (strncmp(word_lwr, "$vervang_attributen", MAX_WORD_LEN) == 0)
    return(REDEFINE_ATTRS);
  if (strncmp(word_lwr, "$alg_triggers", MAX_WORD_LEN) == 0)
    return(COMMON_TRIGGERS);
  if (strncmp(word_lwr, "$vervang_triggers", MAX_WORD_LEN) == 0)
    return(REDEFINE_TRIGGERS);
  if (strncmp(word_lwr, "attributen", MAX_WORD_LEN) == 0)
    return(ATTRIBUTES);
  if (strncmp(word_lwr, "vlaggen", MAX_WORD_LEN) == 0)
    return(FLAGS);
  if (strncmp(word_lwr, "triggers", MAX_WORD_LEN) == 0)
    return(TRIGGERS);
  if (strncmp(word_lwr, "geplaatst", MAX_WORD_LEN) == 0)
    return(CONTAINED);
  if (strncmp(word_lwr, "startwaarde", MAX_WORD_LEN) == 0)
    return(INIT);
  if (strncmp(word_lwr, "stapgrootte", MAX_WORD_LEN) == 0)
    return(STEP);
  if (strncmp(word_lwr, "interval", MAX_WORD_LEN) == 0)
    return(INTERVAL);
  if (strncmp(word_lwr, "telt", MAX_WORD_LEN) == 0)
    return(DIRECTION);
  if (strncmp(word_lwr, "status", MAX_WORD_LEN) == 0)
    return(STATE);
  if (strncmp(word_lwr, "grenswaarde", MAX_WORD_LEN) == 0)
    return(TRIGGER_AT);

  /***********************/
  /*** wildcards start ***/
  /***********************/

  if (strncmp(word_lwr, "%actie", MAX_WORD_LEN) == 0)
    return(ACTION);
  if (strncmp(word_lwr, "%dit", MAX_WORD_LEN) == 0)
    return(THIS);
  if (strncmp(word_lwr, "%niets", MAX_WORD_LEN) == 0)
    return(NONE);
  if (strncmp(word_lwr, "%voorzetsel", MAX_WORD_LEN) == 0)
    return(PREPOS);
  if (strncmp(word_lwr, "%richting", MAX_WORD_LEN) == 0)
    return(DIR);
  if (strncmp(word_lwr, "%waarde", MAX_WORD_LEN) == 0)
    return(VALUE);
  if (strncmp(word_lwr, "%volgnummer", MAX_WORD_LEN) == 0)
    return(ORDINAL);
  if (strncmp(word_lwr, "%een", MAX_WORD_LEN) == 0)
    return(ARTICLE_A);
  if (strncmp(word_lwr, "%de", MAX_WORD_LEN) == 0)
    return(ARTICLE_THE);
  if (strncmp(word_lwr, "%het", MAX_WORD_LEN) == 0)
    return(ARTICLE_THE);

  /*********************/
  /*** wildcards end ***/
  /*********************/

  if (strncmp(word_lwr, "exact", MAX_WORD_LEN) == 0)
    return(EXACT);
  if (strncmp(word_lwr, "of_hoger", MAX_WORD_LEN) == 0)
    return(OR_MORE);
  if (strncmp(word_lwr, "of_lager", MAX_WORD_LEN) == 0)
    return(OR_LESS);
  if (strncmp(word_lwr, "uitvoeren", MAX_WORD_LEN) == 0)
    return(EXECUTE);
  if (strncmp(word_lwr, "aan", MAX_WORD_LEN) == 0)
    return(GO);
  if (strncmp(word_lwr, "uit", MAX_WORD_LEN) == 0)
    return(STOP);
  if (strncmp(word_lwr, "$actie", MAX_WORD_LEN) == 0)
    return(VERB);
  if (strncmp(word_lwr, "$vervang_actie", MAX_WORD_LEN) == 0)
    return(REDEFINE_VERB);
  if (strncmp(word_lwr, "einde_actie", MAX_WORD_LEN) == 0)
    return(ENDVERB);
  if (strncmp(word_lwr, "$znw", MAX_WORD_LEN) == 0)
    return(NOUNS);
  if (strncmp(word_lwr, "$bvnw", MAX_WORD_LEN) == 0)
    return(ADJECTIVES);
  if (strncmp(word_lwr, "$voorzetsels", MAX_WORD_LEN) == 0)
    return(PREPOSITIONS);
  if (strncmp(word_lwr, "$bijwoorden", MAX_WORD_LEN) == 0)
    return(ADVERBS);
  if (strncmp(word_lwr, "$vraagwoorden", MAX_WORD_LEN) == 0)
    return(Q_WORDS);
  if (strncmp(word_lwr, "$voegwoorden", MAX_WORD_LEN) == 0)
    return(CONJUNCTION);
  if (strncmp(word_lwr, "$richtingen", MAX_WORD_LEN) == 0)
    return(DIRECTIONS);
  if (strncmp(word_lwr, "$lidwoorden", MAX_WORD_LEN) == 0)
    return(ARTICLES);
  if (strncmp(word_lwr, "synoniem", MAX_WORD_LEN) == 0)
    return(SYNONYM);
  if (strncmp(word_lwr, "scope", MAX_WORD_LEN) == 0)
    return(SCOPE);
  if (strncmp(word_lwr, "alleen_acteur", MAX_WORD_LEN) == 0)
    return(ACTOR_ONLY);
  if (strncmp(word_lwr, "alleen_plaats", MAX_WORD_LEN) == 0)
    return(CURR_LOC_ONLY);
  if (strncmp(word_lwr, "alles", MAX_WORD_LEN) == 0)
    return(ALL_LOCS);
  if (strncmp(word_lwr, "default", MAX_WORD_LEN) == 0)
    return(DEFAULT);
  if (strncmp(word_lwr, "vooraf", MAX_WORD_LEN) == 0)
    return(PROLOGUE);
  if (strncmp(word_lwr, "achteraf", MAX_WORD_LEN) == 0)
    return(EPILOGUE);
  if (strncmp(word_lwr, "opslaan", MAX_WORD_LEN) == 0)
    return(SAVE);
  if (strncmp(word_lwr, "restore", MAX_WORD_LEN) == 0)
    return(RESTORE);
  if (strncmp(word_lwr, "zoekregels", MAX_WORD_LEN) == 0)
    return(DISAMBIG_RULES);
  if (strncmp(word_lwr, "einde_regels", MAX_WORD_LEN) == 0)
    return(END_RULES);
  if (strncmp(word_lwr, "score", MAX_WORD_LEN) == 0)
    return(SCORE);
  if (strncmp(word_lwr, "$invoegen", MAX_WORD_LEN) == 0)
    return(INSERT);
  if (strncmp(word_lwr, "$library", MAX_WORD_LEN) == 0)
    return(LIBRARY);
      if (strncmp(word_lwr, "isding", MAX_WORD_LEN) == 0)
    return(ISOBJECT);
  if (strncmp(word_lwr, "addjson", MAX_WORD_LEN) == 0)
    return(ADDJSON);
  if (strncmp(word_lwr, "clearjson", MAX_WORD_LEN) == 0)
    return(CLEARJSON);
  if (strncmp(word_lwr, "sendjson", MAX_WORD_LEN) == 0)
    return(SENDJSON);
  if (strncmp(word_lwr, "geentimers", MAX_WORD_LEN) == 0)
    return(NOTIMERS);
  return(NO_KEYWORD);
}
