*
* scoreref.asm - included by things than need to use scores
*
	XREF	DO.SCORE

SREF	MACRO	* noun/room
	XREF	SC.\1
	ENDM


	SREF	LEAVEBUS		;leaving bus at right place
	SREF	DEFEATBULL		;Nobbling bull/cutting wire
	SREF	GETOIL			;Separating oil from water
	SREF	OPENMBX			;Open Xam's mbx
	SREF	GETSPECS		;Listening to magpie
	SREF	OPENBHOUSE		;Surprising Leprechaun
	SREF	LAUNCHCANOE		;Launching canoe
	SREF	CLEARDIRT		;Clearing dirt mound
*	SREF	CHARM1			;
*	SREF	CHARM2			;
*	SREF	CHARM3			;May be covered by others
*	SREF	CHARM4			;
*	SREF	CHARM5			;
	SREF	POSTSADDLE		;Posting saddle to station
	SREF	MUFFLEBELL		;Silencing post office bell
	SREF	OPENSAFE		;Opening post office safe
	SREF	ONGIRDER		;Getting to girder in clock tower
	SREF	GOCLOUD			;Successfully boarding cloud
	SREF	GETTICKET		;Getting train ticket
	SREF	CATCHTRAIN		;Catching the train
	SREF	ARRIVECASTLE		;Landing in walled garden wi unicorn
	SREF	RELEASEXAM		;waking Xam
	SREF	ENTERKITCHEN		;Entering castle kitchen
	SREF	OPENDOME		;Solving sliding block puzzle
*	SREF	FREEZEWORDS		;Freezing magic words (NOT USED)
	SREF	ASSEMBLEBRACELET	;Assembling the bracelet
	SREF	SEEWITCH
	SREF	WINNING
    SREF    DOBAKERY

             ALIGN


* no END - this is included!
