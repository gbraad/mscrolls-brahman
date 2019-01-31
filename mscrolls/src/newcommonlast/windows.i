**************************************************************
*
*	%G%
*	%W%
*
**************************************************************

* First the equates for Windows themselves.
* The stylistic convection is to have the want's of the form;
*	wantWindowsNotifySomething
*
* And to have the corresponding handler routine for this case
* called  WindowsNotifySomething
*
* And to have the message header code that is used to comminicate
* with the windowing front end called;
* 	NotifySomething

    IFNE	GENERIC
		
wantWindowsNotifyExits		SET		TRUE	
wantWindowsNotifyNetwork	SET		TRUE
wantWindowsNotifyCurrentRoom	SET		TRUE
    
* Dependencies between Window wants.
	
    IFD	    	wantWindowsNotifyNetwork
wantWindowsNotifyExits	SET		TRUE
    ENDC	;wantWindowsNotifyNetwork

    ENDC	;GENERIC
			
NotifyExits		EQU	1 /* Sent by game to update a room */
NotifyNetwork		EQU	2 /* Sent by game before packet of above */
NotifyCurrentRoom	EQU	3 /* Sent by game, player's room has changed */
NotifyNetworkInvalid	EQU	4 /* network needs updating (load etc) */
NotifyTextCommand	EQU	5 /* send string to game as if command */
NotifyParsedCommand	EQU	6 /* send a command packet, bypass parser */
NotifyPicture		EQU	7 /* Location requires a picture. */
NotifyIcon		EQU	8 /* Icon resource name for an object */
NotifyIconNum		EQU	9	how many Icons are to follow	
