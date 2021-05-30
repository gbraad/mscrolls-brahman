/*******************************************************************************
 *
 * Magnetic - Magnetic Scrolls Interpreter
 *
 * Written by Niclas Karlsson <nkarlsso@abo.fi>,
 *            David Kinder <davidk@davidkinder.co.uk>,
 *            Stefan Meier <Stefan.Meier@if-legends.org> and
 *            Paul David Doherty <pdd@if-legends.org>
 *
 * Ported to Sinclair ZX Spectrum Next by Stefan Bylund <stefan.bylund99@gmail.com>
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
 *
 * Porting of the Magnetic Scrolls interpreter to Sinclair ZX Spectrum Next.
 *
 * Ideally, this file should be left unmodified when porting to a new target.
 * However, the original code in this file assumes a flat memory model and the
 * loaded game story file being kept in main memory. The ZX Spectrum Next has
 * only 64 KB addressable memory and a banked memory model with 768 KB physical
 * RAM available as 8 KB pages that can be paged-in to the 64 KB address space.
 * The approach of this porting is to always have the interpreter in main memory
 * and loading the game story file into a contiguous set of 8 KB RAM pages that
 * are paged-in as needed to the bottom 16 KB of the address space.
 *
 * The interpreter itself has been stripped to fit in only 36 KB so that it can
 * always be kept in main memory. Support for loading and displaying Amiga-style
 * graphics from a game graphics file has been removed. The individual graphics
 * files are instead manually converted to Spectrum Next format and loaded on
 * demand. Support for loading and playing MIDI sound from an optional game
 * sound file has been removed. Support for loading and showing hints from an
 * optional game hints file has been removed. Support for optionally saving
 * script files has been removed. Undo support has been removed.
 *
 * The game story file is loaded into a contiguous set of 8 KB RAM pages that
 * are paged-in as needed to the bottom 16 KB of the address space where the
 * Spectrum ROM is normally located. The original code provides an effective()
 * function for converting a virtual pointer to an effective pointer, which fits
 * nicely when accessing the code section of the game story file in a paged
 * manner. However, there were no analogous functions for accessing the string
 * and dictionary sections of the game story file. Such effective functions have
 * been added and all references to those sections have been updated to use
 * these functions.
 *
 * For some of the instructions executed by the interpreter there are multiple
 * calls made to the effective functions and a pointer from a preceding effective
 * call is used after another effective call has been made. In those places, the
 * data pointed to by the first pointer is saved in a temporary buffer since the
 * referred page may we swapped out by the succeeding effective calls.
 * Fortunately, only a 4-byte longword or a 16-character string needs to be
 * saved in a temporary buffer in this manner.
 *
 * To improve performance, some parts of the interpreter have been rewritten
 * in Z80 assembler and some general functions have been split into several
 * specialized functions.
 *
 * Files are accessed using ESXDOS whose API is different but similar to the
 * standard POSIX file I/O API. When using the ESXDOS API, the Spectrum ROM must
 * be temporarily paged in to the bottom 16 KB RAM. The memory area at 16K - 24K
 * is temporarily used for paging in the data that should be read/written
 * from/to a file.
 *
 * The Magnetic interpreter for Spectrum Next supports two game story file
 * formats: the .mag and .prg file formats. The .mag file format was designed
 * for the reverse-engineered Magnetic interpreter. The .prg/.prog file format
 * was used by the original Magnetic Scrolls interpreter and has been
 * reintroduced by Strand Games in their remastered versions of the games.
 *
 * The .prg file format does not contain the string sections (as in the .mag
 * file format). The strings are instead placed in a separate .txt file.
 * For Spectrum Next, there is also an .idx file containing an index for the
 * .txt file for quickly looking up the offset of a string given its index.
 * The .txt and .idx files are generated using the convert_text tool from the
 * .text file used in the desktop/mobile versions of the remastered games.
 * The .text file contains additional markup that is removed by this tool.
 *
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <z80.h>
#include <intrinsic.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "zconfig.h"
#include "defs.h"
#include "emu_asm.h"
#include "hunks.h"
#include "ide_friendly.h"

// FIXME: Lower MAX_PRG_CODE_SIZE from 128 KB to 88 KB (eleven 8 KB pages).

#define BASE_PAGE 30
#define STRING_INDEX_BASE_PAGE 2
#define MAX_MAG_CODE_SIZE 0x10000
#define MAX_PRG_CODE_SIZE 0x1F400
#define MAX_STRING_SIZE 0xFF00
#define DECODE_TABLE_CHARS_SIZE 256
#define DICT_INPUT_BUFFER_SIZE 256

/*
 * Optimization:
 * Macros for inlining of reading/writing of 16/32-bit values with endian
 * conversion. These macros replace the frequently used original functions
 * having the following prototypes:
 *
 * type32 read_l(type8 *ptr);
 * type16 read_w(type8 *ptr);
 * void write_l(type8 *ptr, type32 val);
 * void write_w(type8 *ptr, type16 val);
 */
#define read_l(ptr) (intrinsic_swap_endian_32(*(type32 *)(ptr)))
#define read_w(ptr) (intrinsic_swap_endian_16(*(type16 *)(ptr)))
#define write_l(ptr, val) (*(type32 *)(ptr) = intrinsic_swap_endian_32(val))
#define write_w(ptr, val) (*(type16 *)(ptr) = intrinsic_swap_endian_16(val))

#if USE_PROG_FORMAT

#define MAX_SP_STACK_SIZE 5

typedef struct sp_continuation
{
    type8 in_stopron;
    type8 *stopron_continuation;
    type8 stopron_page;
} sp_continuation_t;

static sp_continuation_t sp_stack[MAX_SP_STACK_SIZE];
static type8 sp_stack_size = 0;

#endif

extern void add_startup_commands(void);

type32 dreg[8];
type32 areg[8];
static type32 i_count;
static type32 mem_size;
static type32 string_size;
static type32 rseed;
type32 pc;
type32 arg1i;
static type16 properties;
static type16 fl_sub;
static type16 fl_tab;
static type16 fl_size;
static type16 fp_tab;
static type16 fp_size;
type8 zflag;
type8 nflag;
type8 cflag;
type8 vflag;
type8 byte1;
type8 byte2;
type8 regnr;
type8 admode;
type8 opsize;
type8 *arg1;
type8 *arg2;
type8 arg2_buf[4];
type8 is_reversible;
static type8 running;
type8 tmparg[4];
static type8 lastchar;
type8 version;
static type8 sd;
type8 quick_flag;
static type8 str_buf[16];
static type8 game_file[16] = {0};

#if USE_PROG_FORMAT
static type32 remaster_symbol_address;
static type32 outmode_symbol_address;
static type32 stopron_symbol_address;
static type32 ptn_symbol_address;
#else
static type32 decode_table_offset;
static type16 decode_table_limit;
#endif

/*
 * Note: When using the Timex hi-res mode, we use the free 2 KB RAM between
 * the two Timex hi-res screen buffers (0x5800 - 0x5FFF) for storing the
 * decode_table_chars and dict_input_buffer buffers after the buffers declared
 * in main.c (of size 1.5 KB).
 */

#if USE_PROG_FORMAT

#if USE_TIMEX_HIRES
#define BUFFER_MEMORY_START (0x5800 + 1536)
static type8 *dict_input_buffer = (type8 *) BUFFER_MEMORY_START;
#else
// Dictionary input buffer of 256 bytes used in dict_lookup() optimization.
static type8 dict_input_buffer[DICT_INPUT_BUFFER_SIZE];
#endif

#else

#if USE_TIMEX_HIRES
#define BUFFER_MEMORY_START (0x5800 + 1536)
static type8 *decode_table_chars = (type8 *) BUFFER_MEMORY_START;
static type8 *dict_input_buffer = (type8 *) (BUFFER_MEMORY_START + DECODE_TABLE_CHARS_SIZE);
#else
// Optimization for caching the 256 bytes long decode_table_chars array.
static type8 decode_table_chars[DECODE_TABLE_CHARS_SIZE];
// Dictionary input buffer of 256 bytes used in dict_lookup() optimization.
static type8 dict_input_buffer[DICT_INPUT_BUFFER_SIZE];
#endif

#endif

/*******************************************************************************
 * RAM pages for storing the loaded sections of the game story file:
 *
 * [r/w] code
 * [r]   string
 * [r]   string2 (.mag)
 * [r]   string3 (.mag, optional)
 * [r]   decode_table (.mag, inside string sections)
 * [r/w] dictionary (optional, inside code section in game versions 0 and 1)
 * [r]   string_index (.prg)
 ******************************************************************************/

type8 current_page;
type8 code_base_page;
static type8 code_num_pages;
type8 string_base_page;
static type8 string_num_pages;
#if USE_PROG_FORMAT
static type8 string_index_base_page;
static type8 string_index_num_pages;
#else
type8 string2_base_page;
static type8 string2_num_pages;
type8 string3_base_page;
static type8 string3_num_pages;
#endif
type8 dict_base_page;
static type8 dict_num_pages;

#ifdef LOGEMU
static type8 dbg_log;

static char dbg_buf[128];

static type32 frame_counter = 0;

void out(char *format, ...)
{
    int size;
    va_list args;

    va_start(args, format);
    size = vsnprintf(dbg_buf, sizeof(dbg_buf), format, args);
    va_end(args);

    page_in_rom();
    esx_f_write(dbg_log, dbg_buf, size);
    page_in_game();
}

IM2_DEFINE_ISR_8080(increase_frame_counter)
{
    frame_counter++;
}

static void install_isr(void)
{
    intrinsic_di();
    z80_bpoke(0xFDFD, 0xC3); // jp
    z80_wpoke(0xFDFE, (type16) increase_frame_counter);
    intrinsic_ei();
}
#endif

static type8 calc_num_pages(type32 size) __z88dk_fastcall
{
    return addr_to_page(size) + (type8) ((size % 0x2000 == 0) ? 0 : 1); // page = size / 0x2000
}

static void init_state(void)
{
    memset(dreg, 0, sizeof(dreg));
    memset(areg, 0, sizeof(areg));

    pc = 0;
    zflag = 0;
    nflag = 0;
    cflag = 0;
    vflag = 0;
    quick_flag = 0;
    i_count = 0;
    lastchar = 0;
    current_page = 255;

    // Init stack pointer.
#if USE_PROG_FORMAT
    write_areg_l(7, MAX_PRG_CODE_SIZE);
#else
    write_areg_l(7, MAX_MAG_CODE_SIZE - 2);
#endif
}

static type8 load_section(type8 fh, type32 size, type8 base_page)
{
    type8 num_pages;
    type8 page;
    type16 rest;

    /*
     * The given section is loaded via MMU slot 2 (the ULA screen, page 10)
     * into a contiguous set of one or more 8 KB RAM pages.
     */

    errno = 0;
    num_pages = addr_to_page(size); // num_pages = size / 0x2000

    for (page = base_page; num_pages--; ++page)
    {
        ZXN_WRITE_MMU2(page);
        esx_f_read(fh, (type8 *) 0x4000, 0x2000);
        if (errno)
        {
            goto end;
        }
    }

    // Load the last chunk (or the first and only chunk)
    // if its size is less than 8 KB.

    rest = (type16) (size % 0x2000);

    if (rest > 0)
    {
        ZXN_WRITE_MMU2(page);
        esx_f_read(fh, (type8 *) 0x4000, rest);
    }

    // Restore default page 10 in MMU slot 2.
end:
    ZXN_WRITE_MMU2(10);
    return (errno == 0);
}

#if !USE_PROG_FORMAT

static type8 *get_decode_table(void)
{
    if (decode_table_offset >= string_size)
    {
        return effective_string2(decode_table_offset - string_size);
    }
    else if (decode_table_offset >= MAX_STRING_SIZE)
    {
        return effective_string3(decode_table_offset - MAX_STRING_SIZE);
    }
    else
    {
        return effective_string(decode_table_offset);
    }
}

static void init_decode_table_chars(void)
{
    type8 *ptr = get_decode_table();
    memcpy(decode_table_chars, ptr, DECODE_TABLE_CHARS_SIZE);
    decode_table_limit = read_w(&ptr[0x100]);
}

#endif

static void save_str(type8 **str) __z88dk_fastcall
{
    if (*str != NULL)
    {
        strncpy(str_buf, *str, sizeof(str_buf) - 1);
        str_buf[sizeof(str_buf) - 1] = '\0';
        *str = str_buf;
    }
}

/* Standard rand - for equal cross-platform behaviour */

void ms_seed(type32 seed) __z88dk_fastcall
{
    rseed = seed;
}

#if USE_PROG_FORMAT

static type32 rand_emu(void)
{
    rseed = 1103515245L * rseed + 12345L;
    return (rseed >> 16);
}

#else

static type32 rand_emu(void)
{
    rseed = 1103515245L * rseed + 12345L;
    return rseed & 0x7fffffffL;
}

#endif

void ms_freemem(void)
{
#ifdef LOGEMU
    page_in_rom();
    esx_f_close(dbg_log);
#endif
}

void ms_stop(void)
{
    running = 0;
}

#if !USE_PROG_FORMAT

/* zero all registers and flags and load the game */

type8 ms_init(type8 *name) __z88dk_fastcall
{
    type8 fh;
    static type8 header[42];
    type8 string_size_overflow;
    type32 code_size;
    type32 string2_size;
    type32 string3_size;
    type32 dict_size;

    if (name != NULL)
    {
        // Clean start: save game filename for restart and init random seed.
        strncpy(game_file, name, sizeof(game_file) - 1);
        game_file[sizeof(game_file) - 1] = '\0';
        ms_seed(seed_r());
    }
    else if (strlen(game_file) > 0)
    {
        // Restart: free resources and display start location image immediately.
        ms_freemem();
        ms_showpic(GAME_START_IMAGE, 2);
    }
    else
    {
        // Error: no game filename given and not a restart.
        printf("Error no game file specified.\n");
        return 0;
    }

    // Make sure the Spectrum ROM is paged in before performing all the ESXDOS
    // operations below.
    page_in_rom();

#ifdef LOGEMU
    errno = 0;
    dbg_log = esx_f_open(LOG_FILE, ESX_MODE_OPEN_CREAT_TRUNC | ESX_MODE_W);
    if (errno)
    {
        printf("Warning: error opening log file.\n");
    }
#endif

    ms_stop();
    init_state();

    errno = 0;
    fh = esx_f_open(game_file, ESX_MODE_OPEN_EXIST | ESX_MODE_R);
    if (errno)
    {
        printf("Error opening game file %s.\n", game_file);
        return 0;
    }

    if (esx_f_read(fh, header, sizeof(header)) != sizeof(header))
    {
        goto end;
    }

    if ((read_l(header) != 0x4D615363) || (read_l(header + 8) != 42))
    {
        goto end;
    }

    version = header[13];
    code_size = read_l(header + 14);
    string_size = read_l(header + 18);
    string2_size = read_l(header + 22);
    dict_size = read_l(header + 26);
    decode_table_offset = read_l(header + 30);

    mem_size = ((version < 4) && (code_size < MAX_MAG_CODE_SIZE)) ? MAX_MAG_CODE_SIZE : code_size;
    string_size_overflow = (string_size > MAX_STRING_SIZE);
    string3_size = string_size_overflow ? (string_size - MAX_STRING_SIZE) : 0;
    sd = (dict_size != 0); // sd != 0 if separate dictionary.

    code_base_page = BASE_PAGE;
    code_num_pages = calc_num_pages(mem_size);

    string_base_page = code_base_page + code_num_pages;
    string_num_pages = calc_num_pages(string_size_overflow ? MAX_STRING_SIZE : string_size);

    string2_base_page = string_base_page + string_num_pages;
    string2_num_pages = calc_num_pages(string2_size);

    string3_base_page = string2_base_page + string2_num_pages;
    string3_num_pages = calc_num_pages(string3_size);

    dict_base_page = string3_base_page + string3_num_pages;
    dict_num_pages = calc_num_pages(dict_size);

    if (!load_section(fh, code_size, code_base_page))
    {
        goto end;
    }

    if (string_size_overflow)
    {
        if (!load_section(fh, MAX_STRING_SIZE, string_base_page))
        {
            goto end;
        }

        if (!load_section(fh, string3_size, string3_base_page))
        {
            goto end;
        }
    }
    else
    {
        if (!load_section(fh, string_size, string_base_page))
        {
            goto end;
        }
    }

    if (!load_section(fh, string2_size, string2_base_page))
    {
        goto end;
    }

    if (sd)
    {
        if (!load_section(fh, dict_size, dict_base_page))
        {
            goto end;
        }
    }

    init_decode_table_chars();
    // Note: init_decode_table_chars() will page out the Spectrum ROM
    // but we must page it in before calling esx_f_close() below.
    page_in_rom();

#ifdef LOGEMU
    install_isr();
#endif

    running = 1;
end:
    esx_f_close(fh);
    return running;
}

#else

static type32 get_long(type8 fh) __z88dk_fastcall
{
    type16 num_read;
    type32 value;

    errno = 0;
    num_read = esx_f_read(fh, &value, 4);
    value = read_l(&value);
    return (errno || (num_read != 4)) ? EOF : value;
}

/*
 * Format of a PROG file (which is an AmigaDOS load file).
 * All field sizes are in units of longwords and all data
 * is in big endian byte order.
 *
 * --------------------------------------------------------
 *
 * <hunk_header>
 * 0
 * num_hunks
 * 0
 * 0
 * hunk_sizes[num_hunks] (in longwords)
 *
 * --------------------------------------------------------
 *
 * <hunk_code>
 * code_size (in longwords)
 * code
 *
 * <hunk_reloc32>
 * num_relocs (0 = end of hunk)
 * hunk_number
 * relocs (num_relocs longwords)
 *
 * <hunk_symbol>
 * symbol_name_length (in longwords, 0 = end of hunk)
 * symbol_name (symbol_name_length longwords of characters)
 * symbol_offset
 *
 * <hunk_end>
 *
 * --------------------------------------------------------
 *
 * <hunk_data> (optional)
 * data_size (in longwords)
 * data
 *
 * <hunk_reloc32>
 * num_relocs (0 = end of hunk)
 * hunk_number
 * relocs (num_relocs longwords)
 *
 * <hunk_symbol>
 * symbol_name_length (in longwords, 0 = end of hunk)
 * symbol_name (symbol_name_length longwords of characters)
 * symbol_offset
 *
 * <hunk_end>
 *
 * --------------------------------------------------------
 */

static type8 load_prog_header(type8 fh, type32 *code_size, type32 *dict_size)
{
    type32 num_hunks;
    type32 hunk_sizes[3] = {0};

    if (get_long(fh) != hunk_header)
    {
        return 0;
    }

    get_long(fh); // skip

    num_hunks = get_long(fh);
    if ((num_hunks == 0) || (num_hunks > 3))
    {
        return 0;
    }

    get_long(fh); // skip
    get_long(fh); // skip

    for (type8 i = 0; i < num_hunks; i++)
    {
        hunk_sizes[i] = get_long(fh);
    }

    *code_size = hunk_sizes[0] << 2;
    *dict_size = hunk_sizes[1] << 2;

    return (type8) num_hunks;
}

static void handle_prog_symbol(type8 *name, type32 value)
{
    if (!strcmp(name, "REMASTER"))
    {
        remaster_symbol_address = value;
    }
    else if (!strcmp(name, "OUTMODE"))
    {
        outmode_symbol_address = value;
    }
    else if (!strcmp(name, "STOPRON"))
    {
        stopron_symbol_address = value;
    }
    else if (!strcmp(name, "P.TN"))
    {
        ptn_symbol_address = value;
    }
}

static type8 load_prog_symbols(type8 fh) __z88dk_fastcall
{
    type8 name[32];

    errno = 0;

    for (;;)
    {
        type32 name_size;
        type32 value;

        name_size = get_long(fh) << 2;
        if (name_size == 0)
        {
            // End of symbol hunk
            break;
        }
        else if (name_size > 32)
        {
            // Bail out, too long symbol name
            return 0;
        }

        esx_f_read(fh, name, name_size);
        if (errno)
        {
            return 0;
        }

        name[name_size] = 0;
        value = get_long(fh);

        if (!isdigit(name[0]))
        {
            handle_prog_symbol(name, value);
        }
    }

    return 1;
}

static type8 load_prog_file(type8 fh, type8 num_sections, type32 code_size, type32 dict_size)
{
    type8 code_loaded = 0;
    type8 dict_loaded = 0;
    type8 symbols_loaded = 0;

    while (1)
    {
        type32 hunk_type = get_long(fh);

        if (hunk_type == hunk_code)
        {
            type32 size = get_long(fh) << 2;
            if (size != code_size)
            {
                // Inconsistent code size.
                return 0;
            }
            if (!load_section(fh, code_size, code_base_page))
            {
                return 0;
            }
            code_loaded = 1;
        }
        else if (hunk_type == hunk_data)
        {
            type32 size = get_long(fh) << 2;
            if (size != dict_size)
            {
                // Inconsistent data/dictionary size.
                return 0;
            }
            if (!load_section(fh, dict_size, dict_base_page))
            {
                return 0;
            }
            dict_loaded = 1;
        }
        else if (hunk_type == hunk_symbol)
        {
            if (!load_prog_symbols(fh))
            {
                return 0;
            }
            symbols_loaded = 1;
        }
        else if (hunk_type == hunk_reloc32)
        {
            // Skip relocs
            for (;;)
            {
                type32 num_relocs = get_long(fh);
                if (num_relocs == 0)
                {
                    break;
                }
                get_long(fh);
                while (num_relocs--)
                {
                    get_long(fh);
                }
            }
        }
        else if (hunk_type == hunk_end)
        {
            if (--num_sections == 0)
            {
                break;
            }
        }
        else
        {
            // Unknown hunk type.
            return 0;
        }
    }

    return code_loaded && symbols_loaded && (!sd || dict_loaded);
}

static type32 load_data_file(type8 *suffix, type8 base_page)
{
    type8 filename[16];
    type8 *old_suffix;
    type8 fh;
    struct esx_stat filestat;
    type32 size = 0;

    // Construct filename from game filename.
    strcpy(filename, GAME_FILE);
    old_suffix = strstr(filename, ".prg");
    if (old_suffix == NULL)
    {
        return 0;
    }
    strcpy(old_suffix, suffix);

    // Open file.
    errno = 0;
    fh = esx_f_open(filename, ESX_MODE_OPEN_EXIST | ESX_MODE_R);
    if (errno)
    {
        return 0;
    }

    esx_f_fstat(fh, &filestat);

    // Load file.
    if (!load_section(fh, filestat.size, base_page))
    {
        goto end;
    }

    size = filestat.size;

end:
    esx_f_close(fh);
    return size;
}

/* zero all registers and flags and load the game */

type8 ms_init(type8 *name) __z88dk_fastcall
{
    type8 fh;
    type8 num_sections;
    type32 code_size;
    type32 dict_size;
    type32 string_index_size;

    if (name != NULL)
    {
        // Clean start: save game filename for restart and init random seed.
        strncpy(game_file, name, sizeof(game_file) - 1);
        game_file[sizeof(game_file) - 1] = '\0';
        ms_seed(seed_r());
    }
    else if (strlen(game_file) > 0)
    {
        // Restart: free resources and display start location image immediately.
        ms_freemem();
        ms_showpic(GAME_START_IMAGE, 2);
    }
    else
    {
        // Error: no game filename given and not a restart.
        printf("Error no game file specified.\n");
        return 0;
    }

    // Make sure the Spectrum ROM is paged in before performing all the ESXDOS
    // operations below.
    page_in_rom();

#ifdef LOGEMU
    errno = 0;
    dbg_log = esx_f_open(LOG_FILE, ESX_MODE_OPEN_CREAT_TRUNC | ESX_MODE_W);
    if (errno)
    {
        printf("Warning: error opening log file.\n");
    }
#endif

    ms_stop();
    init_state();

    errno = 0;
    fh = esx_f_open(game_file, ESX_MODE_OPEN_EXIST | ESX_MODE_R);
    if (errno)
    {
        printf("Error opening game file %s.\n", game_file);
        return 0;
    }

    num_sections = load_prog_header(fh, &code_size, &dict_size);
    if (num_sections == 0)
    {
        printf("Invalid game file.\n");
        goto end;
    }

    version = 2; // Remastered game
    mem_size = (code_size < MAX_PRG_CODE_SIZE) ? MAX_PRG_CODE_SIZE : code_size;
    sd = (dict_size != 0); // sd != 0 if separate dictionary.

    code_base_page = BASE_PAGE;
    code_num_pages = calc_num_pages(mem_size);

    dict_base_page = code_base_page + code_num_pages;
    dict_num_pages = calc_num_pages(dict_size);

    if (!load_prog_file(fh, num_sections, code_size, dict_size))
    {
        printf("Error loading game file.\n");
        goto end;
    }

    string_base_page = dict_base_page + dict_num_pages;
    string_size = load_data_file(".txt", string_base_page);
    string_num_pages = calc_num_pages(string_size);
    if (string_size == 0)
    {
        printf("Error loading string file.\n");
        goto end;
    }

    string_index_base_page = STRING_INDEX_BASE_PAGE;
    string_index_size = load_data_file(".idx", string_index_base_page);
    string_index_num_pages = calc_num_pages(string_index_size);
    if (string_index_size == 0)
    {
        printf("Error loading string index file.\n");
        goto end;
    }

#ifdef LOGEMU
    install_isr();
#endif

    running = 1;
end:
    esx_f_close(fh);
    return running;
}

void ms_config(void)
{
    // Make sure the game starts in remastered mode.
    effective(remaster_symbol_address)[0] = 1;
}

type8 toggle_game_mode(void)
{
    // Toggle game mode between remastered and classic modes.
    type8 *mode_ptr = effective(remaster_symbol_address);
    type8 new_mode = !mode_ptr[0];
    mode_ptr[0] = new_mode;
    return new_mode;
}

#endif

#ifdef LOGEMU
static void log_status(void)
{
    type8 j;

    out("\nD0:");
    for (j = 0; j < 8; j++)
        out(" %8.8lX", read_dreg_l(j));
    out("\nA0:");
    for (j = 0; j < 8; j++)
        out(" %8.8lX", read_areg_l(j));
    out("\nPC=%5.5lX page=%3.3u ZCNV=%d%d%d%d - %lu instructions\n\n",
        pc, current_page, zflag & 1, cflag & 1, nflag & 1, vflag & 1, i_count);
}
#endif

void ms_status(void)
{
    type8 j;

    fprintf(stdout, "D0:");
    for (j = 0; j < 8; j++)
        fprintf(stdout, " %8.8lX", read_dreg_l(j));
    fprintf(stdout, "\nA0:");
    for (j = 0; j < 8; j++)
        fprintf(stdout, " %8.8lX", read_areg_l(j));
    fprintf(stdout, "\nPC=%5.5lX ZCNV=%d%d%d%d - %lu instructions\n",
        pc, zflag & 1, cflag & 1, nflag & 1, vflag & 1, i_count);
}

static void char_out(type8 c) __z88dk_fastcall
{
    static type8 big = 0, period = 0, pipe = 0;

    if (c == 0xff)
    {
        big = 1;
        return;
    }
    c &= 0x7f;
    if (read_dreg_b(3))
    {
        if (c == 0x5f || c == 0x40)
            c = 0x20;
        else if ((c >= 'a') && (c <= 'z'))
            c &= 0xdf;
        if (version < 4)
            ms_statuschar(c);
        return;
    }
    if (c == 0x5e)
        c = 0x0a;
    else if (c == 0x40)
    {
        if (read_dreg_b(2))
            return;
        else
            c = 0x73;
    }
    if (version < 3 && c == 0x7e)
    {
        lastchar = 0x7e;
        c = 0x0a;
    }
    if (((c > 0x40) && (c < 0x5b)) || ((c > 0x60) && (c < 0x7b)))
    {
        if (big)
        {
            c &= 0xdf;
            big = 0;
        }
        if (period)
            char_out(0x20);
    }
    period = 0;
#if !USE_MAGNETIC_WINDOWS
    {
        if ((c == 0x2e) || (c == 0x3f) || (c == 0x21) || (c == 0x0a))
            big = 1;
        else if (c == 0x22)
            big = 0;
    }
#else
    {
        if ((c == 0x20) && (lastchar == 0x0a))
            return;
        if ((c == 0x2e) || (c == 0x3f) || (c == 0x21) || (c == 0x0a))
            big = 1;
        else if (c == 0x22)
            big = 0;
    }
#endif
    if (((c == 0x20) || (c == 0x0a)) && (c == lastchar))
        return;
    if (version < 3)
    {
        if (pipe)
        {
            pipe = 0;
            return;
        }
        if (c == 0x7c)
        {
            pipe = 1;
            return;
        }
    }
    else
    {
        if (c == 0x7e)
        {
            c = 0x0a;
            if (lastchar != 0x0a)
                char_out(0x0a);
        }
    }
    lastchar = c;
    if (c == 0x5f)
        c = 0x20;
    if ((c == 0x2e) || (c == 0x2c) || (c == 0x3b) || (c == 0x3a) || (c == 0x21) || (c == 0x3f))
        period = 1;
    ms_putchar(c);
}

/* [30e4] in Jinxter, ~540 lines of 6510 spaghetti-code */
/* The mother of all bugs, but hey - no gotos used :-) */

static void dict_lookup(void)
{
    type16 dtab, doff, output, output_bak, bank, word, output2;
    type16 tmp16, i, obj_adj, adjlist, adjlist_bak;
    type8 c, c2, c3, flag, matchlen, longest, flag2;
    type8 restart = 0, accept = 0;

    type8 *doff_ptr;
    type16 doff_start;
    type16 a5_reg;
    type8 *a2_ptr;
    type8 *adjlist_ptr;
    type8 *effective_ptr;

/*
   dtab=A5.W                    ;dict_table offset <L22>
   output=output_bak=A2.W       ;output <L24>
   A5.W=A6.W                    ;input word
   doff=A3.W                    ;lookup offset (doff) <L1C>
   adjlist=A0.W ;adjlist <L1E>
 */

    dtab = read_areg_w(5); /* used by version>0 */
    output = read_areg_w(2);
    write_areg_w(5, read_areg_w(6));
    doff = read_areg_w(3);
    adjlist = read_areg_w(0);

    bank = (type16) read_dreg_b(6); /* l2d */
    flag = 0; /* l2c */
    word = 0; /* l26 */
    matchlen = 0; /* l2e */
    longest = 0; /* 30e2 */
    write_dreg_w(0, 0); /* apostroph */

    /*
     * The loop below is a performance bottleneck when using a banked memory
     * model. Calls to effective()/effective_dict() are being made for three
     * different memory areas: doff, A5/A6 (input) and A2 (output).
     * The doff memory area is accessed very frequently, the input memory
     * area quite frequently and the output memory area infrequently.
     *
     * The loop has been changed with the following objective: the doff memory
     * area should always be paged in, the input memory area is initially
     * copied to a 256-bytes buffer to avoid using effective() calls and after
     * accessing the output memory area with effective() calls, the doff memory
     * area is explicitly paged in again.
     *
     * This optimization assumes that doff is not incremented more than
     * (16K - address_in_bottom_8k(doff_start)) times and that a 256-bytes
     * buffer is enough to hold the input.
     */

    a5_reg = read_areg_w(5);
    memcpy(dict_input_buffer, effective(a5_reg), DICT_INPUT_BUFFER_SIZE);
    // From now on and in the loop below, we keep the doff memory area paged in.
    doff_ptr = sd ? effective_dict(doff) : effective(doff);
    doff_start = doff;

    while ((c = doff_ptr[doff - doff_start]) != 0x81)
    {
        if (c >= 0x80)
        {
            if (c == 0x82)
            {
                flag = matchlen = 0;
                word = 0;
                write_areg_w(6, a5_reg);
                bank++;
                doff++;
                continue;
            }
            c3 = c;
            c &= 0x5f;
            c2 = dict_input_buffer[read_areg_w(6) - a5_reg] & 0x5f;
            if (c2 == c)
            {
                inc_areg_w(6);
                c = dict_input_buffer[read_areg_w(6) - a5_reg];
                if ((!c) || (c == 0x20) || (c == 0x27) || (!version && (matchlen > 6)))
                {
                    if (c == 0x27)
                    {
                        inc_areg_w(6);
                        write_dreg_w(0, 0x200 + dict_input_buffer[read_areg_w(6) - a5_reg]);
                    }
                    if ((version < 4) || (c3 != 0xa0))
                        accept = 1;
                }
                else
                    restart = 1;
            }
            else if (!version && matchlen > 6 && !c2)
                accept = 1;
            else
                restart = 1;
        }
        else
        {
            c &= 0x5f;
            c2 = dict_input_buffer[read_areg_w(6) - a5_reg] & 0x5f;
            if ((c2 == c && c) || (version && !c2 && (c == 0x5f)))
            {
                if (version && !c2 && (c == 0x5f))
                    flag = 0x80;
                matchlen++;
                inc_areg_w(6);
                doff++;
            }
            else if (!version && matchlen > 6 && !c2)
                accept = 1;
            else
                restart = 1;
        }
        if (accept)
        {
            a2_ptr = effective(read_areg_w(2));
            a2_ptr[0] = (version) ? flag : 0;
            a2_ptr[1] = (type8) bank;
            write_w(a2_ptr + 2, word);
            inc_var_areg_w(2, 4);
            if (matchlen >= longest)
                longest = matchlen;
            restart = 1;
            accept = 0;
            // Page in doff memory area again.
            if (sd)
                effective_dict(doff_start);
            else
                effective(doff_start);
        }
        if (restart)
        {
            write_areg_w(6, a5_reg);
            flag = matchlen = 0;
            word++;
            while (doff_ptr[(doff++) - doff_start] < 0x80);
            restart = 0;
        }
    }
    write_w(effective(read_areg_w(2)), 0xffff);

    if (version)
    {
        // Optimization: Minimize number of effective(output) calls.
        /* version > 0 */
        effective_ptr = effective(output);
        while ((c = effective_ptr[1]) != 0xff)
        {
            if (c == 0x0b)
            {
                tmp16 = read_w(effective_ptr + 2) * 2;
                if (sd)
                    tmp16 = read_w(effective_dict(dtab + tmp16));
                else
                    tmp16 = read_w(effective(dtab + tmp16));
                effective(output); // Page in output memory area again.
                effective_ptr[1] = tmp16 & 0x1f;
                write_w(effective_ptr + 2, (type16) (tmp16 >> 5));
            }
            effective_ptr += 4;
        }
    }

/* l22 = output2,     l1e = adjlist, l20 = obj_adj, l26 = word, l2f = c2 */
/* l1c = adjlist_bak, 333C = i,      l2d = bank,    l2c = flag, l30e3 = flag2 */

    write_dreg_w(1, 0); /* D1.W=0 */
    flag2 = 0;
    output_bak = output;
    output2 = output;
    while ((bank = effective(output2)[1]) != 0xff)
    {
        obj_adj = read_areg_w(1); /* A1.W - obj_adj, ie. adjs for this word */
        write_dreg_b(1, 0); /* D1.B=0 */
        flag = effective(output2)[0]; /* flag */
        word = read_w(effective(output2 + 2)); /* wordnumber */
        output2 += 4; /* next match */
        if ((read_w(effective(obj_adj))) && (bank == 6))
        {
            // Optimization: Minimize number of effective(adjlist) calls.
            /* Any adjectives? */
            if ((i = word) != 0)
            {
                adjlist_ptr = effective(adjlist);
                adjlist_bak = adjlist;
                /* Find list of valid adjs */
                do
                {
                    while (adjlist_ptr[(adjlist++) - adjlist_bak]);
                }
                while (--i > 0);
            }
            adjlist_bak = adjlist;
            do
            {
                adjlist = adjlist_bak;
                c2 = effective(obj_adj)[1]; /* given adjective */
                if ((tmp16 = read_w(effective(obj_adj))) != 0)
                {
                    obj_adj += 2;
                    adjlist_ptr = effective(adjlist);
                    while ((c = adjlist_ptr[(adjlist++) - adjlist_bak]) && (c - 3 != c2));
                    if (c - 3 != c2)
                        write_dreg_b(1, 1); /* invalid adjective */
                }
            }
            while (tmp16 && !read_dreg_b(1));
            adjlist = read_areg_w(0);
        }
        if (!read_dreg_b(1))
        {
            // Optimization: Minimize number of effective(output) calls.
            /* invalid_flag */
            flag2 |= flag;
            effective_ptr = effective(output);
            effective_ptr[0] = flag2;
            effective_ptr[1] = (type8) bank;
            write_w(effective_ptr + 2, word);
            output += 4;
        }
    }
    write_areg_w(2, output);
    output = output_bak;

    if (flag2 & 0x80)
    {
        // Optimization: Minimize number of effective(output) calls.
        type8 *output_ptr;
        effective_ptr = effective(output - 4);
        output_ptr = effective_ptr + 4;
        do
        {
            effective_ptr += 4;
            c = *effective_ptr;
        }
        while (!(c & 0x80));
        write_l(output_ptr, read_l(effective_ptr) & 0x7fffffff);
        write_areg_l(2, output + 4);
        if (longest > 1)
        {
            inc_var_areg_w(5, longest - 2);
        }
    }
    write_areg_w(6, read_areg_w(5) + 1);
}

#if USE_PROG_FORMAT

/* A0=findproperties(D0) A0FE */

static void do_findprop(void)
{
    type16 d0 = read_dreg_w(0) & 0x3fff;

    if (fp_size && (d0 > fp_size))
    {
        // Clones!
        d0 = (type16) (((fp_size - d0) ^ 0xffff) << 1);
        d0 += fp_tab;
        d0 = read_w(effective(d0));
    }
    else
    {
        write_dreg_w(0, read_dreg_w(0) & 0x7fff);
    }
    write_areg_l(0, d0 * 14 + properties);
}

#else

/* A0=findproperties(D0) A0FE */

static void do_findprop(void)
{
    type16 tmp;

    if ((version > 2) && ((read_dreg_w(0) & 0x3fff) > fp_size))
    {
        tmp = (type16) (((fp_size - (read_dreg_w(0) & 0x3fff)) ^ 0xffff) << 1);
        tmp += fp_tab;
        tmp = read_w(effective(tmp));
    }
    else
    {
        if (version < 2)
            write_dreg_l(0, read_dreg_l(0) & 0x7fff);
        else
            write_dreg_w(0, read_dreg_w(0) & 0x7fff);
        tmp = read_dreg_w(0);
    }
    tmp &= 0x3fff;
    write_areg_l(0, tmp * 14 + properties);
}

#endif

#if !USE_PROG_FORMAT

static void write_string(void)
{
    static type32 offset_bak;
    static type8 mask_bak;
    type8 c, b, mask;
    type32 offset;
    type8 *str_ptr;

    if (!cflag)
    {
        /* new string */
        type16 ptr = read_dreg_w(0);
        if (!ptr)
            offset = 0;
        else
        {
            offset = read_w(&(get_decode_table()[0x100 + 2 * ptr]));
            if (decode_table_limit)
            {
                if (ptr >= decode_table_limit)
                    offset += string_size;
            }
        }
        mask = 1;
    }
    else
    {
        offset = offset_bak;
        mask = mask_bak;
    }

    /*
     * Optimization: Minimize number of effective_string*() calls.
     * Assumption: The string is not longer than
     * (16K - address_in_bottom_8k(string)) characters and it
     * does not cross string sections.
     *
     * NOTE: In Wonderland, a string can actually cross string
     * sections so we have to take care of that.
     */
    if (offset >= string_size)
        str_ptr = effective_string2(offset - string_size);
    else
    {
        if (offset >= MAX_STRING_SIZE)
            str_ptr = effective_string3(offset - MAX_STRING_SIZE);
        else
            str_ptr = effective_string(offset);
    }

    do
    {
        c = 0;
        while (c < 0x80)
        {
            b = *str_ptr;
            if (b & mask)
                c = decode_table_chars[0x80 + c];
            else
                c = decode_table_chars[c];
            mask <<= 1;
            if (!mask)
            {
                mask = 1;
                offset++;
#if USE_MAGNETIC_WINDOWS
                if (offset == MAX_STRING_SIZE)
                    // Going from string1 to string3 section.
                    str_ptr = effective_string3(offset - MAX_STRING_SIZE);
                else if (offset == string_size)
                    // Going from string3 to string2 section.
                    str_ptr = effective_string2(offset - string_size);
                else
                    // Staying within same string section.
                    str_ptr++;
#else
                str_ptr++;
#endif
            }
        }
        c &= 0x7f;
        if (c && ((c != 0x40) || (lastchar != 0x20)))
            char_out(c);
    }
    while (c && ((c != 0x40) || (lastchar != 0x20)));

    cflag = c ? 0xff : 0;
    if (c)
    {
        offset_bak = offset;
        mask_bak = mask;
    }
}

#else

static type8 emit_stopron(void)
{
    // Save the string page.
    type8 string_page = current_page;

    // Get the stored pronoun (STOPRON).
    type16 obj = read_w(effective(stopron_symbol_address));

    // In the next iteration of the game loop, call the P.TN
    // routine (print the noun) with the noun number in STOPRON.
    obj &= 0x3FFF;
    if (obj)
    {
        // Execute:
        // D0 = STOPRON
        // JSR P.TN

        write_dreg_w(0, obj);
        push(pc);
        pc = ptn_symbol_address;
    }

    // Page in the string page again.
    current_page = string_page;
    page_in_game();

    return (obj != 0);
}

static void _emit_chars(type8 *msg)
{
    type8 last_ch = 0;

    for (;;)
    {
        type8 ch = *msg++;

        if (ch == '\n')
        {
            break;
        }
        else if ((ch == '@') && (last_ch == ' '))
        {
            /* When we encounter " @" we must print "the <item>"
             * where item is in STOPRON then continue the message.
             *
             * What we do is store any message continuation, set a
             * message countdown to 2 and, in the next iteration of
             * the game loop, execute a virtual JSR P.TN.
             *
             * Countdown of 2 will cause "the " to be printed normally
             * by this function then the P.TN routine will print "<item>"
             * by calling char_out() directly and finally, when the next
             * message is emitted (typically a newline "^"), it will
             * trigger the continuation of the original message to be
             * printed BEFORE the new message is printed.
             */
            if (emit_stopron())
            {
                if (*msg != '\n')
                {
                    // Push the stopron continuation string on the sp_stack.
                    sp_continuation_t *spc = &sp_stack[sp_stack_size];
                    spc->in_stopron = 2;
                    spc->stopron_continuation = msg;
                    spc->stopron_page = current_page;
                    sp_stack_size++;
                }
                return;
            }
        }
        else
        {
            char_out(ch);
        }

        last_ch = ch;
    }
}

static void emit_chars(type8 *msg)
{
    if (sp_stack_size)
    {
        // Wait for the countdown to hit zero then, if there is a continuation, emit it.
        sp_continuation_t *spc = &sp_stack[sp_stack_size - 1];
        if (!--spc->in_stopron)
        {
            type8 *str;
            type8 string_page;

            // Swap the current msg string with the stopron continuation string.
            str = spc->stopron_continuation;
            spc->stopron_continuation = msg;
            msg = str;

            // Swap the current msg page with the stopron continuation page.
            string_page = current_page;
            current_page = spc->stopron_page;
            page_in_game();
            spc->stopron_page = string_page;
        }
    }

    _emit_chars(msg);

    while (sp_stack_size && !sp_stack[sp_stack_size - 1].in_stopron)
    {
        sp_continuation_t *spc;

        // Pop the sp_stack and emit its string.

        sp_stack_size--;
        spc = &sp_stack[sp_stack_size];

        current_page = spc->stopron_page;
        page_in_game();

        _emit_chars(spc->stopron_continuation);
    }
}

static void write_string(void)
{
    type16 offset_array_size;
    type32 *offset_array;
    type16 index;
    type32 offset;
    type8 *str;

    // Assumption: String index fits in 16 KB.
    ZXN_WRITE_MMU0(string_index_base_page);
    ZXN_WRITE_MMU1(string_index_base_page + 1);

    offset_array_size = *((type16 *) 0);
    offset_array = (type32 *) 2;

    index = read_dreg_w(0);
    if (index >= offset_array_size)
    {
        return;
    }

    offset = offset_array[index];
    str = effective_string(offset);

    // Assumption: The string is not longer than (16K - address_in_bottom_8k(string)) characters.
    emit_chars(str);
}

#endif

static void do_line_a(void)
{
    type8 l1c;
    type8 *str;
    type16 ptr, ptr2, tmp16;
    type32 tmp32;

    type8 *effective_ptr;
    type16 d2_reg;
    type16 d4_reg;
    type8 tmp8;

#if USE_PROG_FORMAT
    if (byte2 < 0xdc)
#else
    if ((byte2 < 0xdd) || (version < 4 && byte2 < 0xe4) || (version < 2 && byte2 < 0xed))
#endif
    {
        ms_flush(); /* flush output-buffer */
        rand_emu(); /* Increase game randomness */
        l1c = ms_getchar(1); /* 0 means UNDO */
        write_dreg_l(1, l1c); /* d1=getkey() */
    }
    else
    {
#if USE_PROG_FORMAT
        if (byte2 == 0xdc)
        {
            // New info traps (A0DC). A 16-bit number is passed on the stack;
            // this number can signal InfoAtScreen (1), InfoAtCommand (2) or
            // InfoDump (3). These traps are not used in the ZX Spectrum Next
            // version but we still need to pop their values from the stack.
            write_areg_l(7, read_areg_l(7) + 2);
        }
#endif

        switch ((type8) (byte2 - 0xdd))
        {

        case 0: /* A0DD - Won't probably be needed at all */
            break;

        case 1: /* A0DE */
            write_dreg_b(1, 1); /* Should remove the manual check */
            break;

        case 2: /* A0DF */
            {
                type32 a1reg = read_areg_l(1);
                type8 dtype = effective(a1reg + 2)[0];
                if (dtype == 7)
                {
                    /* Picture: gfx mode = normal, df is not called if graphics are off */
#if USE_ANIM
                    str = effective(a1reg + 3);
                    save_str(&str);
                    str[6] = '\0'; // Max six chars
                    ms_showpic(str, 2);
#else
                    ms_showpic((type8) (a1reg + 3), 2);
#endif
                }
            }
            break;

        case 3: /* A0E0 */
            break;

        case 4: /* A0E1 Read from keyboard to (A1), status in D1 (0 for ok) */
            ms_flush();
            rand_emu();
            tmp32 = read_areg_l(1);
            str = effective(tmp32);
            tmp16 = 0;
            do
            {
                l1c = ms_getchar(1);
                str[tmp16++] = l1c;
            }
            while (l1c != '\n' && tmp16 < 256);
            write_areg_l(1, tmp32 + tmp16 - 1);
            if (tmp16 != 256 && tmp16 != 1)
                write_dreg_w(1, 0);
            else
                write_dreg_w(1, 1);
            break;

        case 5: /* A0E2 */
            break;

        case 6: /* A0E3 */
#if !USE_PROG_FORMAT
            if (read_dreg_l(1) == 0)
            {
                if ((version < 4) || (read_dreg_l(6) == 0))
                    ms_showpic(0, 0);
            }
#endif
            break;

        case 7: /* A0E4 sp+=4, RTS */
            inc_var_areg_w(7, 4);
            pc = pop();
            break;

        case 8: /* A0E5 set z, RTS */
        case 9: /* A0E6 clear z, RTS */
            pc = pop();
            zflag = (byte2 == 0xe5) ? 0xff : 0;
            break;

        case 10: /* A0E7 set z */
            zflag = 0xff;
            break;

        case 11: /* A0E8 clear z */
            zflag = 0;
            break;

        case 12: /* A0E9 */
            ptr = read_areg_w(0);
            ptr2 = read_areg_w(1);
            do
            {
                l1c = effective_dict(ptr2++)[0];
                effective(ptr++)[0] = l1c;
            }
            while ((l1c & 0x80) == 0);
            write_areg_w(0, ptr);
            write_areg_w(1, ptr2);
            break;

        case 13: /* A0EA A1=write_dictword(A1,D1=output_mode) */
            /*
             * Optimization: Minimize number of effective_dict() calls.
             * Assumption: ptr is not incremented more than
             * (16K - address_in_bottom_8k(ptr)) times.
             */
            ptr = read_areg_w(1);
            effective_ptr = effective_dict(ptr);
            tmp16 = ptr;
            tmp8 = read_dreg_b(3);
            write_dreg_b(3, read_dreg_b(1));
            do
            {
                l1c = effective_ptr[(ptr++) - tmp16];
                char_out(l1c);
            }
            while (l1c < 0x80);
            write_areg_w(1, ptr);
            write_dreg_b(3, tmp8);
            break;

        case 14: /* A0EB */
            effective_dict(read_areg_w(1))[0] = read_dreg_b(1);
            break;

        case 15: /* A0EC */
            write_dreg_b(1, effective_dict(read_areg_w(1))[0]);
            break;

        case 16: /* A0ED */
            ms_stop(); /* infinite loop */
            break;

        case 17: /* A0EE */
            if (!ms_init(NULL))
                ms_stop(); /* restart game ie. pc, sp etc. */
            add_startup_commands();
#if USE_PROG_FORMAT
            ms_config();
#endif
            break;

        case 18: /* A0EF printer */
            break;

        case 19: /* A0F0 */
#if USE_ANIM
            str = effective(read_dreg_b(0));
            save_str(&str);
            str[6] = '\0'; // Max six chars
            ms_showpic(str, read_dreg_b(1)); /* Do_picture(D0) */
#else
            ms_showpic(read_dreg_b(0), read_dreg_b(1)); /* Do_picture(D0) */
#endif
            break;

        case 20: /* A0F1 */
            {
                /*
                 * Optimization: Minimize number of effective() calls.
                 * Assumption: ptr is not incremented more than
                 * (16K - address_in_bottom_8k(ptr)) times.
                 */
                type16 d0_reg;
                ptr = read_areg_w(1); /* A1=nth_string(A1,D0) */
                effective_ptr = effective(ptr);
                tmp16 = ptr;
                d0_reg = read_dreg_w(0);
                while (d0_reg-- > 0)
                {
                    while (effective_ptr[(ptr++) - tmp16]);
                }
                write_areg_w(1, ptr);
            }
            break;

        case 21: /* A0F2 */
            cflag = 0;
            write_dreg_w(0, read_dreg_w(2));
            do_findprop();
            ptr = read_areg_w(0);
            d4_reg = read_dreg_w(4);
            while (read_dreg_w(2) > 0)
            {
                if (read_w(effective(ptr + 12)) & 0x3fff)
                {
                    cflag = 0xff;
                    break;
                }
                if (read_dreg_w(2) == (d4_reg & 0x7fff))
                {
                    cflag = 0xff;
                    break;
                }
                ptr -= 0x0e;
                dec_dreg_w(2);
            }
            break;

        case 22: /* A0F3 */
            char_out(read_dreg_b(1));
            break;

        case 23: /* A0F4 D7=Save_(filename A0) D1 bytes starting from A1 */
            str = (version < 4) ? effective(read_areg_w(0)) : NULL;
            save_str(&str);
            write_dreg_b(7, ms_save_file(str, effective(read_areg_w(1)), read_dreg_w(1)));
            break;

        case 24: /* A0F5 D7=Load_(filename A0) D1 bytes starting from A1 */
            str = (version < 4) ? effective(read_areg_w(0)) : NULL;
            save_str(&str);
            write_dreg_b(7, ms_load_file(str, effective(read_areg_w(1)), read_dreg_w(1)));
            break;

        case 25: /* A0F6 D1=Random(0..D1-1) */
            l1c = read_dreg_b(1);
            write_dreg_w(1, (type16) (rand_emu() % (l1c ? l1c : 1)));
            break;

        case 26: /* A0F7 D0=Random(0..255) */
            tmp16 = (type16) rand_emu();
            write_dreg_b(0, (type8) (tmp16 + (tmp16 >> 8)));
            break;

        case 27: /* A0F8 write string [D0] */
            write_string();
            break;

        case 28: /* A0F9 Z,D0=Get_inventory_item(D0) */
            /*
             * Optimization: Minimize number of effective() calls.
             */
            zflag = 0;
            ptr = read_dreg_w(0);
            do
            {
                write_dreg_w(0, ptr);
                do
                {
                    do_findprop();
                    ptr2 = read_areg_w(0); /* object properties */
                    effective_ptr = effective(ptr2);
                    if ((effective_ptr[5]) & 1)
                        break; /* is_described or so */
                    l1c = effective_ptr[6]; /* some_flags */
                    tmp16 = read_w(effective_ptr + 8); /* parent_object */
                    if (!l1c)
                    {
                        /* ordinary object? */
                        if (!tmp16)
                            zflag = 0xff; /* return if parent()=player */
                        break; /* otherwise try next */
                    }
                    if (l1c & 0xcc)
                        break; /* skip worn, bodypart, room, hidden */
                    if (tmp16 == 0)
                    {
                        /* return if parent()=player? */
                        zflag = 0xff;
                        break;
                    }
                    write_dreg_w(0, tmp16); /* else look at parent() */
                }
                while (1);
                ptr--;
            }
            while ((!zflag) && ptr);
            write_dreg_w(0, ptr + 1);
            break;

        case 29: /* A0FA */
            /*
             * Optimization: Minimize number of effective() calls and cache
             * read-only registers.
             */
            ptr = read_areg_w(0);
            effective_ptr = effective(ptr);
            d2_reg = read_dreg_w(2);
            d4_reg = read_dreg_w(4);
            tmp8 = read_dreg_b(5);
            do
            {
                if (tmp8)
                {
                    l1c = ((read_w(effective_ptr) & 0x3fff) == d2_reg);
                }
                else
                {
                    l1c = (effective_ptr[0] == ((type8) d2_reg));
                }
                if (read_dreg_w(3) == d4_reg)
                {
                    cflag = 0;
                    write_areg_w(0, ptr);
                }
                else
                {
                    inc_dreg_w(3);
                    ptr += 0x0e;
                    effective_ptr += 0x0e;
                    // Call effective() again if the 16K paging area is crossed.
                    if (effective_ptr > (type8 *) 0x3FFE)
                    {
                        effective_ptr = effective(ptr);
                    }
                    if (l1c)
                    {
                        cflag = 0xff;
                        write_areg_w(0, ptr);
                    }
                }
            }
            while ((!l1c) && (read_dreg_w(3) != d4_reg));
            break;

        case 30: /* A0FB */
            /*
             * Optimization: Minimize number of effective()/effective_dict() calls.
             * Assumption: ptr is not incremented more than
             * (16K - address_in_bottom_8k(ptr)) times.
             */
            ptr = read_areg_w(1);
            effective_ptr = sd ? effective_dict(ptr) : effective(ptr);
            tmp16 = ptr;
            do
            {
                while (effective_ptr[(ptr++) - tmp16] < 0x80);
                dec_dreg_w(2);
            }
            while (read_dreg_w(2));
            write_areg_w(1, ptr);
            break;

        case 31: /* A0FC */
            /*
             * Optimization: Minimize number of effective()/effective_dict() calls.
             * Assumption: ptr and ptr2 are not incremented more than
             * (16K - address_in_bottom_8k(ptr)) and
             * (16K - address_in_bottom_8k(ptr2)) times, respectively.
             */
            ptr = read_areg_w(0);
            ptr2 = read_areg_w(1);
            do
            {
                effective_ptr = sd ? effective_dict(ptr) : effective(ptr);
                tmp16 = ptr;
                while (effective_ptr[(ptr++) - tmp16] < 0x80);

                effective_ptr = effective(ptr2);
                tmp16 = ptr2;
                while (effective_ptr[(ptr2++) - tmp16]);

                dec_dreg_w(0);
            }
            while (read_dreg_w(0));
            write_areg_w(0, ptr);
            write_areg_w(1, ptr2);
            break;

        case 32: /* A0FD Set properties pointer from A0 */
            properties = read_areg_w(0);
            if (version > 0)
                fl_sub = read_areg_w(3);
            if (version > 1)
            {
                fl_tab = read_areg_w(5);
                fl_size = read_dreg_w(7) + 1;
                /* A3 [routine], A5 [table] and D7 [table-size] */
            }
#if !USE_PROG_FORMAT
            if (version > 2)
            {
                fp_tab = read_areg_w(6);
                fp_size = read_dreg_w(6);
            }
#else
            // Only used when the game has wanted_CLONES set.
            fp_tab = read_areg_w(2);
            fp_size = read_dreg_w(4);
#endif
            break;

        case 33: /* A0FE */
            do_findprop();
            break;

        case 34: /* A0FF Dictionary_lookup */
            dict_lookup();
            break;
        }
    }
}

/* emulate an instruction */

type8 ms_rungame(void)
{
    type8 l1c;
    type16 tmp16;
    type32 tmp32;
#ifdef LOGEMU
    type32 start_time;
    type32 elapsed_time;
    static type8 stat = 0;
#endif

    if (!running)
        return running;

#ifdef LOGEMU
    start_time = frame_counter;
    if (stat)
    {
        log_status();
    }

    out("%.8X: ", pc);
#endif

    i_count++;
    read_word();

    switch (byte1 >> 1)
    {

/* 00-0F */
    case 0x00:
        if (byte1 == 0x00)
        {
            if (byte2 == 0x3c || byte2 == 0x7c)
            {
                /* OR immediate to CCR (30D9) */
                read_word();
#ifdef LOGEMU
                out("or_ccr #%.2X", byte2);
#endif
                if (byte2 & 0x01)
                    cflag = 0xff;
                if (byte2 & 0x02)
                    vflag = 0xff;
                if (byte2 & 0x04)
                    zflag = 0xff;
                if (byte2 & 0x08)
                    nflag = 0xff;
            }
            else
            {
                /* OR */
#ifdef LOGEMU
                out("or");
#endif
                get_arg();
                do_or();
            }
        }
        else
            check_btst();
        break;

    case 0x01:
        if (byte1 == 0x02)
        {
            if (byte2 == 0x3c || byte2 == 0x7c)
            {
                /* AND immediate to CCR */
                read_word();
#ifdef LOGEMU
                out("and_ccr #%.2X", byte2);
#endif
                if (!(byte2 & 0x01))
                    cflag = 0;
                if (!(byte2 & 0x02))
                    vflag = 0;
                if (!(byte2 & 0x04))
                    zflag = 0;
                if (!(byte2 & 0x08))
                    nflag = 0;
            }
            else
            {
                /* AND */
#ifdef LOGEMU
                out("and");
#endif
                get_arg();
                do_and();
            }
        }
        else
            check_btst();
        break;

    case 0x02:
        if (byte1 == 0x04)
        {
            /* SUB */
#ifdef LOGEMU
            out("sub");
#endif
            get_arg();
            do_sub();
        }
        else
            check_btst();
        break;

    case 0x03:
        if (byte1 == 0x06)
        {
            /* ADD */
#ifdef LOGEMU
            out("addi");
#endif
            get_arg();
            do_add();
        }
        else
            check_btst();
        break;

    case 0x04:
        if (byte1 == 0x08)
        {
            /* bit operations (immediate) */
            set_info((type8) (byte2 & 0x3f));
            l1c = (effective(pc))[1];
            pc += 2;
            set_arg1();
            do_bop(byte2, l1c);
        }
        else
            check_btst();
        break;

    case 0x05:
        if (byte1 == 0x0a)
        {
            if (byte2 == 0x3c || byte2 == 0x7c)
            {
                /* EOR immediate to CCR */
                read_word();
#ifdef LOGEMU
                out("eor_ccr #%.2X", byte2);
#endif
                if (byte2 & 0x01)
                    cflag ^= 0xff;
                if (byte2 & 0x02)
                    vflag ^= 0xff;
                if (byte2 & 0x04)
                    zflag ^= 0xff;
                if (byte2 & 0x08)
                    nflag ^= 0xff;
            }
            else
            {
                /* EOR */
#ifdef LOGEMU
                out("eor");
#endif
                get_arg();
                do_eor();
            }
        }
        else
            check_btst();
        break;

    case 0x06:
        if (byte1 == 0x0c)
        {
            /* CMP */
#ifdef LOGEMU
            out("cmp");
#endif
            get_arg();
            do_cmp();
        }
        else
            check_btst();
        break;

    case 0x07:
        check_btst();
        break;

/* 10-1F MOVE.B */
    case 0x08: case 0x09: case 0x0a: case 0x0b:
    case 0x0c: case 0x0d: case 0x0e: case 0x0f:

#ifdef LOGEMU
        out("move.b");
#endif
        set_info((type8) (byte2 & 0x3f));
        set_arg1();
        swap_args();
        save_arg2();
        l1c = (byte1 >> 1 & 0x07) | (byte2 >> 3 & 0x18) | (byte1 << 5 & 0x20);
        set_info(l1c);
        set_arg1();
        do_move_b();
        break;

/* 20-2F MOVE.L */
    case 0x10: case 0x11: case 0x12: case 0x13:
    case 0x14: case 0x15: case 0x16: case 0x17:

#ifdef LOGEMU
        out("move.l");
#endif
        set_info((type8) ((byte2 & 0x3f) | 0x80));
        set_arg1();
        swap_args();
        save_arg2();
        l1c = (byte1 >> 1 & 0x07) | (byte2 >> 3 & 0x18) | (byte1 << 5 & 0x20);
        set_info((type8) (l1c | 0x80));
        set_arg1();
        do_move_l();
        break;

/* 30-3F MOVE.W */
    case 0x18: case 0x19: case 0x1a: case 0x1b:
    case 0x1c: case 0x1d: case 0x1e: case 0x1f:

#ifdef LOGEMU
        out("move.w");
#endif
        set_info((type8) ((byte2 & 0x3f) | 0x40));
        set_arg1();
        swap_args();
        save_arg2();
        l1c = (byte1 >> 1 & 0x07) | (byte2 >> 3 & 0x18) | (byte1 << 5 & 0x20);
        set_info((type8) (l1c | 0x40));
        set_arg1();
        do_move_w();
        break;

/* 40-4F various commands */
    case 0x20:
        if (byte1 == 0x40)
        {
            ms_fatal("unimplemented instructions NEGX and MOVE SR,xx");
        }
        else
            check_lea();
        break;

    case 0x21:
        if (byte1 == 0x42)
        {
            if ((byte2 & 0xc0) == 0xc0)
            {
                ms_fatal("unimplemented instruction MOVE CCR,xx");
            }
            else
            {
                /* CLR */
#ifdef LOGEMU
                out("clr");
#endif
                set_info(byte2);
                set_arg1();
                if (opsize == 0)
                    arg1[0] = 0;
                else if (opsize == 1)
                    *(type16 *)(arg1) = 0;
                else if (opsize == 2)
                    *(type32 *)(arg1) = 0;
                nflag = cflag = 0;
                zflag = 0xff;
            }
        }
        else
            check_lea();
        break;

    case 0x22:
        if (byte1 == 0x44)
        {
            if ((byte2 & 0xc0) == 0xc0)
            {
                /* MOVE to CCR */
#ifdef LOGEMU
                out("move_ccr");
#endif
                zflag = nflag = cflag = vflag = 0;
                set_info((type8) (byte2 & 0x7f));
                set_arg1();
                byte2 = arg1[1];
                if (byte2 & 0x01)
                    cflag = 0xff;
                if (byte2 & 0x02)
                    vflag = 0xff;
                if (byte2 & 0x04)
                    zflag = 0xff;
                if (byte2 & 0x08)
                    nflag = 0xff;
            }
            else
            {
#ifdef LOGEMU
                out("neg");
#endif
                set_info(byte2); /* NEG */
                set_arg1();
                cflag = 0xff;
                if (opsize == 0)
                {
                    arg1[0] = -arg1[0];
                    cflag = arg1[0] ? 0xff : 0;
                }
                else if (opsize == 1)
                {
                    write_w(arg1, -((type16) read_w(arg1)));
                    cflag = read_w(arg1) ? 0xff : 0;
                }
                else if (opsize == 2)
                {
                    write_l(arg1, -read_l(arg1));
                    cflag = read_l(arg1) ? 0xff : 0;
                }
                vflag = 0;
                set_flags();
            }
        }
        else
            check_lea();
        break;

    case 0x23:
        if (byte1 == 0x46)
        {
            if ((byte2 & 0xc0) == 0xc0)
            {
                ms_fatal("unimplemented instruction MOVE xx,SR");
            }
            else
            {
#ifdef LOGEMU
                out("not");
#endif
                set_info(byte2); /* NOT */
                set_arg1();
                memset(tmparg, 0xff, sizeof(tmparg));
                arg2 = tmparg;
                do_eor();
            }
        }
        else
            check_lea();
        break;

    case 0x24:
        if (byte1 == 0x48)
        {
            if ((byte2 & 0xf8) == 0x40)
            {
#ifdef LOGEMU
                out("swap");
#endif
                opsize = 2; /* SWAP */
                admode = 0;
                regnr = byte2 & 0x07;
                set_arg1();
                tmp16 = read_w(arg1);
                write_w(arg1, read_w(arg1 + 2));
                write_w(arg1 + 2, tmp16);
                set_flags();
            }
            else if ((byte2 & 0xf8) == 0x80)
            {
#ifdef LOGEMU
                out("ext.w");
#endif
                opsize = 1; /* EXT.W */
                admode = 0;
                regnr = byte2 & 0x07;
                set_arg1();
                if (arg1[1] > 0x7f)
                    arg1[0] = 0xff;
                else
                    arg1[0] = 0;
                set_flags();
            }
            else if ((byte2 & 0xf8) == 0xc0)
            {
#ifdef LOGEMU
                out("ext.l");
#endif
                opsize = 2; /* EXT.L */
                admode = 0;
                regnr = byte2 & 0x07;
                set_arg1();
                if (read_w(arg1 + 2) > 0x7fff)
                    *(type16 *)(arg1) = 0xffff;
                else
                    *(type16 *)(arg1) = 0;
                set_flags();
            }
            else if ((byte2 & 0xc0) == 0x40)
            {
#ifdef LOGEMU
                out("pea");
#endif
                set_info((type8) ((byte2 & 0x3f) | 0x80)); /* PEA */
                set_arg1();
                if (is_reversible)
                    push(arg1i);
                else
                    ms_fatal("illegal addressing mode for PEA");
            }
            else
            {
                check_movem(); /* MOVEM */
            }
        }
        else
            check_lea();
        break;

    case 0x25:
        if (byte1 == 0x4a)
        {
            /* TST */
            if ((byte2 & 0xc0) == 0xc0)
            {
                ms_fatal("unimplemented instruction TAS");
            }
            else
            {
#ifdef LOGEMU
                out("tst");
#endif
                set_info(byte2);
                set_arg1();
                cflag = vflag = 0;
                set_flags();
            }
        }
        else
            check_lea();
        break;

    case 0x26:
        if (byte1 == 0x4c)
            check_movem2(); /* MOVEM.L (Ax)+,A/Dx */
        else
            check_lea(); /* LEA */
        break;

    case 0x27:
        if (byte1 == 0x4e)
        {
            if (byte2 == 0x75)
            {
                /* RTS */
#ifdef LOGEMU
                out("rts\n");
#endif
                pc = pop();
            }
            else if (byte2 == 0x71)
            {
                /* NOP */
#ifdef LOGEMU
                out("nop");
#endif
            }
            else if ((byte2 & 0xc0) == 0xc0)
            {
                /* indir JMP */
#ifdef LOGEMU
                out("jmp");
#endif
                set_info((type8) (byte2 | 0xc0));
                set_arg1();
                if (is_reversible)
                    pc = arg1i;
                else
                    ms_fatal("illegal addressing mode for JMP");
            }
            else if ((byte2 & 0xc0) == 0x80)
            {
#ifdef LOGEMU
                out("jsr");
#endif
                set_info((type8) (byte2 | 0xc0)); /* indir JSR */
                set_arg1();
                push(pc);
                if (is_reversible)
                    pc = arg1i;
                else
                    ms_fatal("illegal addressing mode for JSR");
            }
            else
            {
                ms_fatal("unimplemented instructions 0x4EXX");
            }
        }
        else
            check_lea(); /* LEA */
        break;

/* 50-5F ADDQ/SUBQ/Scc/DBcc */
    case 0x28: case 0x29: case 0x2a: case 0x2b:
    case 0x2c: case 0x2d: case 0x2e: case 0x2f:

        if ((byte2 & 0xc0) == 0xc0)
        {
            set_info((type8) (byte2 & 0x3f));
            set_arg1();
            if (admode == 1)
            {
                /* DBcc */
#ifdef LOGEMU
                out("dbcc");
#endif
                if (condition(byte1) == 0)
                {
                    arg1 = (arg1 - (type8 *) areg) + (type8 *) dreg - 1;
                    write_w(arg1, (type16) (read_w(arg1) - 1));
                    if (read_w(arg1) != 0xffff)
                        branch(0);
                    else
                        pc += 2;
                }
                else
                    pc += 2;
            }
            else
            {
                /* Scc */
#ifdef LOGEMU
                out("scc");
#endif
                arg1[0] = condition(byte1) ? 0xff : 0;
            }
        }
        else
        {
            set_info(byte2);
            set_arg1();
            quick_flag = (admode == 1) ? 0xff : 0;
            l1c = byte1 >> 1 & 0x07;
            tmparg[0] = tmparg[1] = tmparg[2] = 0;
            tmparg[3] = l1c ? l1c : 8;
            arg2 = reg_align(tmparg, opsize);
            {
#ifdef LOGEMU
                type32s outnum = 0;
                switch (opsize)
                {
                case 0:
                    outnum = (type8s) arg2[0];
                    break;
                case 1:
                    outnum = (type16s) read_w(arg2);
                    break;
                case 2:
                    outnum = (type32s) read_l(arg2);
                    break;
                }
#endif
                if ((byte1 & 0x01) == 1)
                {
#ifdef LOGEMU
                    out("subq #%.8X", outnum);
#endif
                    do_sub(); /* SUBQ */
                }
                else
                {
#ifdef LOGEMU
                    out("addq #%.8X", outnum);
#endif
                    do_add(); /* ADDQ */
                }
            }
        }
        break;

/* 60-6F Bcc */
    case 0x30:
        if (byte1 == 0x61)
        {
            /* BRA, BSR */
#ifdef LOGEMU
            out("bsr");
#endif
            if (byte2 == 0)
                push(pc + 2);
            else
                push(pc);
        }
#ifdef LOGEMU
        else
            out("bra");
#endif
        if ((byte1 == 0x60) && (byte2 == 0xfe))
        {
            ms_flush(); /* flush stdout */
            ms_stop(); /* infinite loop - just exit */
        }
        branch(byte2);
        break;

    case 0x31: case 0x32: case 0x33: case 0x34:
    case 0x35: case 0x36: case 0x37:

        if (condition(byte1) == 0)
        {
#ifdef LOGEMU
            out("beq.s");
#endif
            if (byte2 == 0)
                pc += 2;
        }
        else
        {
#ifdef LOGEMU
            out("bra");
#endif
            branch(byte2);
        }
        break;

/* 70-7F MOVEQ */
    case 0x38: case 0x39: case 0x3a: case 0x3b:
    case 0x3c: case 0x3d: case 0x3e: case 0x3f:

#ifdef LOGEMU
        out("moveq");
#endif
        arg1 = (type8 *) &dreg[byte1 >> 1 & 0x07];
        if (byte2 > 127)
            nflag = arg1[0] = arg1[1] = arg1[2] = 0xff;
        else
            nflag = arg1[0] = arg1[1] = arg1[2] = 0;
        arg1[3] = byte2;
        zflag = byte2 ? 0 : 0xff;
        break;

/* 80-8F */
    case 0x40: case 0x41: case 0x42: case 0x43:
    case 0x44: case 0x45: case 0x46: case 0x47:

        if ((byte2 & 0xc0) == 0xc0)
        {
            ms_fatal("unimplemented instructions DIVS and DIVU");
        }
        else if (((byte2 & 0xf0) == 0) && ((byte1 & 0x01) != 0))
        {
            ms_fatal("unimplemented instruction SBCD");
        }
        else
        {
#ifdef LOGEMU
            out("or?");
#endif
            set_info(byte2);
            set_arg1();
            set_arg2(1, byte1);
            if ((byte1 & 0x01) == 0)
                swap_args();
            do_or();
        }
        break;

/* 90-9F SUB */
    case 0x48: case 0x49: case 0x4a: case 0x4b:
    case 0x4c: case 0x4d: case 0x4e: case 0x4f:

#ifdef LOGEMU
        out("sub");
#endif
        quick_flag = 0;
        if ((byte2 & 0xc0) == 0xc0)
        {
            if ((byte1 & 0x01) == 1)
                set_info((type8) (byte2 & 0xbf));
            else
                set_info((type8) (byte2 & 0x7f));
            set_arg1();
            set_arg2_nosize(0, byte1);
            swap_args();
            do_suba();
        }
        else
        {
            set_info(byte2);
            set_arg1();
            set_arg2(1, byte1);
            if ((byte1 & 0x01) == 0)
                swap_args();
            do_sub();
        }
        break;

/* A0-AF various special commands [LINE_A] */
    case 0x50: case 0x56: case 0x57:
        do_line_a();
#ifdef LOGEMU
        out("LINE_A A0%.2X", byte2);
#endif
        break;

    case 0x51:
#ifdef LOGEMU
        out("rts\n");
#endif
        pc = pop(); /* RTS */
        break;

    case 0x52:
#ifdef LOGEMU
        out("bsr");
#endif
        if (byte2 == 0)
            push(pc + 2); /* BSR */
        else
            push(pc);
        branch(byte2);
        break;

    case 0x53:
        if ((byte2 & 0xc0) == 0xc0)
        {
            /* TST */
            ms_fatal("unimplemented instructions LINE_A #$6C0-#$6FF");
        }
        else
        {
#ifdef LOGEMU
            out("tst");
#endif
            set_info(byte2);
            set_arg1();
            cflag = vflag = 0;
            set_flags();
        }
        break;

    case 0x54:
        check_movem();
        break;

    case 0x55:
        check_movem2();
        break;

/* B0-BF */
    case 0x58: case 0x59: case 0x5a: case 0x5b:
    case 0x5c: case 0x5d: case 0x5e: case 0x5f:

        if ((byte2 & 0xc0) == 0xc0)
        {
#ifdef LOGEMU
            out("cmp");
#endif
            if ((byte1 & 0x01) == 1)
                set_info((type8) (byte2 & 0xbf));
            else
                set_info((type8) (byte2 & 0x7f));
            set_arg1();
            set_arg2(0, byte1);
            swap_args();
            do_cmp(); /* CMP */
        }
        else
        {
            if ((byte1 & 0x01) == 0)
            {
#ifdef LOGEMU
                out("cmp");
#endif
                set_info(byte2);
                set_arg1();
                set_arg2(1, byte1);
                swap_args();
                do_cmp(); /* CMP */
            }
            else
            {
#ifdef LOGEMU
                out("eor");
#endif
                set_info(byte2);
                set_arg1();
                set_arg2(1, byte1);
                do_eor(); /* EOR */
            }
        }
        break;

/* C0-CF EXG, AND */
    case 0x60: case 0x61: case 0x62: case 0x63:
    case 0x64: case 0x65: case 0x66: case 0x67:

        if ((byte1 & 0x01) == 0)
        {
            if ((byte2 & 0xc0) == 0xc0)
            {
                ms_fatal("unimplemented instruction MULU");
            }
            else
            {
                /* AND */
#ifdef LOGEMU
                out("and");
#endif
                set_info(byte2);
                set_arg1();
                set_arg2(1, byte1);
                if ((byte1 & 0x01) == 0)
                    swap_args();
                do_and();
            }
        }
        else
        {
            if ((byte2 & 0xf8) == 0x40)
            {
#ifdef LOGEMU
                out("exg (dx)");
#endif
                opsize = 2; /* EXG Dx,Dx */
                set_arg2(1, (type8) (byte2 << 1));
                swap_args();
                set_arg2(1, byte1);
                tmp32 = read_l(arg1);
                memcpy(arg1, arg2, sizeof(type32));
                write_l(arg2, tmp32);
            }
            else if ((byte2 & 0xf8) == 0x48)
            {
                opsize = 2; /* EXG Ax,Ax */
#ifdef LOGEMU
                out("exg (ax)");
#endif
                set_arg2(0, (type8) (byte2 << 1));
                swap_args();
                set_arg2(0, byte1);
                tmp32 = read_l(arg1);
                memcpy(arg1, arg2, sizeof(type32));
                write_l(arg2, tmp32);
            }
            else if ((byte2 & 0xf8) == 0x88)
            {
                opsize = 2; /* EXG Dx,Ax */
#ifdef LOGEMU
                out("exg (dx,ax)");
#endif
                set_arg2(0, (type8) (byte2 << 1));
                swap_args();
                set_arg2(1, byte1);
                tmp32 = read_l(arg1);
                memcpy(arg1, arg2, sizeof(type32));
                write_l(arg2, tmp32);
            }
            else
            {
                if ((byte2 & 0xc0) == 0xc0)
                {
                    ms_fatal("unimplemented instruction MULS");
                }
                else
                {
                    set_info(byte2);
                    set_arg1();
                    set_arg2(1, byte1);
                    if ((byte1 & 0x01) == 0)
                        swap_args();
                    do_and();
                }
            }
        }
        break;

/* D0-DF ADD */
    case 0x68: case 0x69: case 0x6a: case 0x6b:
    case 0x6c: case 0x6d: case 0x6e: case 0x6f:

#ifdef LOGEMU
        out("add");
#endif
        quick_flag = 0;
        if ((byte2 & 0xc0) == 0xc0)
        {
            if ((byte1 & 0x01) == 1)
                set_info((type8) (byte2 & 0xbf));
            else
                set_info((type8) (byte2 & 0x7f));
            set_arg1();
            set_arg2_nosize(0, byte1);
            swap_args();
            do_adda();
        }
        else
        {
            set_info(byte2);
            set_arg1();
            set_arg2(1, byte1);
            if ((byte1 & 0x01) == 0)
                swap_args();
            do_add();
        }
        break;

/* E0-EF LSR ASL ROR ROL */
    case 0x70: case 0x71: case 0x72: case 0x73:
    case 0x74: case 0x75: case 0x76: case 0x77:

#ifdef LOGEMU
        out("lsr,asl,ror,rol");
#endif
        if ((byte2 & 0xc0) == 0xc0)
        {
            set_info((type8) (byte2 & 0xbf)); /* OP Dx */
            set_arg1();
            l1c = 1; /* steps=1 */
            byte2 = (byte1 >> 1) & 0x03;
        }
        else
        {
            set_info((type8) (byte2 & 0xc7));
            set_arg1();
            if ((byte2 & 0x20) == 0)
            {
                /* immediate */
                l1c = (byte1 >> 1) & 0x07;
                if (l1c == 0)
                    l1c = 8;
            }
            else
            {
                l1c = read_dreg_b(byte1 >> 1 & 0x07);
            }
            byte2 = (byte2 >> 3) & 0x03;
        }
        if ((byte1 & 0x01) == 0)
        {
            /* right */
            while (l1c-- > 0)
            {
                if (opsize == 0)
                {
                    cflag = arg1[0] & 0x01 ? 0xff : 0;
                    arg1[0] >>= 1;
                    if (cflag && (byte2 == 3))
                        arg1[0] |= 0x80;
                }
                else if (opsize == 1)
                {
                    tmp16 = read_w(arg1);
                    cflag = tmp16 & 0x01 ? 0xff : 0;
                    write_w(arg1, (type16) (tmp16 >> 1));
                    if (cflag && (byte2 == 3))
                        write_w(arg1, (type16) (read_w(arg1) | ((type16) 1 << 15)));
                }
                else if (opsize == 2)
                {
                    tmp32 = read_l(arg1);
                    cflag = tmp32 & 0x01 ? 0xff : 0;
                    write_l(arg1, tmp32 >> 1);
                    if (cflag && (byte2 == 3))
                        write_l(arg1, read_l(arg1) | ((type32) 1 << 31));
                }
            }
        }
        else
        {
            /* left */
            while (l1c-- > 0)
            {
                if (opsize == 0)
                {
                    cflag = arg1[0] & 0x80 ? 0xff : 0;
                    arg1[0] <<= 1;
                    if (cflag && (byte2 == 3))
                        arg1[0] |= 0x01;
                }
                else if (opsize == 1)
                {
                    tmp16 = read_w(arg1);
                    cflag = tmp16 & ((type16) 1 << 15) ? 0xff : 0;
                    write_w(arg1, (type16) (tmp16 << 1));
                    if (cflag && (byte2 == 3))
                        write_w(arg1, (type16) (read_w(arg1) | 0x01));
                }
                else if (opsize == 2)
                {
                    tmp32 = read_l(arg1);
                    cflag = tmp32 & ((type32) 1 << 31) ? 0xff : 0;
                    write_l(arg1, tmp32 << 1);
                    if (cflag && (byte2 == 3))
                        write_l(arg1, read_l(arg1) | 0x01);
                }
            }
        }
        set_flags();
        break;

/* F0-FF LINE_F */
    case 0x78: case 0x79: case 0x7a: case 0x7b:
    case 0x7c: case 0x7d: case 0x7e: case 0x7f:

        if (version == 0)
        {
            /* hardcoded jump */
            char_out(l1c = read_dreg_b(1));
        }
        else if (version == 1)
        {
            /* single programmable shortcut */
            push(pc);
            pc = fl_sub;
        }
        else
        {
            /* programmable shortcuts from table */
            type16 ptr = (byte1 & 7) << 8 | byte2;
#ifdef LOGEMU
            out("LINK: %.2X,%.2X", byte1, byte2);
#endif
            if (ptr >= fl_size)
            {
                if ((byte1 & 8) == 0)
                    push(pc);
                ptr = byte1 << 8 | byte2 | 0x0800;
                ptr = fl_tab + 2 * (ptr ^ 0xffff);
                pc = (type32) ptr + (type16s) read_w(effective(ptr));
            }
            else
            {
                push(pc);
                pc = fl_sub;
            }
        }
        break;

    default:
        ms_fatal("Constants aren't and variables don't");
        break;
    }

#ifdef LOGEMU
    elapsed_time = frame_counter - start_time;
    if (elapsed_time > 0)
    {
        out("\t(time: %lu)", elapsed_time);
    }
    out("\n");
#endif

    return running;
}
