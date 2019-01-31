***************************************************************************
*
*
*	fred23jr output from file '../jinxter'*
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


	DC.B	nway_door,nway_passengers,nway_passengers,nway_passengers,nway_passengers,nway_passengers,nway_passengers,nway_passengers,0,0,0,0	; onbus
	DC.B	nway_door,nway_flowers,nway_flowers,nway_flowers,nway_gate,nway_flowers,nway_flowers,nway_flowers,0,0,0,0	; frontgarden1
	DC.B	nway_wall,nway_wall,RNCONSERVATORY,nway_wall,RNBEDROOM1,nway_wall,nway_wall,nway_wall,0,0,0,0	; kitchen1
	DC.B	RNKITCHEN1,RNBATHROOM1,nway_wall,RNHALLWAY1,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,0	; bedroom1
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNBEDROOM1,nway_wall,nway_wall,0,0,0,RNBEDROOM1	; bathroom1
	DC.B	RNCONSERVATORY,nway_wall,nway_wall,nway_wall,nway_wall,RNHALLWAY1,nway_wall,nway_wall,0,0,RNCONSERVATORY,RNHALLWAY1	; library1
	DC.B	nway_door,nway_wall,nway_wall,nway_wall,RNLIBRARY1,nway_wall,RNKITCHEN1,nway_wall,0,0,0,0	; conservatory
	DC.B	RNCOUNTRYLANE2,0,0,0,nway_door,0,0,0,0,0,0,RNCOUNTRYLANE2	; backgarden1
	DC.B	nway_hedgerows,nway_hedgerows,RNFIELD1,RNWOOD1,RNBACKGARDEN1,nway_hedgerows,nway_hedgerows,nway_hedgerows,0,0,RNBACKGARDEN1,0	; countrylane2
	DC.B	nway_gate,0,RNINFINITE1,0,0,0,RNINFINITE2,0,0,0,0,0	; countrylane1
	DC.B	nway_wall,RNLIBRARY1,nway_wall,nway_wall,nway_door,nway_wall,nway_wall,RNBEDROOM1,0,0,0,0	; hallway1
	DC.B	0,0,0,RNORCHARD,RNWOOD1,0,RNCOUNTRYLANE2,0,0,0,0,0	; field1
	DC.B	RNFIELD1,0,RNORCHARD,0,RNFIELD2,0,0,RNCOUNTRYLANE2,0,0,0,0	; wood1
	DC.B	RNWOOD1,0,RNORCHARD,0,0,0,0,0,0,0,0,0	; field2
	DC.B	0,0,RNSTREAM1,RNBRIDGE1,RNFRONTGARDEN2,0,RNFIELD2,RNFIELD1,0,0,0,0	; orchard
	DC.B	nway_wall,nway_door,RNCLOCK,nway_wall,nway_wall,nway_wall,nway_fire,nway_wall,0,0,RNCLOCK,0	; clockmakers
	DC.B	nway_bank,nway_bank,nway_bank,nway_bank,nway_bank,nway_bank,RNORCHARD,nway_bank,0,0,0,0	; stream1
	DC.B	nway_edge,nway_edge,nway_edge,nway_edge,RNBOATHOUSE,nway_edge,nway_edge,RNORCHARD,0,0,0,0	; bridge1
	DC.B	RNORCHARD,nway_flowers,nway_flowers,nway_flowers,nway_flowers,nway_door,nway_flowers,nway_flowers,0,0,0,0	; frontgarden2
	DC.B	nway_wall,nway_door,nway_wall,nway_wall,nway_wall,nway_wall,RNDININGROOM1,nway_wall,0,nway_trapdoor,0,0	; kitchen2
	DC.B	nway_wall,nway_wall,RNKITCHEN2,nway_wall,RNSTUDY1,nway_wall,nway_wall,nway_wall,0,0,0,0	; diningroom1
	DC.B	RNDININGROOM1,nway_wall,nway_wall,nway_wall,nway_wall,RNBACKGARDEN2,nway_wall,nway_wall,0,0,0,0	; study1
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_trapdoor,0,0,0	; cellar2
	DC.B	0,RNSTUDY1,0,0,RNHOLLYBUSH,0,0,0,0,0,0,0	; backgarden2
	DC.B	RNBACKGARDEN2,nway_holly,nway_holly,nway_holly,nway_holly,nway_holly,nway_holly,nway_holly,0,0,nway_holly,RNBACKGARDEN2	; hollybush
	DC.B	RNBRIDGE1,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,nway_wall,nway_wall,0,0,0,0	; boathouse
	DC.B	nway_door,0,0,0,RNLAKE1,0,0,0,0,0,0,0	; canoe_mooring
	DC.B	RNCANOE_MOORING,nway_current,nway_current,nway_current,RNPAGODA,RNWOOD2,nway_current,nway_current,0,RNUNDERWATER,0,0	; lake1
	DC.B	RNLAKE1,nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,0,RNSTEPS1,RNSTEPS1,0	; pagoda
	DC.B	nway_wall,nway_wall,nway_dirt,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNPAGODA,0,0,RNPAGODA	; steps1
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,0,RNOFFICE,RNOFFICE,0	; corridor1
	DC.B	nway_wall,nway_wall,nway_door,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNCORRIDOR1,0,0,RNCORRIDOR1	; office
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNBEERGARDEN,0,0,RNBEERGARDEN	; bottom_well
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNCONNECTING_PASSAGE,nway_wall,0,0,0,RNCONNECTING_PASSAGE	; divers_room
	DC.B	nway_wall,nway_wall,nway_door,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_hatch,0,0,0	; airlock
	DC.B	RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,0,RNUNDERWATER2,RNLAKE1,0,0,0	; underwater
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNOFFICE,nway_wall,0,0,0,RNOFFICE	; library2
	DC.B	nway_trees,RNLAKE1,nway_trees,nway_trees,RNFAIRGROUND,nway_trees,nway_trees,nway_trees,0,0,RNLAKE1,RNFAIRGROUND	; wood2
	DC.B	RNWOOD2,nway_door,0,RNPUB,nway_darktower,RNCAROUSEL,0,RNBAKERY,0,0,RNBAKERY,0	; fairground
	DC.B	0,RNFAIRGROUND,0,0,0,nway_door,0,0,0,0,0,0	; carousel
	DC.B	nway_wall,nway_wall,nway_wall,RNFAIRGROUND,nway_wall,nway_wall,nway_wall,nway_wall,0,RNBAKERY_OVEN,RNBAKERY_OVEN,RNFAIRGROUND	; bakery
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNBAKERY_LARDER,nway_wall,RNBAKERY,0,RNBAKERY_LARDER,RNBAKERY	; bakery_oven
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNCLOCKMAKERS,nway_wall,0,0,0,RNCLOCKMAKERS	; clock
	DC.B	RNON_PLATFORM,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,0,RNCLOCK,0,0	; girder_room
	DC.B	nway_beam,nway_edge,nway_edge,nway_edge,RNGIRDER_ROOM,nway_edge,nway_edge,nway_edge,0,0,0,0	; on_platform
	DC.B	0,0,0,0,nway_beam,0,0,0,0,RNFAIRGROUND,nway_beam,0	; end_of_beam
	DC.B	RNBEERGARDEN,nway_wall,nway_wall,nway_wall,nway_tobar,nway_wall,nway_wall,RNFAIRGROUND,0,0,RNBEERGARDEN,RNFAIRGROUND	; pub
	DC.B	nway_wall,nway_wall,RNJEWEL_ROOM,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,0,0,0,0	; library4
	DC.B	0,0,0,0,RNPUB,0,0,0,0,RNBOTTOM_WELL,RNBOTTOM_WELL,RNPUB	; beergarden
	DC.B	0,0,RNTRAIN_STATION,0,0,0,0,0,0,0,RNTRAIN_STATION,0	; outside_station
	DC.B	nway_wall,0,RNPLATFORM1,0,0,0,RNOUTSIDE_STATION,0,0,0,0,RNOUTSIDE_STATION	; train_station
	DC.B	RNPLATFORM2,0,RNON_TRAIN,0,0,0,RNTRAIN_STATION,0,0,0,0,0	; platform1
	DC.B	nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,nway_steep,0,nway_steep,0,0	; top_mountain
	DC.B	nway_wall,nway_wall,RNQUADRANGLE,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNCASTLE_PARAPETS,0,0,0	; walled_garden
	DC.B	nway_hatch,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,nway_wall,nway_wall,0,0,0,0	; dungeon
	DC.B	nway_wall,nway_wall,RNBALCONY,nway_wall,RNHALLWAY,nway_wall,nway_wall,nway_wall,0,0,0,0	; witch_chamber
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,RNBANQUET_HALL,0,0	; chimney
	DC.B	nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,0,RNWALLED_GARDEN,0,0	; castle_parapets
	DC.B	nway_door,nway_wall,RNPASSAGE,nway_wall,RNQUADRANGLE,nway_wall,nway_wall,nway_wall,0,0,RNPASSAGE,RNQUADRANGLE	; archway
	DC.B	RNARCHWAY,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNWALLED_GARDEN,nway_wall,0,0,RNARCHWAY,RNWALLED_GARDEN	; quadrangle
	DC.B	nway_wall,RNANTECHAMBER,nway_wall,nway_wall,nway_door,nway_wall,nway_wall,RNTOWER_ENTRANCE,RNCHIMNEY,RNCASTLE_KITCHEN,0,RNCASTLE_KITCHEN	; banquet_hall
	DC.B	nway_wall,nway_wall,nway_wall,RNBANQUET_HALL,nway_wall,nway_wall,nway_wall,nway_wall,RNSPIRAL_STAIRCASE,0,0,RNBANQUET_HALL	; tower_entrance
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNHALLWAY,RNTOWER_ENTRANCE,RNHALLWAY,RNTOWER_ENTRANCE	; spiral_staircase
	DC.B	nway_door,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,RNSPIRAL_STAIRCASE,0,RNSPIRAL_STAIRCASE	; hallway
	DC.B	nway_wall,nway_wall,nway_door,nway_wall,nway_wall,RNBANQUET_HALL,nway_wall,nway_wall,0,0,0,RNBANQUET_HALL	; antechamber
	DC.B	nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,nway_edge,RNWITCH_CHAMBER,nway_edge,0,0,RNWITCH_CHAMBER,0	; balcony
	DC.B	0,0,RNINFINITE1,0,0,0,RNCOUNTRYLANE1,0,0,0,0,0	; infinite1
	DC.B	0,0,RNCOUNTRYLANE1,0,0,0,RNINFINITE2,0,0,0,0,0	; infinite2
	DC.B	nway_wall,nway_wall,RNDIVERS_ROOM,nway_wall,nway_wall,nway_wall,RNSTEPS1,nway_wall,0,0,RNDIVERS_ROOM,RNSTEPS1	; connecting_passage
	DC.B	RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNONLAKE,0,0,0	; inf_underwater
	DC.B	RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNUNDERWATER2,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNONLAKE,RNINF_UNDERWATER,0,0	; over_hatch
	DC.B	nway_current,nway_current,nway_current,nway_current,nway_current,nway_current,nway_current,nway_current,0,RNINF_UNDERWATER,0,0	; onlake
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,RNLIBRARY2,0,RNLIBRARY2	; topof_library2
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_door,nway_wall,nway_wall,0,0,0,0	; postoffice
	DC.B	nway_wall,nway_wall,RNBAKERY_OVEN,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,RNBAKERY_OVEN	; bakery_larder
	DC.B	RNOVER_HATCH,RNINF_UNDERWATER,RNINF_UNDERWATER,RNUNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNONLAKE,0,0,0	; underwater2
	DC.B	RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNINF_UNDERWATER,RNONLAKE,RNSHIPWRECK,0,0	; over_wreck
	DC.B	nway_wreckage,nway_wreckage,nway_wreckage,nway_wreckage,nway_wreckage,nway_wreckage,nway_wreckage,nway_wreckage,RNOVER_WRECK,0,0,RNOVER_WRECK	; shipwreck
	DC.B	nway_noway,nway_noway,nway_noway,nway_noway,nway_noway,nway_noway,nway_noway,nway_noway,nway_noway,nway_noway,0,0	; falling
	DC.B	RNPLATFORM3,0,nway_train,0,RNPLATFORM1,0,0,0,RNPLATFORM_BRIDGE,0,0,0	; platform2
	DC.B	RNRAILWAY_TRACK,0,nway_train,0,RNPLATFORM2,0,nway_door,0,0,0,0,0	; platform3
	DC.B	nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,RNPLATFORM3,nway_toorisky,nway_toorisky,nway_toorisky,0,0,0,0	; railway_track
	DC.B	nway_wall,nway_wall,nway_door,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,0	; post_depot
	DC.B	nway_carriage,nway_carriage,nway_carriage,nway_carriage,nway_carriage,nway_carriage,RNPLATFORM1,nway_carriage,0,0,0,0	; on_train
	DC.B	nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,nway_cantmove,0,0	; theatre
	DC.B	nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,0,0,0,0	; guards_van
	DC.B	nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,RNON_TRAIN,nway_toorisky,0,0	; hanging_on_train
	DC.B	nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,nway_toorisky,RNPLATFORM_BRIDGE,0,0,0	; train_roof
	DC.B	nway_railings,nway_railings,RNOTHER_PLATFORM,nway_railings,nway_railings,nway_railings,RNPLATFORM2,nway_railings,0,0,0,0	; platform_bridge
	DC.B	nway_deadend,nway_deadend,nway_deadend,nway_deadend,nway_deadend,nway_deadend,nway_deadend,nway_deadend,RNPLATFORM_BRIDGE,0,0,0	; other_platform
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNARCHWAY,nway_wall,0,RNGAOL,RNGAOL,RNARCHWAY	; passage
	DC.B	nway_door,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNPASSAGE,0,0,RNPASSAGE	; gaol
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,RNIN_HATCH,nway_wall,nway_wall,nway_wall,RNBANQUET_HALL,0,RNBANQUET_HALL,RNBANQUET_HALL	; castle_kitchen
	DC.B	nway_hatch,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,0,0,0,0	; in_hatch
	DC.B	nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,nway_wall,RNLIBRARY4,nway_wall,0,0,0,0	; jewel_room
	DC.B	nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,nway_oven,0,0	; inoven
	DC.B	0,0,0,0,RNOUTSIDE_STATION,0,0,0,0,0,0,0	; waterfall
	DC.B	0,0,0,0,0,0,0,0,0,0,0,0	; dummy1

NET_END

NET_LEN EQU NET_END-NETWORK

       END


