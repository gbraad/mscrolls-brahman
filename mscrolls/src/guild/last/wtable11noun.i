***************************************************************************
*
*
*	fred23jr output from file '../guild'*
*
***************************************************************************

	IFNE	YCHEAT

IFYCHEAT	MACRO	* DC.B 'dsvdafsg',$23+$80
		DC.B	\2,\3
		ENDM

	ENDC


	IFEQ	YCHEAT

IFYCHEAT	MACRO	* ditto

		ENDM

	ENDC


	

	DC.B	'l_fo',$72+$80
	DC.B	'search_fo',$72+$80
	DC.B	'go_t',$6F+$80
	DC.B	'sin',$67+$80
	DC.B	'switc',$68+$80
	DC.B	'borro',$77+$80
	DC.B	'deflec',$74+$80
	DC.B	'stak',$65+$80
	DC.B	'inspec',$74+$80
	DC.B	'assis',$74+$80
	DC.B	'',$7A+$80
	DC.B	'smea',$72+$80
	DC.B	'pe',$65+$80
	DC.B	'tos',$73+$80
	DC.B	'bea',$74+$80
	DC.B	'dra',$67+$80
	DC.B	'stru',$6D+$80
	DC.B	'div',$65+$80
	DC.B	'gol',$64+$80
	DC.B	'gra',$79+$80
	DC.B	'ti',$70+$80
	DC.B	'unzi',$70+$80
	DC.B	'unscre',$77+$80
	DC.B	'toward',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#boa',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#moorin',$67+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#jett',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#scrub',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#scrub',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#scrub',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#forest',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#forest',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#forest',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#forest',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#fiel',$64+$80
 ENDC
	DC.B	'fiel',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#hgroun',$64+$80
 ENDC
	DC.B	'mountai',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#drawbridg',$65+$80
 ENDC
	DC.B	'bridg',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#mill',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gatehous',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bedroom',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#quarter',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#kitche',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#hall',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#hall',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#yar',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pat',$68+$80
 ENDC
	DC.B	'garde',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#stabl',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#loung',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#galler',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#room',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#passage',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sway',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cella',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#room',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#corridor',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#corridor',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#corridor',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#corridor',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#librar',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bathroo',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bedroom',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bedroom',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#la',$62+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#room',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#room',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#acha',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gdn',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#templ',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stair',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stair',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stair',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stair',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sway',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sway',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sway',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sway',$34+$80
 ENDC
	DC.B	'alcov',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#organroo',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cryp',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#shrin',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#library',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#well',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#well',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#well',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tunne',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sum',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ban',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#shaft',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#crab',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cave',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#fac',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cave',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cave',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ent',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#passage',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#room',$39+$80
 ENDC
	DC.B	'junctio',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#psg',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#psg',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tjunc',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tjunc',$32+$80
 ENDC
	DC.B	'junctio',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#tjunc',$33+$80
 ENDC
	DC.B	'junctio',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#avr',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gdn',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#hs',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#offi',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#psg',$37+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cag',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#fr',$6B+$80
 ENDC
	DC.B	'for',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#shop',$31+$80
 ENDC
	DC.B	'sho',$70+$80
 IFNE YCHEAT&VAX
	DC.B	'#shop',$32+$80
 ENDC
	DC.B	'sho',$70+$80
 IFNE YCHEAT&VAX
	DC.B	'#shop',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cemt',$72+$80
 ENDC
	DC.B	'graveyar',$64+$80
	DC.B	'cemetar',$79+$80
 IFNE YCHEAT&VAX
	DC.B	'#rr',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rv',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tpwf',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rmu',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#fnr',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stp',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bhs',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#drm',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#drm',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#drm',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#drm',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#drm',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#thie',$66+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#mille',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gatekeepe',$72+$80
 ENDC
	DC.B	'keepe',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#bea',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gues',$74+$80
 ENDC
	DC.B	'gues',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#spide',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gravedigge',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#snak',$65+$80
 ENDC
	DC.B	'Lawrenc',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#boat',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#jetty',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rive',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wate',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wheatfiel',$64+$80
 ENDC
	DC.B	'fiel',$64+$80
	DC.B	'field_of_whea',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#moa',$74+$80
 ENDC
	DC.B	'strea',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#castl',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cops',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#woo',$64+$80
 ENDC
	DC.B	'fores',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#hil',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tree',$73+$80
 ENDC
	DC.B	'woo',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#fishingsig',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#counte',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lak',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wall',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sk',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bag',$31+$80
 ENDC
	DC.B	'sac',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#sweatshir',$74+$80
 ENDC
	DC.B	'shir',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#jean',$73+$80
 ENDC
	DC.B	'trouser',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#lam',$70+$80
 ENDC
	DC.B	'torc',$68+$80
 IFNE YCHEAT&VAX
	DC.B	'#pocket',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#mill',$32+$80
 ENDC
	DC.B	'windmil',$6C+$80
 IFNE YCHEAT&VAX
	DC.B	'#vane',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#chair',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#chaf',$66+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lut',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#plectru',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gu',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#leve',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bed',$31+$80
 ENDC
	DC.B	'sheet',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#tu',$62+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#maggo',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cabinet',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#key',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sin',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cupboar',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#workto',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#poiso',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ja',$72+$80
 ENDC
	DC.B	'jar_of_ja',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#jam',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ja',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cage',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#table',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#chalic',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ru',$67+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cage',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#chequ',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rat',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rat',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rat',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rat',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bon',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stra',$77+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#flie',$73+$80
 ENDC
	DC.B	'fl',$79+$80
 IFNE YCHEAT&VAX
	DC.B	'#deadflie',$73+$80
 ENDC
	DC.B	'fl',$79+$80
 IFNE YCHEAT&VAX
	DC.B	'#carpe',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#fire',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#grat',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bucke',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#coa',$6C+$80
 ENDC
	DC.B	'piece_of_coa',$6C+$80
	DC.B	'coa',$6C+$80
 IFNE YCHEAT&VAX
	DC.B	'#fossi',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#painting',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#painting',$32+$80
 ENDC
	DC.B	'paintin',$67+$80
 IFNE YCHEAT&VAX
	DC.B	'#painting',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#saf',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#couc',$68+$80
 ENDC
	DC.B	'couc',$68+$80
 IFNE YCHEAT&VAX
	DC.B	'#cushio',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#foa',$6D+$80
 ENDC
	DC.B	'stuffin',$67+$80
 IFNE YCHEAT&VAX
	DC.B	'#not',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#door',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rat',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#step',$73+$80
 ENDC
	DC.B	'stairwa',$79+$80
 IFNE YCHEAT&VAX
	DC.B	'#rack',$73+$80
 ENDC
	DC.B	'rac',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#ratwate',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#syste',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pip',$65+$80
 ENDC
	DC.B	'pipewor',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#stopcoc',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wine',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#champagn',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bottle',$31+$80
 ENDC
	DC.B	'bottle_of_re',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#bottle',$34+$80
 ENDC
	DC.B	'bottle_of_champagn',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#rub',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#jun',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#book',$31+$80
 ENDC
	DC.B	'boo',$6B+$80
	DC.B	'tome',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#desk',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#book',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#devic',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pape',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#toile',$74+$80
 ENDC
	DC.B	'w',$63+$80
	DC.B	'lo',$6F+$80
	DC.B	'lavator',$79+$80
	DC.B	'li',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#bed',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#po',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cabinet',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#brooc',$68+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#blackboar',$64+$80
 ENDC
	DC.B	'boar',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#bed',$33+$80
 ENDC
	DC.B	'sheet',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#wardrob',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#dres',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#box',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#needl',$65+$80
 ENDC
	DC.B	'pi',$6E+$80
	DC.B	'hoo',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#cotto',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cauldro',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bookshel',$66+$80
 ENDC
	DC.B	'shel',$66+$80
	DC.B	'shelve',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#book',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sache',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#powde',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cu',$65+$80
 ENDC
	DC.B	'ti',$70+$80
 IFNE YCHEAT&VAX
	DC.B	'#table',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ball',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ball',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ball',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rack',$31+$80
 ENDC
	DC.B	'rack',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#rin',$67+$80
 ENDC
	DC.B	'diamon',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#pian',$6F+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#stoo',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#shee',$74+$80
 ENDC
	DC.B	'shee',$74+$80
	DC.B	'musi',$63+$80
 IFNE YCHEAT&VAX
	DC.B	'#bag',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#boar',$64+$80
 ENDC
	DC.B	'notic',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#alta',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#statu',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#burne',$72+$80
 ENDC
	DC.B	'li',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#hiv',$65+$80
 ENDC
	DC.B	'hiv',$65+$80
	DC.B	'bee_hiv',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#bee',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#glove',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#hone',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#door',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#gri',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bluesquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#orangesquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#yellowsquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#violetsquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#redsquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#greensquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#indigosquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#blacksquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#whitesquare',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#blacksquar',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#whitesquar',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#redsquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#redsquare',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#orangesquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#orangesquare',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#yellowsquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#yellowsquare',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#greensquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bluesquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bluesquare',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#indigosquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#indigosquare',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#indigosquare',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#violetsquare',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#violetsquare',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#redsquare',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#orangesqure',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#greensquare',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#yellowsquare',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#squaredeat',$68+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sarcophagu',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#skul',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ey',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#statuett',$65+$80
 ENDC
	DC.B	'eye',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#fakemille',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#door',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pamphle',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#key',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ivoryke',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#san',$64+$80
 ENDC
	DC.B	'lump',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#boot',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#rop',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#minera',$6C+$80
 ENDC
	DC.B	'vei',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#pic',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bar',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#signpos',$74+$80
 ENDC
	DC.B	'pos',$74+$80
	DC.B	'sig',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#gat',$65+$80
 ENDC
	DC.B	'gat',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#slot',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#maca',$77+$80
 ENDC
	DC.B	'bir',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#bird',$33+$80
 ENDC
	DC.B	'myna',$68+$80
 IFNE YCHEAT&VAX
	DC.B	'#cage',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tree',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#succulent',$73+$80
 ENDC
	DC.B	'bed_of_succulent',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#coconu',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cage',$36+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cage',$37+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#door',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#desk',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#drawer',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#spad',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#food',$73+$80
 ENDC
	DC.B	'grai',$6E+$80
	DC.B	'zo',$6F+$80
 IFNE YCHEAT&VAX
	DC.B	'#magazine',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#door',$36+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#door',$37+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#windo',$77+$80
 ENDC
	DC.B	'glas',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#caske',$74+$80
 ENDC
	DC.B	'coffi',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#til',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#butto',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#piec',$65+$80
 ENDC
	DC.B	'piec',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#grave',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#grave',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#grave',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#headstone',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#headstone',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#headstone',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#tree',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#berrie',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#skeleto',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#finge',$72+$80
 ENDC
	DC.B	'finge',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#ches',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#hear',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#spik',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ladde',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bea',$6D+$80
 ENDC
	DC.B	'beam_of_ligh',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#poo',$6C+$80
 ENDC
	DC.B	'mu',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#wa',$78+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ge',$6D+$80
 ENDC
	DC.B	'gemston',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#mirro',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#coal',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cube',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cube',$31+$80
 ENDC
	DC.B	'bloc',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#podium',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#podium',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#podium',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#podium',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#podium',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#die',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#die',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#die',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#die',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#die',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#cas',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#slot',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#slot',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#slot',$34+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#slot',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#privdoo',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#shel',$66+$80
 ENDC
	DC.B	'shel',$66+$80
 IFNE YCHEAT&VAX
	DC.B	'#rhin',$6F+$80
 ENDC
	DC.B	'rhin',$6F+$80
 IFNE YCHEAT&VAX
	DC.B	'#blackbook',$73+$80
 ENDC
	DC.B	'boo',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#ladder',$32+$80
 ENDC
	DC.B	'bal',$6C+$80
 IFNE YCHEAT&VAX
	DC.B	'#insect',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#reptile',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#flor',$61+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#outsid',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wrai',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wclou',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wwin',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sui',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#insid',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wratrac',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wetplace',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#wdri',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#orga',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#billiard',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#igat',$65+$80
 ENDC
	DC.B	'doo',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#cke',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#fla',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#shard',$73+$80
 ENDC
	DC.B	'windo',$77+$80
	DC.B	'glas',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#mill',$33+$80
 ENDC
	DC.B	'windmil',$6C+$80
 IFNE YCHEAT&VAX
	DC.B	'#lfis',$68+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#dfis',$68+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pfis',$68+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#plaque',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#leg',$73+$80
 ENDC
	DC.B	'le',$67+$80
 IFNE YCHEAT&VAX
	DC.B	'#fee',$74+$80
 ENDC
	DC.B	'foo',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#arm',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#playe',$72+$80
 ENDC
	DC.B	'm',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#fis',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#han',$64+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#plaque',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#upbutto',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#downbutto',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#abstrac',$74+$80
 ENDC
	DC.B	'abstrac',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#bed2u',$70+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#hol',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#guil',$64+$80
 ENDC
	DC.B	'thieve',$73+$80
	DC.B	'guil',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#spel',$6C+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#chip',$73+$80
 ENDC
	DC.B	'ston',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#trun',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#sas',$68+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#banque',$74+$80
 ENDC
	DC.B	'part',$79+$80
	DC.B	'celebratio',$6E+$80
 IFNE YCHEAT&VAX
	DC.B	'#rac',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#staircas',$65+$80
 ENDC
	DC.B	'stair',$73+$80
	DC.B	'step',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#popenin',$67+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pentranc',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#ppassag',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pai',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pdus',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pcorne',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pfac',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pwhea',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#baro',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#spade',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#zoopaper',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#board',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lunchsig',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lchair',$73+$80
 ENDC
	DC.B	'chai',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#lfloo',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lrock',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lceilin',$67+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#lsurfac',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#oneroo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#tworoo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#threeroo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#fourroo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#wmachin',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#pilla',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#signpost',$33+$80
 ENDC
	DC.B	'pos',$74+$80
	DC.B	'arrow',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#fiveroo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#sixroo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#dumm',$79+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEAT&VAX
	DC.B	'#cardbo',$78+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#card',$35+$80
 ENDC
	DC.B	'car',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#card',$34+$80
 ENDC
	DC.B	'car',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#card',$33+$80
 ENDC
	DC.B	'car',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#card',$32+$80
 ENDC
	DC.B	'car',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#card',$31+$80
 ENDC
	DC.B	'car',$64+$80
 IFNE YCHEAT&VAX
	DC.B	'#hoora',$79+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#dbox',$31+$80
 ENDC
	DC.B	'nightsaf',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#dbox',$32+$80
 ENDC
	DC.B	'nightsaf',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#dbox',$33+$80
 ENDC
	DC.B	'nightsaf',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#dbox',$34+$80
 ENDC
	DC.B	'nightsaf',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#horsesho',$65+$80
 ENDC
	DC.B	'sho',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#stabledoo',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#kban',$6B+$80
 ENDC
	DC.B	'ban',$6B+$80
	DC.B	'buildin',$67+$80
 IFNE YCHEAT&VAX
	DC.B	'#bsig',$6E+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#oakdoo',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bnotic',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#inban',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#antecub',$65+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#queue',$31+$80
 ENDC
	DC.B	'lin',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#queue',$32+$80
 ENDC
	DC.B	'lin',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#mandoo',$72+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#counter',$32+$80
 ENDC
	DC.B	'barrie',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#manoffi',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#vaultdoo',$72+$80
 ENDC
	DC.B	'vaul',$74+$80
 IFNE YCHEAT&VAX
	DC.B	'#desk',$33+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#paper',$73+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#chair',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#dcabine',$74+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#till',$31+$80
 ENDC
	DC.B	'til',$6C+$80
 IFNE YCHEAT&VAX
	DC.B	'#wall',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#till',$32+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#staf',$66+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#telle',$72+$80
 ENDC
	DC.B	'cler',$6B+$80
 IFNE YCHEAT&VAX
	DC.B	'#dress',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#finban',$6B+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#key',$35+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#bglas',$73+$80
 ENDC
	DC.B	'glas',$73+$80
 IFNE YCHEAT&VAX
	DC.B	'#peopl',$65+$80
 ENDC
	DC.B	'peopl',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#manage',$72+$80
 ENDC
	DC.B	'peckha',$6D+$80
	DC.B	'capitalis',$74+$80
	DC.B	'manage',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#manthin',$67+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#whit',$65+$80
 ENDC
	DC.B	'colou',$72+$80
 IFNE YCHEAT&VAX
	DC.B	'#rubba',$67+$80
 ENDC
	DC.B	'rubbis',$68+$80
	DC.B	'ba',$67+$80
 IFNE YCHEAT&VAX
	DC.B	'#keyhol',$65+$80
 ENDC
	DC.B	'hol',$65+$80
 IFNE YCHEAT&VAX
	DC.B	'#strea',$6D+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#temple',$31+$80
 ENDC
 IFNE YCHEAT&VAX
	DC.B	'#for',$6D+$80
 ENDC
* produced on Mon Jun 11 21:57:50 2018

* checksum = 91cb134a
