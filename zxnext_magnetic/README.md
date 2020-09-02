# Magnetic

This is a port of the Magnetic Scrolls interpreter Magnetic to Sinclair ZX
Spectrum Next. The original Magnetic Scrolls interpreter was developed and used
by the British company [Magnetic Scrolls](https://en.wikipedia.org/wiki/Magnetic_Scrolls)
to create text adventure games (interactive fiction) that would run on a variety
of 8-bit and 16/32-bit computers. Unlike Infocom and Level 9 Computing, who
developed their text adventure games in a custom bytecode language (Z-code and
A-code, respectively) that was executed by their interpreters, the Magnetic
Scrolls games were developed in a subset of the Motorola 68000 assembly language
(68K for short), which they called ELTHAM (Extra Low Tech Highly Ambiguous
Metacode). The Magnetic Scrolls interpreter, when running on a non-68K system,
was effectively a Motorola 68K virtual machine capable of running this subset of
68K assembly language plus a few custom instructions for I/O, graphics etc. The
historical reason for this somewhat unorthodox design is that the first version
of the interpreter was developed for Sinclair QL (which used a Motorola 68008
CPU) and this was then ported to other non-68K systems by creating a limited 68K
virtual machine. As a sidenote, the Magnetic Scrolls games were developed using
an internal tool called FRED and then compiled to ELTHAM (the Motorola 68K
assembly language subset).

Magnetic is a reverse-engineered Magnetic Scrolls interpreter originally
developed by Niclas Karlsson in portable C code. David Kinder, Stefan Meier and
Paul David Doherty have also contributed to the development of Magnetic. More
information about Magnetic is available at
[Magnetic Scrolls Memorial](http://msmemorial.if-legends.org/magnetic.php) and
the source code is available on [GitHub](https://github.com/DavidKinder/Magnetic).
Magnetic has been ported to many different systems and is also used by
[Strand Games](https://strandgames.com/) in their remasters of the Magnetic
Scrolls games for modern systems.

The main challenge when porting the Magnetic interpreter to Spectrum Next is that
it assumes a flat memory model where all memory is directly addressable and the
loaded game story file is being kept in main memory. The Spectrum Next and its
Z80 CPU is an 8-bit computer which can address only 64 KB of memory. Although
the Spectrum Next has 1 MB RAM where 768 KB is available to programs, this
memory has to be switched in to the 64 KB address space in 8 or 16 KB banks.

The design approach of the Spectrum Next port is to strip down the interpreter
to fit in the 64 KB main memory and switch in the currently used part of the
game in the bottom 16 KB of the main memory. 12 KB is used for the text screen
(using Timex hi-res mode), which leaves only 36 KB for the actual interpreter
(including interrupt handling code and stack memory). See
[memory_map.txt](memory_map.txt) for a detailed memory layout of the interpreter.
For comparison, the executable for the MS-DOS 16-bit version of the Magnetic
interpreter is 82 KB.

In order to make the interpreter fit in this limited memory, certain parts of it
have been removed. For example, support for loading, converting and displaying
Amiga-style graphics in run-time from a game graphics file has been removed.
The individual graphics files are instead manually converted to Spectrum Next's
layer 2 graphics format and loaded on demand.

The Magnetic interpreter runs Magnetic Scrolls games packaged as a .mag or .prg
game story file. The .mag file format was designed for the reverse-engineered
Magnetic interpreter. The .prg/.prog file format was used by the original
Magnetic Scrolls interpreter and has been reintroduced by Strand Games in their
remastered versions of the games. A game story file is composed of two or three
main parts; the actual game code (Motorola 68K machine code), a string section
(actually several but only in .mag) and a dictionary. When loaded by the
interpreter for Spectrum Next, these parts are loaded into a contiguous set of
8 KB banks. When a certain part of the game is referenced, e.g. an instruction
in the code, that bank and the next bank are switched in to the bottom 16 KB of
the main memory before being referenced. Certain performance-critical parts of
the interpreter code have been changed to minimize the amount of bank switching.

In addition to the actual code and an optional dictionary, a .prg file also
contains a symbol table (which is quite large) and relocation information
(which is quite small). Only a handful of the symbols are used by the Magnetic
interpreter and the relocation information is not used at all. To significantly
reduce the loading time of a .prg file, it is trimmed so that all unused symbols
and the relocation information is removed. The trimmed .prg file is generated
using the [trim_prog](tools/trim_prog) tool from the original .prog file used in
the desktop/mobile versions of the remastered games.

The .prg file format does not contain the string sections (as in the .mag file
format). The strings are instead placed in a separate .txt file. For Spectrum
Next, there is also an .idx file containing an index for the .txt file for
quickly looking up the offset of a string given its index. The .txt and .idx
files are generated using the [convert_text](tools/convert_text) tool from the
original .text file used in the desktop/mobile versions of the remastered games.
The .text file contains additional markup that is removed by this tool. The .txt
and .idx files are loaded into a contiguous set of 8 KB banks by the interpreter.

The Magnetic port to Spectrum Next is done using the
[z88dk](https://github.com/z88dk/z88dk) C compiler. In addition to modifying the
interpreter source code to support bank switching, some C constructs have been
changed to generate better Z80 machine code. For example, unsigned integers and
chars are used where possible, z88dk built-in functions for fast endian
conversion are used, the z88dk fastcall calling convention is used where
applicable, etc. Some parts are written in Z80 assembler for improved
performance. More technical information about the port can be found in the
emu.c file.

The screen of the Magnetic interpreter uses the Timex hi-res mode (or standard
ULA mode) as background layer for displaying the text with a proportional font.
The graphics is displayed using the layer 2 mode as the foreground layer and a
clip window to limit its height. At the top of the screen in the border area is
a status bar, composed of 16 hardware sprites, showing the current location to
its left and the score and number of turns to its right. When there are too many
lines of text to display on the screen at once, the message "&lt;MORE&gt;" will
appear at the bottom left of the screen in the border area using two hardware
sprites. The last entered line of input can be edited by pressing the EDIT key.
The graphics can be hardware scrolled up and down using the up and down arrow
keys to make more or less room for the text. If a PS/2 mouse is connected to the
Spectrum Next, it can also be used to scroll the graphics up and down by dragging
it with the mouse or using the mouse wheel.

The text colour can be changed by cycling downwards or upwards through a palette
of 32 colours by pressing TRUE VIDEO and INV VIDEO, respectively. The default
text colour is light grey.

If a remastered .prg game story file is used, the game mode can be toggled
between remastered and classic mode by pressing BREAK. By default, the game
starts in remastered mode. In classic mode, the bug fixes and playability
improvements provided by the remastered version of the game are ignored.
Any additional graphics is also skipped in classic mode.

The intro music is played with the [vt_sound](https://github.com/stefanbylund/vt_sound)
C API for the Vortex Tracker II player.

The table below shows the special keys used by the Magnetic interpreter:

| Spectrum Key |   PS/2 Key    |                               Description                                |
|--------------|---------------|--------------------------------------------------------------------------|
| BREAK        | SHIFT + SPACE | Toggle game mode between remastered (default) and classic mode.          |
| EDIT         | SHIFT + 1     | Edit last entered line of input.                                         |
| UP           | SHIFT + 7     | Scroll graphics up.                                                      |
| DOWN         | SHIFT + 6     | Scroll graphics down.                                                    |
| TRUE VIDEO   | SHIFT + 3     | Change text colour by cycling downwards through a palette of 32 colours. |
| INV VIDEO    | SHIFT + 4     | Change text colour by cycling upwards through a palette of 32 colours.   |
| Mouse        | Mouse         | Scroll graphics up and down by dragging it or using the mouse wheel.     |

## Games

The goal is to recreate all Magnetic Scrolls games for the Spectrum Next using
the Magnetic interpreter. The following is a list of all Magnetic Scrolls games
where the currently available games for Spectrum Next are marked with links.
Click on a game link to read about and download the game. Games marked as
remastered use the remastered game story file by Strand Games while games
marked as classic use the original game story file.

* [The Pawn (remastered)](games/pawn)
* [The Guild of Thieves (remastered)](games/guild)
* [Jinxter (remastered)](games/jinxter)
* [Corruption (classic)](games/corrupt)
* [Fish! (classic)](games/fish)
* [Myth (classic)](games/myth)
* Wonderland (classic)

## How to Build

If you want to build the Magnetic interpreter for Spectrum Next yourself, follow
the steps below:

1. On Windows, you need [MinGW/MSYS](http://www.mingw.org/wiki/msys),
[Cygwin](https://www.cygwin.com/), [UnxUtils](https://sourceforge.net/projects/unxutils/)
or similar for the basic Unix commands (GNU make, Bash, mkdir, rm, cp, cat and zip).
Make sure the Unix commands are available in your PATH environment variable.

2. Download and install [z88dk](https://github.com/z88dk/z88dk) v1.99C or later.
Add the environment variable ZCCCFG whose value should be &lt;z88dk&gt;/lib/config.
Also add &lt;z88dk&gt;/bin to your PATH environment variable.
**Note:** As of 2020-02-10, the optimisation rule z88dk-606ah in
&lt;z88dk&gt;/libsrc/_DEVELOPMENT/sdcc_peeph.3 must be removed or its condition
changed to `if notSame(%1 'h'), notSame(%1 '(hl)')`. If not, the optimised builds
(BUILD_OPT=true) of Jinxter, Corruption and Wonderland will not work properly.

3. Download and install the latest version of the [CSpect](http://www.cspect.org/)
or [ZEsarUX](https://github.com/chernandezba/zesarux) Spectrum Next emulator.

4. Download the Strand Games GitLab repository either as an archive using the
"Download" button at the top of this page or with Git using the following
command:

```
> git clone https://gitlab.com/strandgames/brahman.git
```

5. The Magnetic interpreter can either be built in the zxnext_magnetic top-level
directory as a development build (go to step 6) or in the appropriate
zxnext_magnetic/games/&lt;game&gt; directory as a release build (go to step 10).

6. Go to the zxnext_magnetic directory and enter the following command to do a
clean development build using the default configuration:

```
> make clean all
```

7. Note: There are many configuration options when building and they are handled
by the configure.m4 file. Either edit the configure.m4 file or pass configuration
options via make like this:

```
> make clean all CONFIG="-DGAME_FILE=pawn.mag -DGAME_DIR=pawn -DGAME_START_IMAGE=4
  -DUSE_TIMEX_HIRES=1 -DUSE_GFX=1 -DUSE_SOUND=1 -DUSE_MOUSE=1" BUILD_OPT=true
```

8. Note: The z88dk SDCC compiler is very slow when using the highest level of
optimisation. This can be controlled with BUILD_OPT=true or BUILD_OPT=false on
the make command-line. Setting BUILD_OPT=false or skipping it will give a much
quicker build but without the highest optimisations.

9. Note: Building with "make debug" instead of "make all" will create
zxnext_magnetic/src/*.lis files containing the generated Z80 assembly interleaved
with the C source code. The generated file zxnext_magnetic/bin/magnetic.map
contains all symbols and their addresses.

10. Go to the appropriate zxnext_magnetic/games/&lt;game&gt; directory and enter
the following command to do a clean release build of the selected game:

```
> make clean all
```

11. Run the generated binary in the [CSpect](http://www.cspect.org/) or
[ZEsarUX](https://github.com/chernandezba/zesarux) emulator.
If you want to run the development build of a game, i.e. zxnext_magnetic/bin/magnetic.nex,
use zxnext_magnetic/games/&lt;game&gt; as the root directory in the emulator.
If you want to run the release build of a game in the zxnext_magnetic/games/&lt;game&gt;
directory, see the README.md file in that directory for more information or
simply run the run_cspect.bat or run_zesarux.bat batch file contained therein.

## Source Code

This section briefly describes the main source files of the Magnetic interpreter
for Spectrum Next.

* emu.c <br>
Implementation of the core part of the Magnetic interpreter. Theoretically,
this file is platform-independent and should not be modified when porting to
a new target. However, the original implementation assumes a flat memory model
and the loaded game story file being kept in its entirety in main memory. The
Spectrum Next has only 64 KB addressable memory and a banked memory model.
The interpreter has been refactored to fit in the main memory and to load the
game story file into a contiguous set of memory banks that are mapped in as
needed to the bottom 16 KB of the 64 KB address space. Several changes have
been done to improve the performance for Spectrum Next.

* main.c <br>
Implementation of the platform-dependent part of the Magnetic interpreter.
Handles the user interface, terminal I/O, main loop, displaying of location
images, saving/loading of game state and playback of a script file (only in
test mode).

* interrupt.asm <br>
Module for setting up IM2 interrupt mode.

* status_bar.c <br>
Module for printing the game status in the status bar above the screen in the
top border using hardware sprites. The status text is first drawn in an
off-screen layer 2 memory area using layer2_fzx and then transferred to the
sprite BRAM.

* scroll_prompt.asm <br>
Module for showing/hiding the scroll prompt.

* text_color.asm <br>
Module for changing the text colour by cycling through a palette of 32 colours.

* layer2.c <br>
Module for loading, displaying and handling layer 2 images.

* layer2_fzx.asm <br>
FZX proportional font drawing function for layer 2. Used together with the FZX
proportional font driver in z88dk for printing text on the layer 2 screen or a
layer 2 off-screen memory area.

* sprite.c <br>
Module for loading, displaying and handling hardware sprites. Used for displaying
the status bar, scroll prompt and mouse pointer.

* mouse.c <br>
IM2 ISR for Kempston mouse support. Updates the mouse pointer sprite and invokes
a supplied mouse listener.

* sound.c <br>
Module for playing music using the Vortex Tracker II player.

* music_module.asm.m4 <br>
Module for bringing in the intro music PT3 module.

* gfx_util.asm <br>
Module for handling scrolling of the location image using the keyboard or mouse.

* crt_driver_instantiation.asm.m4 <br>
Custom z88dk module for setting up the input and output terminals.

* zx_01_input_kbd_inkey_custom.asm <br>
Input terminal driver subclass for handling special keys for location image
scrolling, changing text colour, edit the last entry etc.

* zx_01_output_fzx_custom.asm <br>
FZX output terminal driver subclass for standard ULA mode. Handles the scroll
prompt and scroll limit computations.

* tshr_01_output_fzx_custom.asm <br>
FZX output terminal driver subclass for Timex hi-res mode. Handles the scroll
prompt and scroll limit computations.

* in_key_translation_table.asm <br>
Overridden z88dk module for redefining the ASCII code for some special keys.

* asm_in_mouse_kempston.asm <br>
Overridden z88dk Kempston mouse driver.

* asm_in_mouse_kempston_wheel.asm <br>
Overridden z88dk Kempston mouse wheel driver.

* stack.asm <br>
Test module for filling the stack with a predefined data pattern and calculating
the peak stack usage.

* esxdos_util.c <br>
Test module with ESXDOS utility functions used when playing a script file.

## Links

* [Magnetic interpreter (original)](https://github.com/DavidKinder/Magnetic) <br>
  The source code of the original Magnetic interpreter, which the Spectrum Next
  porting is originally based on.

* [Magnetic interpreter (Strand Games)](https://gitlab.com/strandgames/brahman) <br>
  The source code of an enhanced version of the original Magnetic interpreter
  developed and used by Strand Games in their remasters of the Magnetic Scrolls
  games for modern systems. Here you can also find the source code of the
  original Magnetic Scrolls development tools (fred23jr, as68, lnk and sim).

* [Magnetic Scrolls Memorial](http://msmemorial.if-legends.org/) <br>
  The best source of information about Magnetic Scrolls and their games.

## Credits

A big thank you to Allen Albright for his contributions, performance improvements,
z88dk expertise and general helpfulness in this project.

Thanks to Hugh Steers at Strand Games for help with adding support for the prog
game story file format to the Magnetic interpreter for Spectrum Next.

Thanks to Phoebus Dokos for creating the beautiful font for the Magnetic
interpreter and for lending me a Spectrum Next development board.

## License

Magnetic is licensed under the terms of the GNU General Public License version 2
and is copyright (C) 1997-2008 by Niclas Karlsson. Developed by Niclas Karlsson,
David Kinder, Stefan Meier and Paul David Doherty.

The Magnetic interface for Sinclair ZX Spectrum Next is copyright (C) 2018 by
Stefan Bylund.

The QLStyle font is copyright (C) 2018 by Phoebus Dokos.

The Vortex Tracker II player in the file PT3PROM.asm is copyright (C) 2004-2007
by Sergey Bulba.
