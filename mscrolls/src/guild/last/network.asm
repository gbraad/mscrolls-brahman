***************************************************************************
*
*
*	fred23jr output from file '../guild'*
*
***************************************************************************

		SECTION	"ADV",code

*--------------------------------
*
*  SRC.ADV.NETWORK
*
*--------------------------------

       XDEF       NETWORK,NET_END,NET_LEN


	include	"roomequ.i"
	include "nway.i"


 
*                  N   NE  E   SE  S   SW  W   NW  U   D

NETWORK


	DC.B	RNBOAT,RNBOAT,RNBOAT,RNBOAT,RNBOAT,RNBOAT,RNJETTY,RNBOAT,0,0,RNJETTY,RNJETTY	; boat
	DC.B	nway_lake,0,RNBOAT,0,nway_lake,RNFIELD,RNSCRUB2,RNSCRUB1,0,0,RNSCRUB2,RNBOAT	; jetty
	DC.B	nway_castlemoat,nway_castlemoat,nway_river,RNJETTY,RNSCRUB2,RNSCRUB3,RNSCRUB3,nway_castlemoat,0,0,RNSCRUB3,RNJETTY	; scrub1
	DC.B	RNSCRUB1,RNSCRUB1,RNJETTY,0,RNFIELD,RNFOREST1,RNSCRUB3,RNSCRUB3,0,0,RNSCRUB3,RNJETTY	; scrub2
	DC.B	RNGATEHOUSE,RNSCRUB1,RNSCRUB2,RNFIELD,RNFOREST1,RNFOREST2,RNHGROUND,RNHGROUND,0,0,RNGATEHOUSE,RNSCRUB2	; scrub3
	DC.B	RNSCRUB3,RNSCRUB2,RNFIELD,RNFIELD,RNFOREST3,RNFOREST4,RNFOREST2,RNHGROUND,nway_trees,0,RNFOREST4,RNSCRUB2	; forest1
	DC.B	RNHGROUND,RNSCRUB3,RNFOREST1,RNFOREST3,RNFOREST4,0,nway_riverbank,RNHGROUND,nway_trees,0,RNFOREST3,RNSCRUB3	; forest2
	DC.B	RNFOREST1,RNFIELD,RNFIELD,0,nway_river,0,RNFOREST4,RNFOREST2,nway_trees,0,RNFOREST4,RNFIELD	; forest3
	DC.B	RNFOREST2,RNFOREST1,RNFOREST3,0,RNACHAM,0,0,RNFOREST2,0,0,RNACHAM,RNFOREST1	; forest4
	DC.B	RNSCRUB2,RNJETTY,0,0,RNMILL1,RNFOREST3,RNFOREST1,RNSCRUB3,0,0,RNMILL1,RNJETTY	; field
	DC.B	0,0,RNSCRUB3,RNFOREST1,RNFOREST2,RNFOREST2,RNENTR,0,0,0,RNENTR,RNSCRUB3	; hground
	DC.B	RNFIELD,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNFIELD	; mill1
	DC.B	RNYARD,nway_wall,nway_wall,nway_wall,RNSCRUB3,nway_wall,nway_wall,nway_wall,0,0,RNYARD,RNSCRUB3	; gatehouse
	DC.B	RNQUARTERS,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNQUARTERS	; bedroom1
	DC.B	RNKITCHEN,nway_wall,nway_wall,nway_wall,RNBEDROOM1,nway_wall,nway_wall,nway_wall,0,0,RNKITCHEN,0	; quarters
	DC.B	RNHALL1,nway_wall,RNPASSAGE1,nway_wall,RNQUARTERS,nway_wall,nway_wall,nway_wall,0,0,RNHALL1,RNQUARTERS	; kitchen
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,RNKITCHEN,nway_wall,RNHALL2,nway_wall,0,0,RNHALL2,RNKITCHEN	; hall1
	DC.B	RNPATH,nway_wall,RNHALL1,nway_wall,RNYARD,nway_wall,RNLOUNGE,nway_wall,RNCORRIDOR1,0,RNLOUNGE,RNYARD	; hall2
	DC.B	RNHALL2,nway_wall,nway_wall,nway_wall,RNGATEHOUSE,nway_wall,nway_wall,nway_wall,0,0,RNHALL2,RNGATEHOUSE	; yard
	DC.B	0,0,RNSTABLE,0,RNHALL2,0,0,0,0,0,RNSTABLE,RNHALL2	; path
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNPATH,nway_wall,0,0,0,RNPATH	; stable
	DC.B	nway_wall,nway_wall,RNHALL2,nway_wall,RNGALLERY,nway_wall,nway_wall,nway_wall,0,0,RNGALLERY,RNHALL2	; lounge
	DC.B	RNLOUNGE,nway_wall,nway_wall,nway_wall,RNROOM1,nway_wall,nway_wall,nway_wall,0,0,RNROOM1,RNLOUNGE	; gallery
	DC.B	RNGALLERY,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNGALLERY	; room1
	DC.B	nway_door,nway_wall,nway_wall,nway_wall,RNROOM2,nway_wall,RNKITCHEN,nway_wall,0,0,RNROOM2,RNKITCHEN	; passage1
	DC.B	RNCELLAR,0,0,nway_wall,nway_door,nway_wall,0,0,0,RNCELLAR,RNCELLAR,0	; sway5
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,RNSWAY5,nway_wall,nway_wall,nway_wall,RNSWAY5,0,0,RNSWAY5	; cellar
	DC.B	RNPASSAGE1,nway_wall,nway_wall,nway_wall,nway_misc5,nway_wall,nway_wall,nway_wall,0,0,nway_misc5,RNPASSAGE1	; room2
	DC.B	RNBATHROOM,nway_wall,RNCORRIDOR2,nway_wall,RNCORRIDOR4,nway_wall,RNLIBRARY,nway_wall,0,RNHALL2,RNLIBRARY,RNHALL2	; corridor1
	DC.B	RNROOM3,nway_wall,RNCORRIDOR3,nway_wall,nway_wall,nway_wall,RNCORRIDOR1,nway_wall,0,0,RNROOM3,RNCORRIDOR1	; corridor2
	DC.B	RNROOM4,nway_wall,nway_wall,nway_wall,RNBEDROOM3,nway_wall,RNCORRIDOR2,nway_wall,0,0,RNROOM4,RNCORRIDOR2	; corridor3
	DC.B	RNCORRIDOR1,nway_wall,nway_wall,nway_wall,RNBEDROOM2,nway_wall,nway_wall,nway_wall,0,0,RNBEDROOM2,RNCORRIDOR1	; corridor4
	DC.B	nway_wall,nway_wall,RNCORRIDOR1,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNCORRIDOR1	; library
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,RNCORRIDOR1,nway_wall,nway_wall,nway_wall,0,0,0,RNCORRIDOR1	; bathroom
	DC.B	RNCORRIDOR4,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNCORRIDOR4	; bedroom2
	DC.B	RNCORRIDOR3,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNCORRIDOR3	; bedroom3
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNBEDROOM2,nway_wall,0,0,0,RNBEDROOM2	; lab
	DC.B	nway_wall,nway_wall,RNROOM4,nway_wall,RNCORRIDOR2,nway_wall,nway_wall,nway_wall,0,0,RNROOM4,RNCORRIDOR2	; room3
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,RNCORRIDOR3,nway_wall,RNROOM3,nway_wall,0,0,RNROOM3,RNCORRIDOR3	; room4
	DC.B	RNFOREST4,nway_wall,nway_wall,nway_wall,RNTEMPLE,nway_wall,nway_wall,nway_wall,0,0,RNTEMPLE,RNFOREST4	; acham
	DC.B	RNTEMPLE,nway_railings,nway_railings,nway_railings,nway_railings,nway_railings,nway_railings,nway_railings,0,0,RNTEMPLE,RNTEMPLE	; gdn1
	DC.B	RNACHAM,RNSTAIR2,nway_wall,RNSTAIR3,RNGDN1,RNSTAIR4,nway_wall,RNSTAIR1,0,0,0,RNACHAM	; temple
	DC.B	nway_wall,nway_wall,nway_wall,RNTEMPLE,nway_wall,nway_wall,nway_wall,nway_wall,0,RNSWAY1,RNSWAY1,RNTEMPLE	; stair1
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNTEMPLE,nway_wall,nway_wall,RNSWAY2,0,RNSWAY2,RNTEMPLE	; stair2
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNTEMPLE,RNSWAY3,0,RNSWAY3,RNTEMPLE	; stair3
	DC.B	nway_wall,RNTEMPLE,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNSWAY4,0,RNSWAY4,RNTEMPLE	; stair4
	DC.B	nway_wall,nway_wall,nway_wall,RNBLACKSQUARE,nway_wall,nway_wall,nway_wall,nway_wall,RNSTAIR1,0,RNBLACKSQUARE,RNSTAIR1	; sway1
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,RNSTAIR2,nway_wall,RNSTAIR2	; sway2
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_door,0,RNSTAIR3,nway_door,RNSTAIR3	; sway3
	DC.B	nway_wall,nway_wall,RNORGANROOM,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,RNSTAIR4,RNORGANROOM,RNSTAIR4	; sway4
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNSWAY4,nway_wall,0,0,0,RNSWAY4	; organroom
	DC.B	nway_wall,nway_wall,RNSHRINE,nway_wall,nway_wall,nway_wall,nway_wall,RNWHITESQUARE,0,0,RNSHRINE,RNWHITESQUARE	; crypt
	DC.B	nway_wall,nway_wall,nway_door,nway_wall,nway_wall,nway_wall,RNCRYPT,nway_wall,0,0,0,RNCRYPT	; shrine
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,0,0,0,0	; library2
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,RNWELL2,0,0	; well1
	DC.B	RNTUNNEL,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNWELL1,RNWELL3,RNWELL3,RNWELL1	; well2
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNWELL2,RNWELL3,RNWELL3,RNWELL2	; well3
	DC.B	RNSUMP,nway_wall,nway_wall,nway_wall,RNWELL2,nway_wall,nway_wall,nway_wall,0,0,RNSUMP,RNWELL2	; tunnel
	DC.B	RNBANK,0,0,0,RNTUNNEL,0,0,0,0,0,RNBANK,RNTUNNEL	; sump
	DC.B	0,0,0,0,RNSUMP,0,0,0,0,RNSHAFT1,0,0	; bank
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNBANK,RNCRAB1,0,0	; shaft1
	DC.B	0,nway_rockface,nway_rockface,nway_rockface,RNCAVE2,nway_rockface,nway_rockface,nway_rockface,RNSHAFT1,nway_rockface,0,0	; crab1
	DC.B	RNCRAB1,nway_rockface,RNCAVE3,nway_rockface,RNCAVE1,nway_rockface,RNFACE,nway_rockface,0,nway_rockface,0,0	; cave2
	DC.B	nway_rockface,nway_rockface,RNCAVE2,nway_rockface,nway_rockface,nway_rockface,nway_rockface,nway_rockface,nway_rockface,nway_rockface,RNCAVE2,RNCAVE2	; face
	DC.B	RNCAVE2,0,0,0,0,0,0,0,nway_misc3,0,nway_misc3,RNCAVE2	; cave1
	DC.B	nway_rockface,nway_rockface,nway_rockface,nway_rockface,nway_rockface,nway_rockface,RNCAVE2,nway_rockface,nway_rockface,0,0,0	; cave3
	DC.B	nway_rockface,nway_rockface,RNHGROUND,nway_rockface,nway_rockface,nway_rockface,RNROOM9,nway_rockface,nway_rockface,nway_rockface,RNROOM9,RNHGROUND	; entr
	DC.B	RNROOM2,nway_wall,nway_castlemoat,nway_castlemoat,nway_castlemoat,nway_castlemoat,nway_castlemoat,nway_wall,0,0,RNROOM2,RNROOM2	; passage3
	DC.B	0,RNPSG4,RNENTR,RNFRK,0,nway_ironbars,0,RNPSG2,0,0,0,0	; room9
	DC.B	0,0,0,RNROOM9,0,RNRMUD,0,RNFNRM,0,0,0,0	; psg2
	DC.B	nway_rockface,0,RNTJUNC1,0,nway_rockface,RNROOM9,nway_rockface,0,0,0,0,0	; psg4
	DC.B	RNTJUNC2,0,0,0,RNTJUNC3,0,RNPSG4,0,0,0,0,0	; tjunc1
	DC.B	0,0,RNAVRE,0,RNTJUNC1,0,RNGDN2,0,0,0,0,0	; tjunc2
	DC.B	RNTJUNC1,0,nway_door,0,0,0,RNHSE,0,0,0,RNHSE,0	; tjunc3
	DC.B	0,0,0,0,0,0,RNTJUNC2,0,0,0,0,0	; avre
	DC.B	0,0,RNTJUNC2,0,0,0,0,0,0,0,0,0	; gdn2
	DC.B	0,0,RNTJUNC3,0,0,0,0,0,0,0,0,0	; hse
	DC.B	nway_wall,nway_door,nway_wall,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,0,0,0,0	; offis
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,nway_door,0,0,0,0	; psg7
	DC.B	0,0,0,nway_door,0,0,0,0,0,0,0,0	; cage
	DC.B	0,0,0,nway_misc1,RNSHOP1,0,0,RNROOM9,0,0,0,0	; frk
	DC.B	RNFRK,0,0,0,nway_door,0,RNKBANK,0,0,0,0,0	; shop1
	DC.B	nway_door,nway_wall,nway_wall,nway_wall,RNSHOP3,nway_wall,nway_wall,nway_wall,0,0,0,0	; shop2
	DC.B	RNSHOP2,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,0	; shop3
	DC.B	0,0,0,0,0,0,0,nway_misc1,0,0,0,0	; cemtr
	DC.B	0,nway_ironbars,0,0,RNRVR,0,0,0,0,0,0,0	; rrm
	DC.B	RNRRM,0,0,0,0,0,RNTPWFL,0,0,0,0,0	; rvr
	DC.B	nway_misc3,nway_misc3,RNRVR,nway_misc3,nway_misc3,nway_misc3,0,nway_misc3,0,nway_misc3,0,0	; tpwfl
	DC.B	0,RNPSG2,0,0,0,0,0,0,0,0,0,0	; rmud
	DC.B	0,0,0,RNPSG2,0,0,RNSTPS,0,0,0,0,0	; fnrm
	DC.B	0,0,RNFNRM,0,0,0,0,0,RNBHSE,0,0,0	; stps
	DC.B	0,nway_fridge,nway_fridge,RNDRM5,0,0,0,0,0,RNSTPS,0,0	; bhse
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNDRM3,nway_wall,nway_wall,0,0,0,0	; drm1
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNDRM3,0,0,0,0	; drm2
	DC.B	nway_wall,RNDRM1,nway_wall,RNDRM2,nway_wall,RNDRM4,nway_wall,RNDRM5,0,0,0,0	; drm3
	DC.B	nway_wall,RNDRM3,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,0	; drm4
	DC.B	nway_fridge,nway_fridge,nway_wall,RNDRM3,nway_wall,nway_wall,RNBHSE,RNBHSE,0,0,0,0	; drm5
	DC.B	nway_wall,nway_wall,RNSQUAREDEATH,RNVIOLETSQUARE1,RNSQUAREDEATH,nway_wall,nway_wall,RNSWAY1,0,0,RNSWAY1,RNSWAY1	; blacksquare
	DC.B	RNSQUAREDEATH,nway_wall,nway_wall,RNCRYPT,nway_wall,nway_wall,RNSQUAREDEATH,RNVIOLETSQUARE2,0,0,RNCRYPT,RNCRYPT	; whitesquare
	DC.B	RNSQUAREDEATH,RNSQUAREDEATH,RNWHITESQUARE,nway_wall,nway_wall,nway_wall,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; redsquare1
	DC.B	RNBLACKSQUARE,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,nway_wall,nway_wall,nway_wall,0,0,nway_misc4,nway_misc4	; redsquare2
	DC.B	RNSQUAREDEATH,nway_wall,nway_wall,nway_wall,RNSQUAREDEATH,RNREDSQUARE1,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; orangesquare1
	DC.B	RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNREDSQUARE2,0,0,nway_misc4,nway_misc4	; orangesquare2
	DC.B	RNSQUAREDEATH,nway_wall,nway_wall,nway_wall,RNORANGESQUARE1,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; yellowsquare1
	DC.B	RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNORANGESQUARE2,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; yellowsquare2
	DC.B	nway_wall,nway_wall,RNSQUAREDEATH,RNYELLOWSQUARE1,RNSQUAREDEATH,RNYELLOWSQUARE2,RNSQUAREDEATH,nway_wall,0,0,nway_misc4,nway_misc4	; greensquare1
	DC.B	nway_wall,nway_wall,RNGREENSQUARE1,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,nway_wall,0,0,nway_misc4,nway_misc4	; bluesquare1
	DC.B	RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; bluesquare3
	DC.B	nway_wall,nway_wall,RNBLUESQUARE1,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,nway_wall,0,0,nway_misc4,nway_misc4	; indigosquare1
	DC.B	RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNBLUESQUARE3,RNSQUAREDEATH,RNBLUESQUARE1,0,0,nway_misc4,nway_misc4	; indigosquare2
	DC.B	RNBLUESQUARE3,RNSQUAREDEATH,RNSQUAREDEATH,nway_wall,nway_wall,nway_wall,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; indigosquare3
	DC.B	RNINDIGOSQUARE1,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; violetsquare1
	DC.B	RNINDIGOSQUARE2,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNSQUAREDEATH,RNINDIGOSQUARE3,RNSQUAREDEATH,RNSQUAREDEATH,0,0,nway_misc4,nway_misc4	; violetsquare2
	DC.B	0,0,0,0,0,0,0,0,0,0,0,0	; redsquare3
	DC.B	0,0,0,0,0,0,0,0,0,0,0,0	; orangesqure3
	DC.B	0,0,0,0,0,0,0,0,0,0,0,0	; greensquare2
	DC.B	0,0,0,0,0,0,0,0,0,0,0,0	; yellowsquare3
	DC.B	RNCRYPT,0,RNWHITESQUARE,0,RNBLACKSQUARE,0,0,0,0,0,0,0	; squaredeath
	DC.B	0,0,0,0,0,0,0,0,0,RNDUMMY,0,0	; oneroom
	DC.B	0,0,0,RNDUMMY,0,0,0,RNDUMMY,0,0,0,0	; tworoom
	DC.B	0,0,0,RNDUMMY,0,0,RNMANOFFIS,RNDUMMY,0,RNDUMMY,0,0	; threeroom
	DC.B	0,RNDUMMY,0,RNDUMMY,0,RNDUMMY,0,RNDUMMY,0,0,0,0	; fourroom
	DC.B	0,RNDUMMY,0,RNDUMMY,0,RNDUMMY,0,RNDUMMY,0,RNDUMMY,0,0	; fiveroom
	DC.B	0,RNDUMMY,RNDUMMY,RNDUMMY,0,RNDUMMY,RNDUMMY,RNDUMMY,0,0,0,0	; sixroom
	DC.B	0,0,0,0,0,0,0,0,0,0,0,0	; dummy
	DC.B	0,0,RNSHOP1,0,nway_door,0,0,0,0,0,0,0	; kbank
	DC.B	nway_door,0,RNTILL1,0,nway_door,0,nway_wall,0,0,0,0,0	; inbank
	DC.B	nway_door,0,nway_door,0,0,0,0,0,0,0,0,0	; manoffis
	DC.B	0,0,0,0,0,0,RNINBANK,0,0,0,0,0	; till1

NET_END

NET_LEN EQU NET_END-NETWORK

       END


* produced on Tue Jun 12 09:38:16 2018

* checksum = 56ec48e1
