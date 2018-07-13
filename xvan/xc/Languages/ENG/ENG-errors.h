
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
  "\n(000) Error opening file:",
  "\n(001) Error in malloc() for:",
  "\n(002) Compilation aborted...",
  "\nlines compiled:",
  "\n(004) Unknown commandline option:",
  "\n(005) Inputfile and outputfile must have different names",
  "\n(006) filename (with quotes) expected after insert keyword",
  "\n(007) Loop detected in inserted files, file:",
  "\n(008) File not found in file list: ",
  "\n(009) SwitchInputFile(): error in malloc() for file:",
  "\n(010) unexpected location parameter in string:",
  "\n(011) unexpected object parameter in string:",
  "\n(012) unexpected timer parameter in string:",
  "\n(013) unexpected description parameter in string:",
  "\n(014) unexpected attribute parameter in string:",
  "\n(015) unexpected [this] parameter in string.",
  "\n(016) unexpected '.' in string parameter",
  "\n(017) unexpected wildcard parameter in string:",
  "\n(018) missing ']' symbol in parameter",
  "\n(019) unexpected ']' symbol in parameter",
  "\n(020) parameter expected after '[' symbol. For a '[', type '[['.",
  "\n(021) Unexpected end of file in string",
  "\n(022) StoreOffsets(): error in fseek",
  "\n(023) StoreOffsets(): error writing offset for:",
  "\n(024) StoreId(): error writing id",
  "\n(025) StoreString(): error writing string length",
  "\n(026) StoreString(): error writing string:",
  "\n(027) StoreExtendedSysDescr(): error writing preposition.",
  "\n(028) StoreSysDescr(): write error",
  "\n(029) StoreContData(): write error",
  "\n(030) StoreAdverbInfo(): write error",
  "\n(031) StorePreposInfo(): write error",
  "\n(032) StoreActionRec(): write error",
  "\n(033) StoreStoryInfo(): write error",
  "\n(034) StoreWordTable(): write error",
  "\n(035) Warning: no timers defined",
  "\n(036) StoreTimerInfo(): write error",
  "\n(037) StoreParList(): write error",
  "\n(038) StoreExits(): write error",
  "\n(039) StoreFlags(): write error",
  "\n(040) StoreAttributes(): write error.",
  "\n(041) StoreVerbDir(): write error",
  "\n(042) StoreLocDir(): write error",
  "\n(043) StoreObjDir(): write error",
  "\n(044) StoreTriggOwners(): write error",
  "\n(045) StoreDescrOwners(): write error",
  "\n(046) SetBitVal(): error in value, must be 1 or 0:",
  "\n(047) Unexpected End Of File:",
  "\n(048) Unexpected keyword:",
  "\n(049) Unexpected string:",
  "\n(050) Unexpected scope:",
  "\n(051) Unexpected word (assignment error?):",
  "\n(052) ParseVerb(): verb not found:",
  "\n(053) IF or END_RULES keyword was expected",
  "\n(054) unexpected attribute parameter:",
  "\n(055) illegal owner for local attribute:",
  "\n(056) unexpected description parameter:",
  "\n(057) illegal owner for local description:",
  "\n(058) unexpected timer parameter:",
  "\n(059) unexpected location identifier:",
  "\n(060) unexpected object identifier:",
  "\n(061) unexpected value:",
  "\n(062) unexpected function:",
  "\n(063) unexpected direction",
  "\n(064) the word you assigned as attribute value, is not in the vocabulary:",
  "\n(065) too many adjectives in system description",
  "\n(066) syntax error in system description:",
  "\n(067) <unknown word id>",
  "\n(068) max function length exceeded",
  "\n(069) unexpected end of parameter",
  "\n(070) unexpected flag identifier:",
  "\n(071) illegal owner for local flag:",
  "\n(072) unexpected trigger identifier:",
  "\n(073) illegal owner for local trigger:",
  "\n(074) unexpected function call in par list",
  "\n(075) unexpected exit:",
  "\n(076) preposition for location is missing:",
  "\n(077) preposition expected, but found unknown word:",
  "\n(078) preposition expected",
  "I don't know the word",
  "\n(080) CheckSyntax(): unknown keyword type",
  "\n(081) syntax error in string\n"
   "Use [l_xx] [o_xx] [dir] etc to refer to XVAN concepts\n"
   "Nouns, adjectives etc don't work here",
  "\n(082) unexpected trigger owner:",
  "\n(083) max condition length exceeded",
  "\n(084) ParseCTriggs(): Error writing keyword:",
  "\n(085) flag value must be either 0 or 1",
  "\n(086) error in parentheses level",
  "\n(087) runverb() and runcommon() functions may not be used in verb code",
  "\n(088) runcommon() function may not be used in common trigger code",
  "\n(089) error creating WordTable",
  " undefined",
  "\n(091) String too long for:",   /* @@@ */
  "\n(092) Reserved",               /* @@@ */
  "\n(093) Reserved",               /* @@@ */
  "Processed",
  "\n(095) Too many verbs defined. Max number of verbs is:",
  "\n(096) Too many words defined. Max number of words is:",
  "\n(097) 'location' is a reserved location name",
  "\n(098) Multiple defined location:",
  "\n(099) Too many locations defined. Max number of locations is:",
  "\n(100) You may not use this as an object name (reserved for system):",
  "\n(101) Multiple defined object:",
  "\n(102) Too many objects defined. Max number of objects is:",
  "\n(103) Multiple defined flag:",
  "\n(104) Too many common flags defined. Max number of common flags is:",
  "\n(105) Too many local flags defined. Max number of local flags is:",
  "\n(106) Owner missing for trigger:",
  "\n(107) Multiple defined trigger:",
  "\n(108) Too many common triggers defined. Max number of common triggers is:",
  "\n(109) Too many local triggers defined. Max number of local triggers is:",
  "\n(110) Multiple defined timer:",
  "\n(111) Too many timers defined. Max number of timers is:",
  "\n(112) Owner missing for description:",
  "\n(113) Multiple defined description:",
  "\n(114) Too many common descriptions defined. Max number of common descriptions is:",
  "\n(115) Too many local descriptions defined. Max number of local descriptions is:",
  "\n(116) Owner missing for attribute:",
  "\n(117) Multiple defined attribute:",
  "\n(118) Too many common attributes defined. Max number of common attributes is:",
  "\n(119) Too many local attributes defined. Max number of local attributes is:",
  "\n(120) Too many types for word:",
  "\n(121) Multiple defined word:",
  "\n(122) Fatal error in ProcLocDSys(). Id not in loc_table:",
  "\n(123) Max number of system descriptions exceeded for location:",
  "\n(124) Fatal error in ProcObjDSys(). Id not in obj_table:",
  "\n(125) Max number of system descriptions exceeded for object:",
  "\n(126) Too many contained objects. Max number is:",
  "\n(127) Internal error, may only be called with loc or obj id: function",
  "\n(128) Multiple defined exit:",
  " has an undefined flag:",
  " has an undefined attribute:",
  " has an undefined trigger:",
  "\n(132) Undefined timer:",
  " has an undefined description:",
  "\n(134) PrintlocationNameForId(): error, id not found:",
  "\n(135) PrintLocationNameForId(): error, input is not a location id:",
  "\n(136) PrintObjectNameForId(): error, id not found:",
  "\n(137) PrintObjectNameForId(): error, input is not an object id:",
  "\n(138) Multiple redefinition of verb:",
  "\n(139) Unexpected value in switch statement in function",
  "\n(140) Multiple redefinition of trigger:",
  "\n(141) Unexpected comma",
  "\n(142) Missing end quote",
  "\n(143) Unknown escape sequence \\",
  "\n(144) Double definition of:",  /* @@@ */
  "\n(145) Owner missing for flag:",
  "\n(146) Multiple redefinition of flag:",
  "\n(147) Multiple redefinition of attribute:"
};

#endif
