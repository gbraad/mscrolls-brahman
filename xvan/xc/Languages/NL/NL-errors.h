
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

#if !defined(__NL_errors)
#define __NL_errors


const char *NL_errors[] = {
  "\n(000) Fout bij het openen van bestand:",
  "\n(001) Fout bij geheugentoewijzing voor:",
  "\n(002) Compilatie afgebroken...",
  "\nRegels verwerkt:",
  "\n(004) Onbekende optie:",
  "\n(005) Het invoer- en uitvoerbestand moeten verschillende namen hebben",
  "\n(006) na $insert moet een bestandsnaam (tussen dubbele quotes) volgen",
  "\n(007) Er is een lus in de ingevoegde files, file:",
  "\n(008) Bestand niet gevonden in de bestandslijst:",
  "\n(009) SwitchInputFile(): fout bij geheugentoewijzing voor bestand:",
  "\n(010) onverwachte locatieparameter in tekst:",
  "\n(011) onverwachte objectparameter in tekst:",
  "\n(012) onverwachte timerparameter in tekst:",
  "\n(013) onverwachte beschrijving parameter in tekst:",
  "\n(014) onverwachte attribuut parameter in tekst:",
  "\n(015) onverwachte [this] parameter in tekst:",
  "\n(016) onverwachte '.' in tekst parameter",
  "\n(017) onverwachte wildcard parameter in tekst:",
  "\n(018) ontbrekend ']' symbool in parameter",
  "\n(019) onverwacht ']' symbool in parameter",
  "\n(020) na het '[' symbol moet een parameter komen. Voor een '[', type '[['",
  "\n(021) Onverwacht einde van het bestand in tekst (waarschijnlijk eerder een end quote vergeten)",
  "\n(022) StoreOffsets(): fout bij fseek()",
  "\n(023) StoreOffsets(): fout bij wegschrijven van de offset voor:",
  "\n(024) StoreId(): fout bij wegschrijven van i",
  "\n(025) StoreString(): fout bij wegschrijven van tekstlengte",
  "\n(026) StoreString(): fout bij wegschrijven van tekst:",
  "\n(027) StoreExtendedSysDescr(): fout bij wegschrijven van voorzetsel",
  "\n(028) StoreSysDescr(): fout bij wegschrijven",
  "\n(029) StoreContData(): fout bij wegschrijven",
  "\n(030) StoreAdverbInfo(): fout bij wegschrijven",
  "\n(031) StorePreposInfo(): fout bij wegschrijven",
  "\n(032) StoreActionRec(): fout bij wegschrijven",
  "\n(033) StoreStoryInfo(): fout bij wegschrijven",
  "\n(034) StoreWordTable(): fout bij wegschrijven",
  "\n(035) waarschuwing: er zijn geen timers gedefinieerd",
  "\n(036) StoreTimerInfo(): fout bij wegschrijven",
  "\n(037) StoreParList(): fout bij wegschrijven",
  "\n(038) StoreExits(): fout bij wegschrijven",
  "\n(039) StoreFlags(): fout bij wegschrijven",
  "\n(040) StoreAttributes(): write error",
  "\n(041) StoreVerbDir(): fout bij wegschrijven",
  "\n(042) StoreLocDir(): fout bij wegschrijven",
  "\n(043) StoreObjDir(): fout bij wegschrijven",
  "\n(044) StoreTriggOwners(): fout bij wegschrijven",
  "\n(045) StoreDescrOwners(): fout bij wegschrijven",
  "\n(046) SetBitVal(): fout in waarde. De waarde moet 0 of 1 zijn:",
  "\n(047) Onverwacht einde van het bestand:",
  "\n(048) Onverwacht sleutelwoord:",
  "\n(049) Onverwachte tekststring:",
  "\n(050) Onverwachte scope:",
  "\n(051) Onverwacht woord:",
  "\n(052) ParseVerb(): werkwoord niet gevonden:",
  "\n(053) Verwacht was een IF of END_RULES sleutelwoord",
  "\n(054) Onverwachte attribuutparameter:",
  "\n(055) Niet toegestane eigenaar voor lokaal attribuut:",
  "\n(056) Onverwachte beschrijvingparameter:",
  "\n(057) Niet toegestane eigenaar voor lokale beschrijving:",
  "\n(058) Onverwachte timerparameter:",
  "\n(059) Onverwachte locatieparameter:",
  "\n(060) Onverwachte object parameter:",
  "\n(061) Onverwachte waarde:",
  "\n(062) Onverwachte functie:",
  "\n(063) Onverwachte richting",
  "\n(064) Het woord dat is gebruikt als attribuutwaarde is geen onderdeel van de vocabulaire:",
  "\n(065) te veel bijvoeglijke naamwoorden in de systeembeschrijving",
  "\n(066) syntax error in de systeembeschrijving:",
  "\n(067) <onbekende woordidentifier>",
  "\n(068) De maximaal toegestane lengte voor de functie is overschreden",
  "\n(069) Onverwacht einde van de parameter",
  "\n(070) Onverwachte vlagparameter:",
  "\n(071) Niet toegestane eigenaar voor lokale vlag:",
  "\n(072) onverwachte triggerparameter:",
  "\n(073) Niet toegestane eigenaar voor lokale trigger:",
  "\n(074) Onverwachte functie-aanroep in parameterlijst",
  "\n(075) Onverwachte uitgang:",
  "\n(076) Het voorzetsel voor de locatie ontbreekt:",
  "\n(077) Onverwacht woord waar een voorzetsel werd verwacht:",
  "\n(078) Voorzetsel verwacht",
  "Ik ken dit woord niet:",
  "\n(080) CheckSyntax(): onbekend type sleutelwoord",
  "\n(081) syntax error in tekst.\n"
   "Gebruik [l_xx] [o_xx] [dir] etc om naar XVAN artefacten te verwijzen.\n"
   "Zelfstandige en bijvoeglijke naamwoorden etc werken hier niet",
  "\n(082) Onverwachte trigger eigenaar:",
  "\n(083) De maximale lente van de IF-voorwaarde is overschreden",
  "\n(084) ParseCTriggs(): fout bij wegschrijven van keyword:",
  "\n(085) De waarde van een vlag moet 0 of 1 zijn",
  "\n(086) Het aantal linker- en rechterhaakjes klopt niet met elkaar",
  "\n(087) De functies runverb() enruncommon() zijn niet toegestaan in werkwoord code",
  "\n(088) De runcommon() functie is niet toegestaan in common trigger code",
  "\n(089) Fout bij het maken van de woordentabel",
  "ongedefinieerd als",
  "\n(091) De tekst is te lang voor:",   /* @@@ */
  "\n(092) Gereserveerd",                /* @@@ */
  "\n(093) Gereserveerd",                /* @@@ */
  "Verwerkt",
  "\n(095) Er zijn teveel werkwoorden gedefinieerd. Het maximum aantal is:",
  "\n(096) Er zijn teveel woorden gedefinieerd. Het maximum aantal is:",
  "\n(097) Dit is een voor het systeem gereserveerde naam:",
  "\n(098) Deze plaats wordt meerdere malen gedefinieerd:",
  "\n(099) Er zijn te veel plaatsen gedefinieerd. Het maximum aantal is:",
  "\n(100) Deze naam voor een ding is voor het systeem gereserveerd:",
  "\n(101) Dit ding wordt meerdere malen gedefinieerd:",
  "\n(102) Er zijn te veel dingen gedefinieerd. Het maximum aantal is:",
  "\n(103) Deze vlag wordt meerdere malen gedefinieerd:",
  "\n(104) Er zijn te veel algemene vlaggen gedefinieerd. Het maximum aantal is:",
  "\n(105) Er zijn te veel locale vlaggen gedefinieerd. Het maximum aantal is:",
  "\n(106) Bij deze trigger ontbreekt de eigenaar:",
  "\n(107) Deze trigger wordt meerdere malen gedefinieerd:",
  "\n(108) Er zijn te veel algemene triggers gedefinieerd. Het maximum aantal is:",
  "\n(109) Er zijn te veel loccale triggers gedefinieerd. Het maximum aantal is:",
  "\n(110) Deze timer wordt meerdere malen gedefinieerd:",
  "\n(111) Er zijn te veel timers gedefinieerd. Het maximum aantal is:",
  "\n(112) Bij deze beschrijving ontbreekt de eigenaar:",
  "\n(113) Deze beschrijving wordt meerdere malen gedefinieerd:",
  "\n(114) Er zijn te veel algemene beschrijvingen gedefinieerd. Het maximum aantal is:",
  "\n(115) Er zijn te veel locale beschrijvingen gedefinieerd. Het maximum aantal is:",
  "\n(116) Bij dit attribuur ontbreekt de eigenaar:",
  "\n(117) Dit attribuut wordt meerdere malen gedefinieerd:",
  "\n(118) Er zijn te veel algemene attributen gedefinieerd. Het maximum aantal is:",
  "\n(119) Er zijn te veel locale attributen gedefinieerd. Het maximum aantal is:",
  "\n(120) Dit woord heeft te veel types:",
  "\n(121) Dit woord wordt meerdere keren gedefinieerd:",
  "\n(122) Fatale fout in ProcLocDSys(). Dit id staat niet in loc_table:",
  "\n(123) Deze plaats heeft te veel systeembeschrijvingen:",
  "\n(124) Fatale fout in ProcObjDSys(). Dit id staat niet in lobj_table:",
  "\n(125) Dit ding heeft te veel systeembeschrijvingen:",
  "\n(126) Het maximum aantal dingen in een ander ding is:",
  "\n(127) Interne fout, deze functie mag alleen aangeroepen worden met een plaats of ding:",
  "\n(128) Deze uitgang wordt meerdere keren gedefinieerd:",
  " heeft een niet-gedefinieerde vlag:",
  " heeft een niet-gedefinieerd attribuut:",
  " heeft een niet-gedefinieerde trigger:",
  "\n(132) Timer niet gedefinieerd:",
  " heeft een niet-gedefinieerde beschrijving:",
  "\n(134) PrintlocationNameForId(): foutmelding, id niet gevonden:",
  "\n(135) PrintLocationNameForId(): foutmelding, invoer is geen plaats id:",
  "\n(136) PrintObjectNameForId(): foutmelding, id niet gevonden:",
  "\n(137) PrintObjectNameForId(): foutmelding, invoer is geen ding id:",
  "\n(138) Meer dan 1 vervangende definitie van actie:",
  "\n(139) Onverwachte waarde in switch statement in functie",
  "\n(140) Meer dan 1 vervangende definitie van trigger:",
  "\n(141) Onverwachte komma",
  "\n(142) de quote aan het einde van de tekst ontbreekt",
  "\n(143) Onbekende escape code \\",
  "\n(144) Dubbele definitie van:",  /* @@@ */
  "\n(145) Bij deze vlag ontbreekt de eigenaar:",
  "\n(146) Meer dan 1 vervangende definitie van flag:"
  "\n(146) Meer dan 1 vervangende definitie van attribuut:"
};

#endif
