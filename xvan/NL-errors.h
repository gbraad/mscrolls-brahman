
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

#if !defined(__NL_errors)
#define __NL_errors

const char *NL_errors[] = {
  "runtime error (000): XeqLtGt(): onbekende operator:",
  "runtime error (001): XeqDiv() attribuut of timer verwacht",
  "runtime error (002): XeqBasicOperator(): delen door nul",
  "runtime error (003): XeqBasicOperator(): onverwachte operator:",
  "runtime error (004): Print(): de parameter kan niet afgedrukt worden",
  "runtime error (005): SetAttribute(): parameter 2 moet van het type "
  "plaats, ding, beschrijving, attribuut, timer, getal, woord, "
  "richting of voorzetsel zijn",
  "runtime error (006): CHackPars(): ongedefinieerde functiecode:",
  "runtime error (007):L GetDescr(): verkkerd identifier type",
  "runtime error (008): GetDescr(): lokale beschrijving kan niet gevonden worden",
  "runtime error (009): GetTrigger(): lokale trigger kan niet gevonden worden",
  "runtime error (010): verkeerde opcode in functie",
  "runtime error (011): XeqActionRec(): plaats of object identifier verwacht",
  "runtime error (012): fout bij het laden van een actie in functie:",
  "runtime error (013): het onmogelijke is net gebeurd in",
  "runtime error (014): Leesfout in functie",
  "Initialization error (015): fout bij geheugentoewijzing in functie:",
  "initialization error (016): fseek() fout in functie",
  "initialization error (017): ReadDirOffsets() fout bij het lezen van",
  "initialization error (018): ReadDirOffsets(): fout bij het tergzetten van de filepositie",
  "initialization error (019): ReadTimers(): fout bij het lezen van",
  "initialization error (020): fout bij het lezen van de offset voor",
  "initialization error (021): verkeerde offset voor",
  "Initialization error (022): ReadMapData(): fout bij het lezen van",
  "initialization error (023): ReadFlags(): fout bij het lezen van",
  "initialization error (024): ReadStoryInfo(): fout bij het lezen van",
  "initialization error (025): ReadWordTable(): fout bij het lezen van",
  "initialization error (026): ReadVerbDir(): fout bij het lezen van",
  "initialization error (027): ReadLocDir(): fout bij het lezen van",
  "initialization error (028): ReadLocDir(): fout bij het lezen van",
  "initialization error (029): RdCTriggs(): fout bij het lezen van",
  "initialization error (030): InitAttributes(): fout bij het lezen van",
  "initialization error (031): RdTrigOwners(): fout bij het lezen van",
  "initialization error (032): RdDescrOwners(): fout bij het lezen van",
  "initialization error (033): ReadExtendedSysDescr(): fout bij het lezen van",
  "initialization error (034): ReadSysDescr(): fout bij het lezen van",
  "initialization error (035): ReadContData(): fout bij het lezen van",
  "initialization error (036): load(): fout bij het lezen van",
  "\n\nVersie 2.3.4 van de interpreter kan alleen bestanden van compiler versie 2.3.4 lezen.",
  "\nDit bestand is gecompileerd met versie",
  "Druk een willekeurige toets...",
  "initialization error (040): fout bij het openen van bestand:",
  "initialization error (041): onbekende optie:",
  "runtime error (042): FindParserRules(): fout bij het laden van de parser records.",
  "*** einde van de testfile ***",
  "runtime error (044): fout bij het shcrijven naar het transcriptbestand.",
  "Er zijn teveel dingen die voldoen aan je beschrijving.\n"
  "Gebruik extra bijvoeglijke naamwoorden indien mogelijk...",
  "Ik begrijp deze zin niet",
  "runtime error (047): fout bij het schrijven naar de debugfile",
  "runtime error (048): PrintString(): fout bij het ophalen van attribuutinformatie.",
  "runtime error (049): PrintString(): een attribuutparameter in een tekststring mag geen ander attribuut als waarde hebben.\n",
  "runtime error (050): onbekende woord identifier:",
  "runtime error (051): CheckedStore(): het maximale aantal elementen in de array is overschreden",
  "runtime error (052): ReadFun(): fout bij het lezen van string_par",
  "initialization error (053): onbekende code in functie",
  "runtime error (054): ReadDescr(): fout bij het lezen van",
  "runtime error (055): ProcCode(): de code van de trigger is te lang",
  "runtime error (056): schrijffout in functie",
  "runtime error (057): BuildRoute(): er is geen beginpunt",
  "Dit woord ken ik niet:",
  "Er zijn te veel bijvoeglijke naamwoorden, ik zal het proberen",
  "een bijvoegliijk naamwoord komt twee keer voor",
  "De ANSWER-syntax is nog niet geimplementeerd",
  "runtime error (062): onbekende parameter syntax in functie",
  "runtime error (063): GetLvaluePar(): de gebruikte functie geeft geen locaite- of objectid terug",
  "runtime error (064): GetLvaluePar(): er werd een attribuut of timer verwacht",
  "runtime error (065): GetAttributeInfo(): geen attribuutid:",
  "runtime error (066): BuildCList(): de toegestane grootte van de lijst is overschreden; er kan een loop in de containment tree zitten",
  "runtime error (067): SetBitVal(): de waarde moet 0 of 1 zijn",
  "runtime error (068): Move(): er is geen plaats voor nog een ding in de bestemming",
  "runtime error (069): Move(): het te verplaatsen ding is niet gevonden",
  "runtime error (070): Shuffle(): de parameter moet een locatie of een object zijn",
  "runtime error (071): Synchronize(): onbekende retourwaarde van XeqTrigger():",
  "runtime error (072): Push(): stack overflow",
  "runtime error (073): Pop(): stack underflow",
  "runtime error (074): HandleTimers(): verkeerde trigger_spec",
  "runtime error (075): SearchHits(): onbekende retourwaarde van ParseDSys()",
  "runtime error (076): Find(): verkeerde scope parameter",
  "runtime error (077): Translate(): onbekende retourwaarde van Find()",
  "runtime error (078): NONE-parameter in functie",
  "runtime error (079): onbekende retourwaarde in functie:",
  "runtime error (080): XeqRunCommon(): er is geen common triggercommon trigger",
  "runtime error (081): XeqTestFun(): ongedefinieerde opcode voor testfunctie:",
  "runtime error (082): Xeqbackground(): alleen zwart en blauw zijn toegestaan als achtergrondkleur",
  "runtime error (083): parameter moet waarde aan of uit hebben in functie:",
  "XeqGoto(): geen route gevonden van vertrekpunt naar bestemming",
  "runtime error (085): XeqMove(): een ding kan niet in zichzelf gestopt worden",
  "runtime error (086): XeqIndent(): de inspringwaarde is te groot:",
  "runtime error (087): XeqText(): alleen zwart en blauw zijn toegestaan als tekstkleur",
  "runtime error (088): de file met opgeslagen voortgang is voor XVAN-verhaal:",
  "runtime error (089): de file met opgeslagen voortgang is voor XVAN-versie:",
  "Testmode staat al aan",
  "Testmode wordt aangezet...",
  "Transcript mode aan",
  "Turning mode uit",
  "runtime error (094): XeqIntAct(): onbekende functiecode:",
  "runtime error (095): GetActionRecPar(): fout bij het lezen van actie record",
  "runtime error (096): ontbrekende parameter in try() functie:",
  "runtime error (097): 'het' attribuut moet een plaats of een ding zijn",
  "runtime error (098): Base64 coderingsfout",
  "runtime error (099): strncpy fout in functie:",
  "runtime error (100): Base64 decoderingsfout",
  "runtime error (101): Fout bij laden van opgeslagen game",
  "runtime error (102): Fout bij het opslaan van de voortgang"
};

#endif
