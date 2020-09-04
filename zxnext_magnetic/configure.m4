divert(-1)

################################################################################
# CONFIGURE THE COMPILE HERE
#
# Compile-time configuration of the Magnetic Scrolls interpreter for Sinclair
# ZX Spectrum Next.
#
# This M4 configuration file generates the following files:
# - zconfig.h    (TARGET=1): C include file
# - zconfig.inc  (TARGET=2): Assembly include file
# - zconfig.m4   (TARGET=3): M4 include file
# - zpragma.inc  (TARGET=4): Pragma file
# - zproject.lst (TARGET=5): List of source files to compile
#
# The following definitions are also configurable from the M4 command-line:
# - USE_PROG_FORMAT
# - GAME_FILE
# - GAME_DIR
# - GAME_START_IMAGE
# - GAME_LAST_IMAGE
# - GAME_VERSION_STR
# - USE_TIMEX_HIRES
# - USE_GFX
# - MAX_IMAGE_HEIGHT
# - USE_SOUND
# - USE_MOUSE
# - USE_SCRIPT
# - USE_LOGEMU
# - USE_IMAGE_SLIDESHOW
# - MS_FATAL_CHECK
# - FAST_COMPILE (Winmake.bat only)
################################################################################

# Game

# Non-zero to enable PROG game story file format, default is MAG file format.
ifdef(`USE_PROG_FORMAT',, `define(`USE_PROG_FORMAT', 0)')

# Game story file, default is "pawn.mag".
ifdef(`GAME_FILE', `define(`GAME_FILE', "GAME_FILE")', `define(`GAME_FILE', "pawn.mag")')

# Game directory, default is pawn.
ifdef(`GAME_DIR', `define(`GAME_DIR', GAME_DIR)', `define(`GAME_DIR', pawn)')

# Image number of the game's start location image, default is 4 as for pawn.mag.
ifdef(`GAME_START_IMAGE',, `define(`GAME_START_IMAGE', 4)')

# Image number of the game's last location image, default is 29 as for pawn.mag.
# Used by the image slideshow test module.
ifdef(`GAME_LAST_IMAGE',, `define(`GAME_LAST_IMAGE', 29)')

# Version string of the game, default is not defined.
ifdef(`GAME_VERSION_STR', `define(`GAME_VERSION_STR', "GAME_VERSION_STR")')

# Text input

# Time in ms @ 3.5 MHz before a key starts repeating. Default is 500 ms @ 3.5 MHz.
# At 28 MHz, we multiply with a factor of 8.
define(`KEY_REPEAT_START', 4000)

# Time in ms @ 3.5 MHz between key repeats. Default is 15 ms @ 3.5 MHz.
# At 28 MHz, we multiply with a factor of 8.
define(`KEY_REPEAT_RATE', 120)

# ASCII code associated with CAPS SHIFT + 7 (up arrow key).
define(`ASCII_CODE_UP', 253)

# ASCII code associated with CAPS SHIFT + 6 (down arrow key).
define(`ASCII_CODE_DOWN', 254)

# ASCII code associated with CAPS SHIFT + SPACE (break key).
define(`ASCII_CODE_BREAK', 252)

# Text output

# Non-zero to enable Timex hi-res mode for text, default is ULA mode.
ifdef(`USE_TIMEX_HIRES',, `define(`USE_TIMEX_HIRES', 0)')

# Height of text window in characters.
define(`TEXT_WINDOW_HEIGHT', 24)

# FZX font used for text window.
#define(`TEXT_FONT', _ff_ao_Prefect)
define(`TEXT_FONT', _ff_pd_QLStyle)

# Text font color index (0 - 31), default is 1 (light gray).
define(`TEXT_FONT_COLOR_INDEX', 1)

# Status font RGB332 color, default is orange.
define(`STATUS_FONT_COLOR', 0xD4)

# Graphics

# Non-zero to enable intro and location graphics, default is no graphics.
ifdef(`USE_GFX',, `define(`USE_GFX', 0)')

# Height of all location images in pixels.
ifdef(`MAX_IMAGE_HEIGHT',, `define(`MAX_IMAGE_HEIGHT', 136)')

# Sound

# Non-zero to enable intro music, default is no music.
ifdef(`USE_SOUND',, `define(`USE_SOUND', 0)')

# Mouse

# Non-zero to enable mouse support, default is no mouse support.
ifdef(`USE_MOUSE',, `define(`USE_MOUSE', 0)')

# Script

# Non-zero to enable script playing support, default is disabled.
# Used for debugging and testing.
ifdef(`USE_SCRIPT',, `define(`USE_SCRIPT', 0)')

# Log for Magnetic Scrolls 68K emulator / interpreter

# Non-zero to log emulator activity to file, default is off.
ifdef(`USE_LOGEMU',, `define(`USE_LOGEMU', 0)')

# Name of emulator log file.
define(`LOG_FILE', "log.txt")

# Image slideshow

# Non-zero to enable image slideshow, default is off.
# Used for debugging and testing.
ifdef(`USE_IMAGE_SLIDESHOW',, `define(`USE_IMAGE_SLIDESHOW', 0)')

# Runtime error checks

# Non-zero to enable performance-heavy emulator fatal error checks, default is off.
ifdef(`MS_FATAL_CHECK',, `define(`MS_FATAL_CHECK', 0)')

################################################################################
# NO CHANGES BELOW
################################################################################

# Height of all location images in characters.
define(`MAX_IMAGE_HEIGHT_IN_CHARS', eval((MAX_IMAGE_HEIGHT / 8) + ((MAX_IMAGE_HEIGHT % 8) != 0)))

################################################################################
# FILE: zconfig.h
################################################################################

ifelse(TARGET, 1,
`
divert
`#ifndef' _ZCONFIG_H
`#define' _ZCONFIG_H

`#define' `USE_PROG_FORMAT' USE_PROG_FORMAT
`#define' `GAME_FILE' GAME_FILE
`#define' `GAME_START_IMAGE' GAME_START_IMAGE
`#define' `GAME_LAST_IMAGE' GAME_LAST_IMAGE
ifdef(`GAME_VERSION_STR', `#define' `GAME_VERSION_STR' GAME_VERSION_STR, `dnl')

`#define' `USE_TIMEX_HIRES' USE_TIMEX_HIRES
`#define' `TEXT_WINDOW_HEIGHT' TEXT_WINDOW_HEIGHT
`#define' `TEXT_FONT_COLOR_INDEX' TEXT_FONT_COLOR_INDEX
`#define' `STATUS_FONT_COLOR' STATUS_FONT_COLOR

`#define' `USE_GFX' USE_GFX
`#define' `MAX_IMAGE_HEIGHT' MAX_IMAGE_HEIGHT
`#define' `MAX_IMAGE_HEIGHT_IN_CHARS' MAX_IMAGE_HEIGHT_IN_CHARS

`#define' `USE_SOUND' USE_SOUND

`#define' `USE_MOUSE' USE_MOUSE

`#define' `USE_SCRIPT' USE_SCRIPT

ifelse(USE_LOGEMU, 0,,
`
`#define' `LOGEMU'
')dnl
`#define' `LOG_FILE' LOG_FILE

`#define' `USE_IMAGE_SLIDESHOW' USE_IMAGE_SLIDESHOW

`#define' `MS_FATAL_CHECK' MS_FATAL_CHECK

`#endif'
divert(-1)
')

################################################################################
# FILE: zconfig.inc
################################################################################

ifelse(TARGET, 2,
`
divert
IFNDEF _ZCONFIG_INC
DEFC _ZCONFIG_INC = 1

defc `USE_PROG_FORMAT' = USE_PROG_FORMAT

defc `KEY_REPEAT_RATE' = KEY_REPEAT_RATE
defc `ASCII_CODE_UP' = ASCII_CODE_UP
defc `ASCII_CODE_DOWN' = ASCII_CODE_DOWN
defc `ASCII_CODE_BREAK' = ASCII_CODE_BREAK

defc `USE_TIMEX_HIRES' = USE_TIMEX_HIRES
defc `TEXT_WINDOW_HEIGHT' = TEXT_WINDOW_HEIGHT

defc `USE_GFX' = USE_GFX
defc `MAX_IMAGE_HEIGHT' = MAX_IMAGE_HEIGHT

ENDIF
divert(-1)
')

################################################################################
# FILE: zconfig.m4
################################################################################

ifelse(TARGET, 3,
`
divert
`define'(`GAME_DIR', GAME_DIR)

`define'(`KEY_REPEAT_START', KEY_REPEAT_START)

`define'(`KEY_REPEAT_RATE', KEY_REPEAT_RATE)

`define'(`USE_TIMEX_HIRES', USE_TIMEX_HIRES)

`define'(`TEXT_WINDOW_HEIGHT', TEXT_WINDOW_HEIGHT)

`define'(`TEXT_FONT', TEXT_FONT)
divert(-1)
')

################################################################################
# FILE: zpragma.inc
################################################################################

ifelse(TARGET, 4,
`
divert
ifelse(USE_TIMEX_HIRES, 0,
`
// Program org for ULA mode.
`#pragma' output CRT_ORG_CODE = 0x6100
',
`
// Program org for Timex hi-res mode.
`#pragma' output CRT_ORG_CODE = 0x7800
')dnl

// Set stack location to top of memory.
`#pragma' output REGISTER_SP = 0

// No malloc heap.
`#pragma' output CLIB_MALLOC_HEAP_SIZE = 0

// No stdio heap.
`#pragma' output CLIB_STDIO_HEAP_SIZE = 0

// Space to register atexit functions.
`#pragma' output CLIB_EXIT_STACK_SIZE = 1

// Reduce the size of printf to what is needed.
`#pragma' printf = "%s %d %u %X %lu %lX"

// Include crt_driver_instantiation.asm.m4 in driver instantiation section.
`#pragma' output CRT_INCLUDE_DRIVER_INSTANTIATION = 1

// Set origin address to 0 for bank 3 (vt_sound binary and music module).
`#pragma' output CRT_ORG_BANK_3 = 0

// Set origin address to 0x1000 for page 28 (status font in Timex hi-res mode).
`#pragma' output CRT_ORG_BANK_14_L = 0x1000
divert(-1)
')

################################################################################
# FILE: zproject.lst
################################################################################

ifelse(TARGET, 5,
`
divert
ifdef(`FAST_COMPILE', ifelse(USE_TIMEX_HIRES, 0, `src/emu-s.o', `src/emu-t.o'), `src/emu.c')
src/main.c
src/sprite.c
src/interrupt.asm
src/in_key_translation_table.asm
src/zx_01_input_kbd_inkey_custom.asm
src/console_01_output_char_stdio_msg_ictl.asm
src/scroll_prompt.asm
src/status_bar.c
src/text_color.asm
src/layer2_fzx.asm
ifelse(TEXT_FONT, `_ff_pd_QLStyle',
`
src/_ff_pd_QLStyle.asm
')dnl
ifelse(USE_TIMEX_HIRES, 0,
`
src/zx_01_output_fzx_custom.asm
',
`
src/tshr_01_output_fzx_custom.asm
src/status_font.asm
')dnl
ifelse(USE_GFX, 0,,
`
src/layer2.c
src/gfx_util.asm
')dnl
ifelse(USE_SOUND, 0,,
`
src/sound.c
src/music_module.asm.m4
vt_sound/src/vt_sound.asm
vt_sound/src/PT3PROM.asm
')dnl
ifelse(USE_MOUSE, 0,,
`
src/asm_in_mouse_kempston.asm
src/asm_in_mouse_kempston_wheel.asm
src/mouse.c
')dnl
ifelse(USE_SCRIPT, 0,,
`
src/esxdos_util.c
src/stack.asm
')dnl
ifelse(USE_IMAGE_SLIDESHOW, 0,,
`
src/image_slideshow.c
')dnl
divert(-1)
')

divert(0)dnl
