/*******************************************************************************
 *
 * Magnetic - Magnetic Scrolls Interpreter
 *
 * Written by Niclas Karlsson <nkarlsso@abo.fi>,
 *            David Kinder <davidk@davidkinder.co.uk>,
 *            Stefan Meier <Stefan.Meier@if-legends.org> and
 *            Paul David Doherty <pdd@if-legends.org>
 *
 * Copyright (C) 1997-2008 Niclas Karlsson
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111, USA.
 *
 ******************************************************************************/

/*******************************************************************************
 * The following APIs have been removed from the original header file:
 *
 * - Displaying Amiga-style images from a game graphics file.
 * - Displaying animations from a game graphics file.
 * - Playing MIDI sound from an optional game sound file.
 * - Showing hints from an optional game hints file.
 * - Unused utility functions ms_is_running(), ms_is_magwin() and ms_count().
 ******************************************************************************/

#ifndef MAGNETIC_DEFS_H
#define MAGNETIC_DEFS_H

#include "zconfig.h"
#include "ide_friendly.h"

/******************************************************************************\
* Type definitions for Magnetic
*
* Note: When running into trouble please ensure that these types have the
*       correct number of bits on your system !!!
\******************************************************************************/

typedef unsigned char type8;
typedef signed   char type8s;
typedef unsigned int  type16;
typedef signed   int  type16s;
typedef unsigned long type32;
typedef signed   long type32s;

/******************************************************************************\
* Abstract functions
*
* Note: These functions MUST be implemented by each port of Magnetic!
\******************************************************************************/

/******************************************************************************\
* Function: ms_load_file
*
* Purpose: Load a saved game file and restore game.
*
* Parameters:   type8*  name            zero terminated string of filename
*                                       typed by player
*               type8*  ptr             pointer to data to load
*               type16 size             number of bytes to load
*
* Result: 0 is successful
*
* Note: You probably want to put in a file requester!
\******************************************************************************/

type8 ms_load_file(type8 *name, type8 *ptr, type16 size);

/******************************************************************************\
* Function: ms_save_file
*
* Purpose: Save the current game to file.
*
* Parameters:   type8*  name            zero terminated string of filename
*                                       typed by player
*               type8*  ptr             pointer to data to save
*               type16  size            number of bytes to save
*
* Result: 0 is successful
*
* Note: You probably want to put in a file requester!
\******************************************************************************/

type8 ms_save_file(type8 *name, type8 *ptr, type16 size);

/******************************************************************************\
* Function: ms_statuschar
*
* Purpose: Output a single character to the status bar.
*
* Parameters:   type8   c               character to be printed
*
* Note: All characters are printed as provided except for:
*       0x0A resets the x position to zero
*       0x09 moves the cursor to the right half of the bar, i.e. 'width-11'
\******************************************************************************/

void ms_statuschar(type8 c) __z88dk_fastcall;

/******************************************************************************\
* Function: ms_putchar
*
* Purpose: Output a single character to the game/text windows.
*
* Parameters:   type8   c               character to be printed
*
* Note: It is highly recommended to buffer the output, see also ms_flush().
\******************************************************************************/

void ms_putchar(type8 c) __z88dk_fastcall;

/******************************************************************************\
* Function: ms_flush
*
* Purpose: Flush the output buffer (if applicable).
*
* Note: See also ms_putchar().
\******************************************************************************/

void ms_flush(void);

/******************************************************************************\
* Function: ms_getchar
*
* Purpose: Read user input, buffered.
*
* Parameters:   type8   trans           if not 0, translate any #undo
*                                       input to a return code of 0
*
* Return: One character
*
* Note: The first time it is called a string should be read and then given
*       back one byte at a time (i.e. one for each call) until a '\n' is
*       reached (which will be the last byte sent back before it all restarts).
*       Returning a zero means 'undo' and the rest of the line must then be
*       ignored.
*       Returning 1 means that the opcode should return immediately. This is
*       needed to prevent possible corruption of the game's memory in
*       interpreters which allow a new game to be loaded without restarting.
\******************************************************************************/

type8 ms_getchar(type8 trans) __z88dk_fastcall;

/******************************************************************************\
* Function: ms_showpic
*
* Purpose: Displays or hides a picture.
*
* Parameter:    type8   c       number/name of image to be displayed
*               type8   mode    mode == 0 means gfx off,
*                               mode == 1 gfx on thumbnails,
*                               mode == 2 gfx on normal.
*
* Note: For retrieving the raw data of a picture call ms_extract().
\******************************************************************************/

#if USE_ANIM
void ms_showpic(type8 *c, type8 mode);
#else
void ms_showpic(type8 c, type8 mode);
#endif

/******************************************************************************\
* Function: ms_fatal
*
* Purpose: Handle fatal interpreter error.
*
* Parameter:    type8*  txt     message
\******************************************************************************/

void ms_fatal(type8 *txt) __z88dk_fastcall;

/******************************************************************************\
* Magnetic core functions
*
* Note: These functions SHOULD be used somewhere in your port!
\******************************************************************************/

/******************************************************************************\
* Function: ms_init
*
* Purpose: Loads the interpreter with a game.
*
* Parameters:   type8*  name            filename of story file
*
* Return:       0 = failure
*               1 = success
*
* Note: You must call this function before starting the ms_rungame() loop.
\******************************************************************************/

type8 ms_init(type8 *name) __z88dk_fastcall;

/******************************************************************************\
* Function: ms_rungame
*
* Purpose: Executes an interpreter instruction.
*
* Return: True if successful
*
* Note: You must call this function in a loop like this:
*       while (running) {running = ms_rungame();}
\******************************************************************************/

type8 ms_rungame(void);

/******************************************************************************\
* Function: ms_freemen
*
* Purpose: Frees all allocated resources.
\******************************************************************************/

void ms_freemem(void);

/******************************************************************************\
* Function: ms_seed
*
* Purpose: Initializes the interpreter's random number generator with
*          the given seed.
*
* Parameter:    type32  seed    seed
\******************************************************************************/

void ms_seed(type32 seed) __z88dk_fastcall;

/******************************************************************************\
* Function: ms_stop
*
* Purpose: Stops further processing of opcodes.
\******************************************************************************/

void ms_stop(void);

/******************************************************************************\
* Function: ms_status
*
* Purpose: Dumps interpreter state to stderr, i.e. registers.
\******************************************************************************/

void ms_status(void);

#endif /* MAGNETIC_DEFS_H */
