***************************************************************************
*
*
*	fred23jr output from file '../jinxter'*
*
***************************************************************************

	IFNE	YCHEATMORE

IFYCHEAT	MACRO	* DC.B 'dsvdafsg',$23+$80
		DC.B	\2,\3
		ENDM

	ENDC


	IFEQ	YCHEATMORE

IFYCHEAT	MACRO	* ditto

		ENDM

	ENDC


	

	DC.B	'sen',$64+$80
	DC.B	'mai',$6C+$80
	DC.B	'boar',$64+$80
	DC.B	'disembar',$6B+$80
	DC.B	'lubricat',$65+$80
	DC.B	'hea',$74+$80
	DC.B	'',$7A+$80
	DC.B	'bus',$6B+$80
	DC.B	'snuf',$66+$80
	DC.B	'connec',$74+$80
	DC.B	'disconnec',$74+$80
	DC.B	'ban',$67+$80
	DC.B	'fol',$64+$80
	DC.B	'unfol',$64+$80
	DC.B	'fur',$6C+$80
	DC.B	'unfur',$6C+$80
	DC.B	'urinat',$65+$80
	DC.B	'pe',$65+$80
	DC.B	'phon',$65+$80
	DC.B	'inspec',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#onbu',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bus_sig',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#butto',$6E+$80
 ENDC
	DC.B	'bel',$6C+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#autodoor',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#crow',$64+$80
 ENDC
	DC.B	'crow',$64+$80
	DC.B	'peopl',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bus_drive',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#frontgarden',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#law',$6E+$80
 ENDC
	DC.B	'law',$6E+$80
	DC.B	'pastur',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#flowerur',$6E+$80
 ENDC
	DC.B	'miniatur',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#rose',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#gardengat',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#deckchai',$72+$80
 ENDC
	DC.B	'chai',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bag',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#jean',$73+$80
 ENDC
	DC.B	'duck',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#sweatshir',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fis',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#han',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#playe',$72+$80
 ENDC
	DC.B	'm',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#floo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#kitchen',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#tabl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#tin_opene',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fridg',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#ramblingshe',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#sandwic',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#heart',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#cmakers_sig',$6E+$80
 ENDC
	DC.B	'sig',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bedroom',$31+$80
 ENDC
	DC.B	'simpl',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#be',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#cmakers_windo',$77+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wardrob',$65+$80
 ENDC
	DC.B	'cupboar',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#as',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#keyrin',$67+$80
 ENDC
	DC.B	'rin',$67+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#key',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#key',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#key',$33+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bathroom',$31+$80
 ENDC
	DC.B	'ancien',$74+$80
	DC.B	'warpe',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bathtu',$62+$80
 ENDC
	DC.B	'bat',$68+$80
	DC.B	'tu',$62+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#basi',$6E+$80
 ENDC
	DC.B	'basi',$6E+$80
	DC.B	'sin',$6B+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#lavator',$79+$80
 ENDC
	DC.B	'lo',$6F+$80
	DC.B	'bo',$67+$80
	DC.B	'toile',$74+$80
	DC.B	'W',$43+$80
	DC.B	'joh',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#drago',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#book',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#library',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#green_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bookshelve',$73+$80
 ENDC
	DC.B	'bookshelve',$73+$80
	DC.B	'antiqu',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#des',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#countrysid',$65+$80
 ENDC
	DC.B	'sid',$65+$80
	DC.B	'landscap',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#telephon',$65+$80
 ENDC
	DC.B	'phon',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#magazine',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#conservator',$79+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wickerchair',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wickertabl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#milkbottl',$65+$80
 ENDC
	DC.B	'bottl',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#tableclot',$68+$80
 ENDC
	DC.B	'clot',$68+$80
	DC.B	'ra',$67+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#pseudocmaker',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#backgarden',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#oil_lam',$70+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#shear',$73+$80
 ENDC
	DC.B	'pair_of_secateur',$73+$80
	DC.B	'cutter',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#glove',$73+$80
 ENDC
	DC.B	'pair_of_glove',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#countrylane',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#mailbo',$78+$80
 ENDC
	DC.B	'postbo',$78+$80
	DC.B	'gizm',$6F+$80
	DC.B	'bo',$78+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#postal_not',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#dead_mous',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#countrylane',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#ladde',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#charm',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#charm',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#charm',$33+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#stoo',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pubheart',$68+$80
 ENDC
	DC.B	'fireplac',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#charm',$34+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#charm',$35+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#hallway',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#vie',$77+$80
 ENDC
	DC.B	'cliff',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#field',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bul',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bull',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#curtain',$32+$80
 ENDC
	DC.B	'ra',$67+$80
	DC.B	'clot',$68+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#wood',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#tree',$73+$80
 ENDC
	DC.B	'oak',$73+$80
	DC.B	'conifer',$73+$80
	DC.B	'tre',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#flie',$73+$80
 ENDC
	DC.B	'carpetin',$67+$80
	DC.B	'maggot',$73+$80
	DC.B	'egg',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#field',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fenc',$65+$80
 ENDC
	DC.B	'wir',$65+$80
	DC.B	'barbed-wir',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#orchar',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#int_freez',$65+$80
 ENDC
	DC.B	'oojim',$79+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#clockmaker',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#stream',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bridge',$31+$80
 ENDC
	DC.B	'structur',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bridg',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#frontgarden',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pat',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#kitchen_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#parce',$6C+$80
 ENDC
	DC.B	'packag',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#kitchen',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#soc',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#ja',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#oi',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#crystalbal',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#diningroom',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#diningtabl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#rung',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#candl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#paintin',$67+$80
 ENDC
	DC.B	'portrai',$74+$80
	DC.B	'fathe',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#study',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#benche',$73+$80
 ENDC
	DC.B	'benc',$68+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#encyclopaedia',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wooden_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#study_tabl',$65+$80
 ENDC
	DC.B	'des',$6B+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#ic',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#keyhole',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#trapdoor',$31+$80
 ENDC
	DC.B	'doo',$72+$80
	DC.B	'trap-doo',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#cellar',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#chees',$65+$80
 ENDC
	DC.B	'lum',$70+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#wooden_bo',$78+$80
 ENDC
	DC.B	'matchbo',$78+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#matc',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#paddl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#backgarden',$32+$80
 ENDC
	DC.B	'prickl',$79+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#holly_bus',$68+$80
 ENDC
	DC.B	'hollybus',$68+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#naturestuf',$66+$80
 ENDC
	DC.B	'substance',$73+$80
	DC.B	'spice',$73+$80
	DC.B	'herb',$73+$80
	DC.B	'holl',$79+$80
	DC.B	'flowerbe',$64+$80
	DC.B	'berrie',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#hollybus',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#magpi',$65+$80
 ENDC
	DC.B	'bir',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#spectacle',$73+$80
 ENDC
	DC.B	'spec',$73+$80
	DC.B	'glasse',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#boathous',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fishingboa',$74+$80
 ENDC
	DC.B	'skeleto',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#witch_han',$64+$80
 ENDC
	DC.B	'wris',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#slidingdoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#runner',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#canoe_moorin',$67+$80
 ENDC
	DC.B	'jett',$79+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#cano',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#lake',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#clockmake',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pagod',$61+$80
 ENDC
	DC.B	'isle',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#steps',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pubfir',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#dirtmoun',$64+$80
 ENDC
	DC.B	'moun',$64+$80
	DC.B	'dir',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#corridor',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#offic',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#red_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bottom_wel',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pubcrow',$64+$80
 ENDC
	DC.B	'peopl',$65+$80
	DC.B	'crow',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#plastic_des',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pouc',$68+$80
 ENDC
	DC.B	'cas',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#Aquitanian_fer',$67+$80
 ENDC
	DC.B	'fer',$67+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#icewate',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#swivel_chai',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#divers_roo',$6D+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#panel',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#flush_butto',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#flood_butto',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#diving_sui',$74+$80
 ENDC
	DC.B	'wetsui',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#flipper',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#mas',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#aqualun',$67+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#airloc',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#underwate',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#library',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#water',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wood',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fairgroun',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#carouse',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#jukebo',$78+$80
 ENDC
	DC.B	'orga',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#slo',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#unicor',$6E+$80
 ENDC
	DC.B	'hors',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#Kerulgian_drago',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pelica',$6E+$80
 ENDC
	DC.B	'bir',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#state_carriag',$65+$80
 ENDC
	DC.B	'carriag',$65+$80
	DC.B	'walru',$73+$80
	DC.B	'state_carriag',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#fire_engin',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#baker',$79+$80
 ENDC
	DC.B	'hug',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bake',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bakery_ove',$6E+$80
 ENDC
	DC.B	'roo',$6D+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#ove',$6E+$80
 ENDC
	DC.B	'doo',$72+$80
	DC.B	'Gawd-help-u',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#oven_pane',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#oven_butto',$6E+$80
 ENDC
	DC.B	'control',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#trestl',$65+$80
 ENDC
	DC.B	'trestl',$65+$80
	DC.B	'work-surfac',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#canoworm',$73+$80
 ENDC
	DC.B	'ca',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#ingredient',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#puddin',$67+$80
 ENDC
	DC.B	'loa',$66+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#cake_ti',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#niceclou',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bakerystep',$73+$80
 ENDC
	DC.B	'stair',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#cloc',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#girde',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#girder_roo',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#plastictu',$62+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#on_platfor',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bea',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#gon',$67+$80
 ENDC
	DC.B	'mirro',$72+$80
	DC.B	'glob',$65+$80
	DC.B	'su',$6E+$80
	DC.B	'blo',$62+$80
	DC.B	'yellow_thin',$67+$80
	DC.B	'bel',$6C+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#model',$73+$80
 ENDC
	DC.B	'model',$73+$80
	DC.B	'figurin',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#rain_weatherma',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#umbrell',$61+$80
 ENDC
	DC.B	'broll',$79+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#sun_weatherma',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#end_of_bea',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#distant_rainma',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#distant_sunma',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pu',$62+$80
 ENDC
	DC.B	'fro',$67+$80
	DC.B	'gible',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#ba',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bookshelves',$32+$80
 ENDC
	DC.B	'bookshelve',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#publica',$6E+$80
 ENDC
	DC.B	'landlor',$64+$80
	DC.B	'barma',$6E+$80
	DC.B	'bartende',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#solutio',$6E+$80
 ENDC
	DC.B	'liqui',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#drin',$6B+$80
 ENDC
	DC.B	'drin',$6B+$80
	DC.B	'glasse',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bee',$72+$80
 ENDC
	DC.B	'bee',$72+$80
	DC.B	'moosebolte',$72+$80
	DC.B	'bolte',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#saddl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#puddl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#handpum',$70+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#library',$34+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#beergarde',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bakerynotic',$65+$80
 ENDC
	DC.B	'sig',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#flour_sac',$6B+$80
 ENDC
	DC.B	'sack_of_flou',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#outside_statio',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#train_statio',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#benc',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#ticket_offic',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#locomotiv',$65+$80
 ENDC
	DC.B	'locomotiv',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#train_ticke',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#platform',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#tapdance_shoe',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#top_mountai',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#castl',$65+$80
 ENDC
	DC.B	'turret',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#sac',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#lake',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#walled_garde',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#weed',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#precipic',$65+$80
 ENDC
	DC.B	'chas',$6D+$80
	DC.B	'abys',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#dungeo',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#manacle',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#witch_chambe',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#windows',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#witch_door',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bed',$32+$80
 ENDC
	DC.B	'Couch_of_Dream',$73+$80
	DC.B	'couc',$68+$80
	DC.B	'be',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#trestle_tabl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#witch_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#cmakers_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#outsid',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wrai',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wclou',$64+$80
 ENDC
	DC.B	'stormcloud',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#wlightnin',$67+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wwind',$30+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#chimne',$79+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#castle_parapet',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#glass_fragment',$73+$80
 ENDC
	DC.B	'shard',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#garden_statue',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#archwa',$79+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#quadrangl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#banquet_hal',$6C+$80
 ENDC
	DC.B	'chambe',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#banquet_table',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#chair',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fireplace',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#painting',$31+$80
 ENDC
	DC.B	'portrai',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#door',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#tower_entranc',$65+$80
 ENDC
	DC.B	'chambe',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#spiral_staircas',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#hallwa',$79+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#antechambe',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bracele',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#balcon',$79+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#infinite',$31+$80
 ENDC
	DC.B	'lan',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#infinite',$32+$80
 ENDC
	DC.B	'lan',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#nuke_devic',$65+$80
 ENDC
	DC.B	'bom',$62+$80
	DC.B	'nuk',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#va',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#your_hous',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#receive',$72+$80
 ENDC
	DC.B	'handse',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#front_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#documen',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#guardia',$6E+$80
 ENDC
	DC.B	'mal',$65+$80
	DC.B	'Le',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#ticke',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#inspecto',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bsto',$70+$80
 ENDC
	DC.B	'sto',$70+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bu',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#xams_hous',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#mailbox',$31+$80
 ENDC
	DC.B	'postbo',$78+$80
	DC.B	'bo',$78+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#telephone',$31+$80
 ENDC
	DC.B	'phon',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#receiver',$31+$80
 ENDC
	DC.B	'handse',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#blo',$62+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#key',$34+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#smok',$65+$80
 ENDC
	DC.B	'atmospher',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#latticed_window',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wal',$6C+$80
 ENDC
	DC.B	'ston',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#hea',$64+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#connecting_passag',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#airlock_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#whee',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#inf_underwate',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#over_hatc',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#onlak',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#hatc',$68+$80
 ENDC
	DC.B	'cove',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#lette',$72+$80
 ENDC
	DC.B	'handwritin',$67+$80
	DC.B	'writin',$67+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#topof_library',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#postoffic',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#podoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#counte',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#scale',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bel',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#postmistres',$73+$80
 ENDC
	DC.B	'mistres',$73+$80
	DC.B	'staf',$66+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#saf',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#plu',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pubdoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#flou',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#sea',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bakery_larde',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#jo',$62+$80
 ENDC
	DC.B	'apprenticeshi',$70+$80
	DC.B	'vacanc',$79+$80
	DC.B	'positio',$6E+$80
	DC.B	'applicatio',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#pudding',$32+$80
 ENDC
	DC.B	'loa',$66+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bathroomdoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#kitchendoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#leatherdoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#xa',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#match',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#match',$33+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#match',$34+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#notic',$65+$80
 ENDC
	DC.B	'sig',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#barsig',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#int_flyz',$7A+$80
 ENDC
	DC.B	'doofe',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#int_aqu',$61+$80
 ENDC
	DC.B	'dooda',$68+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#int_so',$6C+$80
 ENDC
	DC.B	'thing',$79+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#int_dracmazz',$61+$80
 ENDC
	DC.B	'watchercalli',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#chandelie',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#top_chandelie',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#underwater',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#over_wrec',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#shipwrec',$6B+$80
 ENDC
	DC.B	'shi',$70+$80
	DC.B	'hul',$6C+$80
	DC.B	'wrec',$6B+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#top_ha',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#fallin',$67+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#glass_dom',$65+$80
 ENDC
	DC.B	'bas',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#bloc',$6B+$80
 ENDC
	DC.B	'squar',$65+$80
	DC.B	'block',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#platform',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#platform',$33+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#railway_trac',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#post_depo',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#dungeon_rop',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#telegraph_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#station_maste',$72+$80
 ENDC
	DC.B	'maste',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#harmonic',$61+$80
 ENDC
	DC.B	'silve',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#silver_coi',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#danc',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wate',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#on_trai',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#tlhandl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#trhandl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#blhandl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#brhandl',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#twofer',$67+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#carriage_window',$31+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#carriage_window',$32+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#theatr',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#optabl',$65+$80
 ENDC
	DC.B	'surfac',$65+$80
	DC.B	'be',$64+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#strap',$73+$80
 ENDC
	DC.B	'restraint',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#machin',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#nurs',$65+$80
 ENDC
	DC.B	'femal',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#surgicalmas',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#garmen',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#guards_va',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#hanging_on_trai',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#train_roo',$66+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#platform_bridg',$65+$80
 ENDC
	DC.B	'step',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#other_platfor',$6D+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#train_passenger',$73+$80
 ENDC
	DC.B	'tourist',$73+$80
	DC.B	'touris',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#guar',$64+$80
 ENDC
	DC.B	'inspecto',$72+$80
	DC.B	'collecto',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#overcoa',$74+$80
 ENDC
	DC.B	'coa',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#scalpe',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#swa',$62+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#waswossnam',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#roo',$66+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#busk_sig',$6E+$80
 ENDC
	DC.B	'sig',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#voice',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#wooden_shutte',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#unicorn',$32+$80
 ENDC
	DC.B	'hors',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#siev',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#parachut',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#worm',$73+$80
 ENDC
	DC.B	'boastfu',$6C+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#towe',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#passag',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#gao',$6C+$80
 ENDC
	DC.B	'jai',$6C+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#gaole',$72+$80
 ENDC
	DC.B	'jaile',$72+$80
	DC.B	'stou',$74+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#solid_doo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#inner_hatc',$68+$80
 ENDC
	DC.B	'shutte',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#outer_hatc',$68+$80
 ENDC
	DC.B	'shutte',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#castle_kitche',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#in_hatc',$68+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#jewel_roo',$6D+$80
 ENDC
	DC.B	'vaul',$74+$80
	DC.B	'roo',$6D+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#witc',$68+$80
 ENDC
	DC.B	'witc',$68+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#cris',$70+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#hol',$65+$80
 ENDC
	DC.B	'fissur',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#postma',$6E+$80
 ENDC
	DC.B	'leblin',$67+$80
	DC.B	'posti',$65+$80
	DC.B	'darklin',$6B+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#pseudocloc',$6B+$80
 ENDC
	DC.B	'towe',$72+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#busstopbit',$73+$80
 ENDC
	DC.B	'bas',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#debri',$73+$80
 ENDC
	DC.B	'ruin',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#spare_tan',$6B+$80
 ENDC
	DC.B	'tan',$6B+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#inove',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#ovendoo',$72+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#mouse_tra',$70+$80
 ENDC
	DC.B	'tra',$70+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#canoe_hol',$65+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#bun',$67+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#buskin',$67+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#conservatory_window',$73+$80
 ENDC
	DC.B	'garde',$6E+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#mil',$6B+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#staircase',$33+$80
 ENDC
	DC.B	'step',$73+$80
	DC.B	'stairwa',$79+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#kitchen_fa',$6E+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#caryatid',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#lecter',$6E+$80
 ENDC
	DC.B	'succubu',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#carving',$73+$80
 ENDC
	DC.B	'snake-bird',$73+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#cmaker_fir',$65+$80
 ENDC
	DC.B	'flam',$65+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#mixedoi',$6C+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#pseudop',$6F+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#random_objec',$74+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#waterfal',$6C+$80
 ENDC
	DC.B	'waterfal',$6C+$80
 IFNE YCHEATMORE&VAX
	DC.B	'#fakecmaker',$73+$80
 ENDC
 IFNE YCHEATMORE&VAX
	DC.B	'#dummy',$31+$80
 ENDC
