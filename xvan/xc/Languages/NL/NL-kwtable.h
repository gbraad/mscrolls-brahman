
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

#if !defined(__NL_kwtable)
#define __NL_kwtable

#include "typedefs.h"

/* the keyword table is used to translate English #defines into Dutch   */
/* for displaying in error messages.                                    */
/* The keyword table must be sorted A->Z because we use a binary search */


kwTable kw_table[] =  {
  {.keyword_eng = "ACTOR_ONLY", .keyword_nl = "ALLEEN_ACTEUR"},
  {.keyword_eng = "ADDJSON", .keyword_nl = "ADDJSON"},     /* @@@ */
  {.keyword_eng = "ADJECTIVES", .keyword_nl = "BVNW"},
  {.keyword_eng = "ADVERBS", .keyword_nl = "BIJWOORDEN"},
  {.keyword_eng = "AGREE", .keyword_nl = "KLAAR"},
  {.keyword_eng = "ALL_LOCS", .keyword_nl = "ALLES"},
  {.keyword_eng = "AND", .keyword_nl = "EN"},
  {.keyword_eng = "ARTICLE_A", .keyword_nl = "EEN"},
  {.keyword_eng = "ARTICLE_THE", .keyword_nl = "DE"},
  {.keyword_eng = "ARTICLES", .keyword_nl = "LIDWOORDEN"},
  {.keyword_eng = "ATTRIBUTES", .keyword_nl = "ATTRIBUTEN"},
  {.keyword_eng = "BACKGROUND", .keyword_nl = "ACHTERGRONDKLEUR"},
  {.keyword_eng = "BLOCK_EXIT", .keyword_nl = "VERWIJDERUITGANG"},
  {.keyword_eng = "BOLD", .keyword_nl = "VET"},
  {.keyword_eng = "CANSEE", .keyword_nl = "KANZIEN"},
  {.keyword_eng = "CLEARFLAG", .keyword_nl = "MAAKONWAAR"},
  {.keyword_eng = "CLEARJSON", .keyword_nl = "CLEARJSON"},     /* @@@ */
  {.keyword_eng = "CLEARSCREEN", .keyword_nl = "WISSCHERM"},
  {.keyword_eng = "CLEARSTATUS", .keyword_nl = "WISSTATUSSCHERM"},
  {.keyword_eng = "COMMON_ATTRS", .keyword_nl = "ALG_ATTRIBUTEN"},
  {.keyword_eng = "COMMON_DESCRS", .keyword_nl = "ALG_BESCHRIJVINGEN"},
  {.keyword_eng = "COMMON_FLAGS", .keyword_nl = "ALG_VLAGGEN"},
  {.keyword_eng = "COMMON_TRIGGERS", .keyword_nl = "ALG_TRIGGERS"},
  {.keyword_eng = "CONJUNCTION", .keyword_nl = "VOEGWOORDEN"},
  {.keyword_eng = "CONTAINED", .keyword_nl = "GEPLAATST"},
  {.keyword_eng = "CONTENTS", .keyword_nl = "INHOUD"},
  {.keyword_eng = "COUNT", .keyword_nl = "TEL"},
  {.keyword_eng = "CURR_LOC_ONLY", .keyword_nl = "HUIDIGE_PLAATS"},
  {.keyword_eng = "DEBUG", .keyword_nl = "DEBUG"},
  {.keyword_eng = "DEFAULT", .keyword_nl = "DEFAULT"},
  {.keyword_eng = "DESCRIPTIONS", .keyword_nl = "BESCHRIJVINGEN"},
  {.keyword_eng = "DEST", .keyword_nl = "BESTEMMING"},
  {.keyword_eng = "DIR", .keyword_nl = "RICHTING"},
  {.keyword_eng = "DIRECTION", .keyword_nl = "TELT"},
  {.keyword_eng = "DIRECTIONS", .keyword_nl = "RICHTINGEN"},
  {.keyword_eng = "DISAGREE", .keyword_nl = "STOP"},
  {.keyword_eng = "DISAMBIG_RULES", .keyword_nl = "ZOEKREGELS"},
  {.keyword_eng = "DISTANCE", .keyword_nl = "AFSTAND"},
  {.keyword_eng = "DOWN", .keyword_nl = "AF"},
  {.keyword_eng = "ELSE", .keyword_nl = "ANDERS"},
  {.keyword_eng = "END_LOC", .keyword_nl = "EINDE_PLAATS"},
  {.keyword_eng = "END_OBJ", .keyword_nl = "EINDE_DING"},
  {.keyword_eng = "END_RULES", .keyword_nl = "EINDE_REGELS"},
  {.keyword_eng = "ENDIF", .keyword_nl = "EINDE_ALS"},
  {.keyword_eng = "ENDVERB", .keyword_nl = "EINDE_ACTIE"},
  {.keyword_eng = "ENG", .keyword_nl = "ENGELS"},
  {.keyword_eng = "ENTRANCE", .keyword_nl = "NAARBINNEN"},
  {.keyword_eng = "EPILOGUE", .keyword_nl = "ACHTERAF"},
  {.keyword_eng = "EQUAL", .keyword_nl = "HETZELFDE"},
  {.keyword_eng = "EXACT", .keyword_nl = "EXACT"},
  {.keyword_eng = "EXECUTE", .keyword_nl = "UITVOEREN"},
  {.keyword_eng = "EXIT", .keyword_nl = "VERLAAT"},
  {.keyword_eng = "EXITS", .keyword_nl = "UITGANGEN"},
  {.keyword_eng = "FIRSTDIR", .keyword_nl = "EERSTE_RICHTING"},
  {.keyword_eng = "FLAGS", .keyword_nl = "VLAGGEN"},
  {.keyword_eng = "FUNCTION", .keyword_nl = "FUNCTIE"},
  {.keyword_eng = "GET_SPECIFIER", .keyword_nl = "VRAAGDING2"},
  {.keyword_eng = "GET_SUBJECT", .keyword_nl = "VRAAGDING1"},
  {.keyword_eng = "GO", .keyword_nl = "AAN"},
  {.keyword_eng = "GO_TO", .keyword_nl = "GANAAR"},
  {.keyword_eng = "GREATER_THAN", .keyword_nl = "GROTER"},
  {.keyword_eng = "HITANYKEY", .keyword_nl = "TOETS"},
  {.keyword_eng = "IF", .keyword_nl = "ALS"},
  {.keyword_eng = "INDENT", .keyword_nl = "INSPRINGEN"},
  {.keyword_eng = "INIT", .keyword_nl = "STARTWAARDE"},
  {.keyword_eng = "INSERT", .keyword_nl = "INVOEGEN"},
  {.keyword_eng = "INTERVAL", .keyword_nl = "INTERVAL"},
  {.keyword_eng = "ISLIT", .keyword_nl = "VERLICHT"},
  {.keyword_eng = "ITALIC", .keyword_nl = "SCHUIN"},
  {.keyword_eng = "LESS_THAN", .keyword_nl = "KLEINER"},
  {.keyword_eng = "LOCATION", .keyword_nl = "PLAATS"},
  {.keyword_eng = "MOVE", .keyword_nl = "VERPLAATS"},
  {.keyword_eng = "NEW_EXIT", .keyword_nl = "MAAKUITGANG"},
  {.keyword_eng = "NL", .keyword_nl = "NEDERLANDS"},
  {.keyword_eng = "NOMATCH", .keyword_nl = "OEPS"},
  {.keyword_eng = "NONE", .keyword_nl = "NIETS"},
  {.keyword_eng = "NOT", .keyword_nl = "NIET"},
  {.keyword_eng = "NOTIMERS", .keyword_nl = "GEENTIMERS"},
  {.keyword_eng = "NOUNS", .keyword_nl = "ZNW"},
  {.keyword_eng = "NUMBER", .keyword_nl = "NUMMER"},
  {.keyword_eng = "OBJECT", .keyword_nl = "DING"},
  {.keyword_eng = "OR", .keyword_nl = "OF"},
  {.keyword_eng = "OR_LESS", .keyword_nl = "OF_LAGER"},
  {.keyword_eng = "OR_MORE", .keyword_nl = "OF_HOGER"},
  {.keyword_eng = "ORDINAL", .keyword_nl = "VOLGNUMMER"},
  {.keyword_eng = "OWNER", .keyword_nl = "EIGENAAR"},
  {.keyword_eng = "OWNS", .keyword_nl = "HEEFT"},
  {.keyword_eng = "PREPOS", .keyword_nl = "VOORZETSEL"},
  {.keyword_eng = "PREPOSITION", .keyword_nl = "voorzetsel"},
  {.keyword_eng = "PREPOSITIONS", .keyword_nl = "VOORZETSELS"},
  {.keyword_eng = "PRIMARY_COLOR", .keyword_nl = "HOOFDKLEUR"},
  {.keyword_eng = "PRINT", .keyword_nl = "PRINT"},
  {.keyword_eng = "PRINTBOLD", .keyword_nl = "PRINTVET"},
  {.keyword_eng = "PRINTCR", .keyword_nl = "PRINTCR"},
  {.keyword_eng = "PRINTCRBOLD", .keyword_nl = "PRINTCRVET"},
  {.keyword_eng = "PRINTCRITALIC", .keyword_nl = "PRINTCRSCHUIN"},
  {.keyword_eng = "PRINTCRSTATUS", .keyword_nl = "PRINTCRSTATUS"},
  {.keyword_eng = "PRINTITALIC", .keyword_nl = "PRINTSCHUIN"},
  {.keyword_eng = "PRINTSTATUS", .keyword_nl = "PRINTSTATUS"},
  {.keyword_eng = "PROLOGUE", .keyword_nl = "VOORAF"},
  {.keyword_eng = "Q_WORDS", .keyword_nl = "VRAAGWOORDEN"},
  {.keyword_eng = "QUIT", .keyword_nl = "QUIT"},
  {.keyword_eng = "QUOT", .keyword_nl = "QUOT"},
  {.keyword_eng = "RAND", .keyword_nl = "RAND"},
  {.keyword_eng = "REDEFINE_ATTRS", .keyword_nl = "VERVANG_ATTRIBUTEN"},
  {.keyword_eng = "REDEFINE_FLAGS", .keyword_nl = "VERVANG_VLAGGEN"},
  {.keyword_eng = "REDEFINE_TRIGGERS", .keyword_nl = "VERVANG_TRIGGERS"},
  {.keyword_eng = "REDEFINE_VERB", .keyword_nl = "VERVANG_ACTIE"},
  {.keyword_eng = "REM", .keyword_nl = "REM"},
  {.keyword_eng = "RESTORE", .keyword_nl = "LADEN"},
  {.keyword_eng = "RUNCOMMON", .keyword_nl = "DOE_ALGEMEEN"},
  {.keyword_eng = "RUNVERB", .keyword_nl = "DOE_WERKWOORD"},
  {.keyword_eng = "SAVE", .keyword_nl = "OPSLAAN"},
  {.keyword_eng = "SCOPE", .keyword_nl = "SCOPE"},
  {.keyword_eng = "SCORE", .keyword_nl = "SCORE"},
  {.keyword_eng = "SENDJSON", .keyword_nl = "SENDJSON"},      /* @@@ */
  {.keyword_eng = "SETCURSOR", .keyword_nl = "PLAATSCURSOR"},
  {.keyword_eng = "SETFLAG", .keyword_nl = "MAAKWAAR"},
  {.keyword_eng = "SETTIMER", .keyword_nl = "ZETTIMER"},
  {.keyword_eng = "SHUFFLE", .keyword_nl = "HUSSEL"},
  {.keyword_eng = "STARTTIMER", .keyword_nl = "STARTTIMER"},
  {.keyword_eng = "STATE", .keyword_nl = "STATUS"},
  {.keyword_eng = "STEP", .keyword_nl = "STAPGROOTTE"},
  {.keyword_eng = "STOP", .keyword_nl = "UIT"},
  {.keyword_eng = "STOPTIMER", .keyword_nl = "STOPTIMER"},
  {.keyword_eng = "STORY_LANGUAGE", .keyword_nl = "STORY_TAAL"},
  {.keyword_eng = "STRING", .keyword_nl = "TEKST"},
  {.keyword_eng = "SYNCHRONIZE", .keyword_nl = "SAMEN"},
  {.keyword_eng = "SYNONYM", .keyword_nl = "SYNONIEM"},
  {.keyword_eng = "TESTFLAG", .keyword_nl = "ISWAAR"},
  {.keyword_eng = "TESTMODE", .keyword_nl = "TESTMODE"},
  {.keyword_eng = "TEXT", .keyword_nl = "TEKSTKLEUR"},
  {.keyword_eng = "THEN", .keyword_nl = "DAN"},
  {.keyword_eng = "THIS", .keyword_nl = "DIT"},
  {.keyword_eng = "TIMERS", .keyword_nl = "TIMERS"},
  {.keyword_eng = "TITLE", .keyword_nl = "TITEL"},
  {.keyword_eng = "TRANSCRIPT", .keyword_nl = "VERSLAG"},
  {.keyword_eng = "TRIGGER", .keyword_nl = "DOE"},
  {.keyword_eng = "TRIGGER_AT", .keyword_nl = "GRENSWAARDE"},
  {.keyword_eng = "TRIGGERS", .keyword_nl = "TRIGGERS"},
  {.keyword_eng = "TRY", .keyword_nl = "PROBEER"},
  {.keyword_eng = "UNDERLINE", .keyword_nl = "ONDERSTREEPT"},
  {.keyword_eng = "UP", .keyword_nl = "OP"},
  {.keyword_eng = "VALID_DIRECTION", .keyword_nl = "UITGANG"},
  {.keyword_eng = "VALUE", .keyword_nl = "WAARDE"},
  {.keyword_eng = "VERB", .keyword_nl = "ACTIE"},
  {.keyword_eng = "VERSION", .keyword_nl = "VERSIE"},
  {.keyword_eng = "WAIT", .keyword_nl = "WACHT"},
  {.keyword_eng = "XVAN_LANGUAGE", .keyword_nl = "XVAN_TAAL"},
  {.keyword_eng = "YES_NO", .keyword_nl = "JA_NEE"},
};

#endif