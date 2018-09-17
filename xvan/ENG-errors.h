
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


#if !defined(__ENG_errors)
#define __ENG_errors

const char *ENG_errors[] = {
  "runtime error (000): XeqLtGt(): unknown operation:",
  "runtime error (001): XeqDiv() fatal error: attribute or timer expected",
  "runtime error (002): XeqBasicOperator(): division by zero",
  "runtime error (003): XeqBasicOperator(): illegal operator:",
  "runtime error (004): Print(): the parameter cannot be printed",
  "runtime error (005): SetAttribute(): parameter 2 must be of type "
  "location, object, description, attribute, timer, number, word, "
  "direction or preposition",
  "runtime error (006): CheckPars(): undefined function code:",
  "runtime error (007): GetDescr(): wrong id type",
  "runtime error (008): GetDescr(): local description not found",
  "runtime error (009): GetTrigger(): local trigger not found",
  "runtime error (010): bad opcode in function",
  "runtime error (011): XeqActionRec(): Location or object identifier expected",
  "runtime error (012): error loading verb in function:",
  "runtime error (013): the impossible just happened in",
  "runtime error (014): Read error in function",
  "Initialization error (015): memory allocation error in function:",
  "initialization error (016): error in fseek() for function",
  "initialization error (017): ReadDirOffsets() error reading",
  "initialization error (018): ReadDirOffsets(): error restoring position in file",
  "initialization error (019): ReadTimers(): error reading",
  "initialization error (020): error reading offset for",
  "initialization error (021): wrong offset for",
  "Initialization error (022): ReadMapData(): error reading",
  "initialization error (023): ReadFlags(): error reading",
  "initialization error (024): ReadStoryInfo(): error reading",
  "initialization error (025): ReadWordTable(): error reading",
  "initialization error (026): ReadVerbDir(): error reading",
  "initialization error (027): ReadLocDir(): error reading",
  "initialization error (028): ReadLocDir(): error reading",
  "initialization error (029): RdCTriggs(): error reading",
  "initialization error (030): InitAttributes(): error reading",
  "initialization error (031): RdTrigOwners(): error reading",
  "initialization error (032): RdDescrOwners(): error reading",
  "initialization error (033): ReadExtendedSysDescr(): error reading",
  "initialization error (034): ReadSysDescr(): error reading",
  "initialization error (035): ReadContData(): error reading",
  "initialization error (036): load(): error reading",
  "\n\nThis interpreter version 2.3.4 needs compiler version 2.3.4 stories.",
  "\nThis story was compiled with XVAN compiler version",
  "Hit a key...",
  "initialization error (040): error opening file:",
  "initialization error (041): unknown command option:",
  "runtime error (042): FindParserRules(): error loading parser records.",
  "*** end of testfile ***",
  "runtime error (044): error writing to transcript file.",
  "There are too many items that match your description.\n"
  "Try using some adjectives to narrow it down...",
  "I don't understand that sentence",
  "runtime error (047): error writing to debug file",
  "runtime error (048): PrintString(): error retrieving attribute info.",
  "runtime error (049): PrintString(): Attribute par in string may not contain another attribute.\n",
  "runtime error (050): unknown word id:",
  "runtime error (051): CheckedStore(): array bound overflow",
  "runtime error (052): ReadFun(): error reading string_par",
  "initialization error (053): unexpected code in function",
  "runtime error (054): ReadDescr(): error reading",
  "runtime error (055): ProcCode(): triggercode too long",
  "runtime error (056): Write error in function",
  "runtime error (057): BuildRoute(): no starting point.",
  "I don't know the word",
  "Too many adjectives, but will give it a try",
  "You have entered a double adjective",
  "Syntax ANSWER not yet implemented",
  "runtime error (062): unknown parameter syntax in function",
  "runtime error (063): GetLvaluePar(): used function does not return location or object id",
  "runtime error (064): GetLvaluePar():attribute or timer expected",
  "runtime error (065): GetAttributeInfo(): error, not an attribute id:",
  "runtime error (066): BuildCList(): there might be a loop in the containment tree (list size exceeded)",
  "runtime error (067): SetBitVal(): error in value, must be 1 or 0",
  "runtime error (068): Move(): no more room in destination",
  "runtime error (069): Move(): contained object not found",
  "runtime error (070): Shuffle(): id must be location or object",
  "runtime error (071): Synchronize(): unexpected return value from XeqTrigger():",
  "runtime error (072): Push(): stack overflow",
  "runtime error (073): Pop(): stack underflow",
  "runtime error (074): HandleTimers(): bad trigger_spec",
  "runtime error (075): SearchHits(): bad ParseDSys() return value",
  "runtime error (076): Find(): bad scope parameter",
  "runtime error (077): Translate(): unknown return value from Find()",
  "runtime error (078): NONE-parameter in function",
  "runtime error (079): unexpected return value in function:",
  "runtime error (080): XeqRunCommon(): there is no common trigger",
  "runtime error (081): XeqTestFun(): undefined testfunction opcode:",
  "runtime error (082): Xeqbackground(): only blue and black are supported background colors",
  "runtime error (083): unknown parameter (must be on or off) in function:",
  "XeqGoto(): no route found from source to destination",
  "runtime error (085): XeqMove(): something cannot be moved into itself",
  "runtime error (086): XeqIndent(): indent value too large:",
  "runtime error (087): XeqText(): only blue and black are supported text colors",
  "runtime error (088): Error, savefile is for story:",
  "runtime error (089): Error, savefile is for version:",
  "Testmode is already active",
  "Switching to testmode...",
  "Transcript mode started",
  "Turning off transcript mode",
  "runtime error (094): XeqIntAct(): undefined internal action opcode:",
  "runtime error (095): GetActionRecPar(): error reading action record",
  "runtime error (096): parameter missing in try() function:",
  "runtime error (097): 'it' attribute must be location or object",
  "runtime error (098): Base64 coding error",
  "runtime error (099): strncpy failed in function:",
  "runtime error (100): Base64 decoding error",
  "runtime error (101): Error restoring game state",
  "runtime error (102): Error saving game progress"
};

#endif
