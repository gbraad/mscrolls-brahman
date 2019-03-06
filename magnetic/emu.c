/****************************************************************************\
 *
 * Magnetic - Magnetic Scrolls Interpreter.
 *
 * Written by Niclas Karlsson <nkarlsso@abo.fi>,
 *            David Kinder <davidk.kinder@virgin.net>,
 *            Stefan Meier <Stefan.Meier@if-legends.org> and
 *            Paul David Doherty <pdd@if-legends.org>
 *
 * Copyright (C) 1997-2008  Niclas Karlsson
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 * History:
 *
 * 0.1  123096 : Making the 68000 emulation work. Converting C64 6510 code to
 *     -010297   C isn't all that much fun. Don't mention the word "BUG".
 *
 * 0.2  010397 : Debugging, debugging, debugging... Probably quite a lot left
 *               as well. Haven't tried it out in practice yet.
 *
 * 0.3  010497 : String decoding now works. Trying out on the real thing - more
 *               bugs, modest success. Implementing some very specialized
 *               instructions. 0xA0FF gives the word nightmare a whole new
 *               meaning.
 *
 * 0.4  010597 : Getting better... More specialized instructions coming up. The
 *               output routines are up and running as well. Occasional
 *               emulation bugs still show up due to my MS-style program
 *               developing techniques ;-) First playable version [The Pawn].
 *
 * 0.5  010697 : Trying out with Jinxter. Sigh... Back to reverse engineering
 *               (10+ new instructions, separate dictionary and two string
 *               banks - these guys sure had some space problems).
 *
 * 0.6  010997 : Guild of Thieves seems to work now as well. Jinxter still has
 *               a small problem with the parsing (I know of _one_ different
 *               reaction) - probably due to the not-so-nice dict_lookup2().
 *               Load/Save implemented.
 *
 * 0.7  011097 : The last (major) bug is now fixed (an 8-bit variable instead
 *               of a 16-bit one messed up the adjectives). Small cleanup.
 *
 * 0.8  011397 : Now also runs Fish and Corruption. Small status-bar fix.
 *
 * 0.9  011497 : New single-file format. Improved user friendliness.
 *
 * 0.91 011597 : More cleaning. Random number range fixed. Other minor bugfixes.
 *
 * 0.92 011897 : Major dict_lookup() overhaul. Minor cleanups. <= 64K memory
 *               blocks. Seems like the file format wasn't that great after all
 *               (most suitable for one big block).
 *
 * 0.93 012097 : New file format. Fast restarts (no loading). UNDO implemented.
 *               Subtle but fatal bug in Write_string() removed [Corruption].
 *
 * 0.94 012397 : Emulation bug (rotations) fixed. Prng slightly improved, or
 *               at least changed. :-) Admode $xx(PC,[A|D]x) implemented.
 *
 * 0.95 012597 : Another one bites the dust - bit operations with register
 *               source were broken. __MSDOS__ ifdef inserted (changes by
 *               David Kinder). Error reporting improved (initiative by
 *               Paul David Doherty).
 *
 * 0.96 012697 : No flag changes after MOVE xx,Ax (version>1) - Duhh...
 *
 * 0.97 020497 : Small output handling bug (0x7E-handling) for version>=3
 *
 * 0.98 040497 : Pain, agony... Another bit operation bug spotted and removed.
 *               A difference between version 0 and the rest (MOVEM.W) caused
 *               problems with Myth. Also, findproperties behaved badly with
 *               bit 14 set (version 3 only?). dict_lookup() didn't recognise
 *               composite words properly.
 *
 * 0.99 041697 : ADDQ/SUBQ xx,Ax doesn't set flags for (version>=3) [corruption]
 *               Small dict_lookup() fix (must be checked thoroughly once more).
 *               Difference between versions in findproperties caused problems
 *               with Jinxter. [light match/unicorn]. Integrated gfx-handling.
 *
 * 0.9A 050397 : = instead of == caused problems. Sign error in do_findprop()
 *               Stupid A0F1 quirk removed. SAVEMEM flag added.
 *
 * 0.9B 050997 : Small ms_showpic() modification.
 *
 * 0.9C 051297 : Fixes for running Magnetic Windows games.
 *
 * 0.9D 080297 : Minor improvements to save some memory. Bug fix: Last picture
 *               couldn't be extracted when SAVEMEM was set. Use time() to get
 *               initial random seed.
 *
 * 1.0  082897 : First public release.
 *
 * 2.0  081599 : [DK] Fixed Wonderland bug: ADDA and SUBA don't change
 *               the condition codes.
 *               [DK] Changed the A0EF line A opcode to do nothing, rather
 *               than exit the interpreter. This opcode is used by the
 *               PRINTER command available in some games.
 *
 * 2.0  082799 : [DK] Merged in Stefan Jokisch's MS-DOS changes.
 *             : [DK] Changed MS-DOS ifdefs to also depend on __BORLANDC__.
 *
 * 2.0  092699 : [DK] ADDA and SUBA always operate on the entire address
 *             : register, rather than using the opcode size.
 *
 * 2.0  101699 : [DK] Changed handling of string buffer to cope with
 *             : MS-DOS limitations.
 *
 * 2.0  102399 : [DK] Fixed spurious capitalization and leading white
 *               space problems.
 *
 * 2.0  102499 : [DK] Hopefully fixed problem in dict_lookup() with
 *               dictionary words ending with a space character.
 *
 * 2.0  102699 : [SM] Added code to line_a DF for displaying pictures
 *               in version 4 games. Also modified line_a E1 (readline)
 *               for additional security check to avoid false DF actions
 *
 * 2.0  102799 : [SM] Changed line_a DF for game locations where the
 *               picture request is not the first DF after keyboard input.
 *
 * 2.0  102999 : [SM] Another slight modification of DF for Collection games
 *               compatibility
 *
 * 2.0  103099 : [DK] Fixed "#undo" for version 4 games.
 *
 * 2.0  110299 : [SM] One more fix to DF code for image indexing
 *
 * 2.0  110799 : [DK] Disabled calls to ms_statuschar() for version 4 games.
 *               Disabled printing of spurious '@' characters to the status
 *               bar (Spectrum Pawn 2.3)
 *
 * 2.0  110999 : [DK] Added an extra allowed return code to ms_getchar()
 *               of 1, which means that the calling opcode should be
 *               terminated immediately. This is needed to prevent possible
 *               corruption of the game's memory in interpreters which
 *               allow a new game to be loaded without restarting
 *               (e.g. the Windows version).
 *
 * 2.0  112099 : [DK] Allowed all games to have a code segment greater than
 *               65536 bytes. This is needed for Acorn Jinxter 1.3 to run.
 *
 * 2.0  112199 : [NK] Fixed dictionary code for Guild of Thieves 1.3.
 *
 * 2.0  112199 : [PDD] Made whether or not a game has a separate dictionary
 *               independant of the version number.
 *
 * 2.0  121399 : [DK] Changed fix for spurious capitalization to apply to
 *               all game versions.
 *
 * 2.0  011500 : [DK] Added support for a new graphics format for the
 *               Magnetic Windows games.
 *
 * 2.0  012900 : [DK] Fixed a bug in ms_extract2 and tidied up ms_showpic
 *               and ms_extract.
 *
 * 2.0  020500 : [DK] For v4, ms_showpic() is only called if pic_index > 0.
 *
 * 2.0  021100 : [DK] Updated #define for MSDOS to include lfopen().
 *
 * 2.0  022700 : [DK] A0E3 appears to be used to turn windows on and off.
 *               If it is called with d1 == 0 then graphics are turned off.
 *               [DK] Fixed a problem with v4 graphics not showing after
 *               loading a new .mag file into the interpreter.
 *
 * 2.0  040900 : [DK] Completely changed the graphics format for v4 games.
 *
 * 2.0  041500 : [DK] ms_get_anim_frame() now returns a masking flag.
 *
 * 2.0  041700 : [SM] ms_get_anim_frame() now returns the frame mask. One bug
 *               removed that messed up the frame sequence.
 *
 * 2.0  042200 : [DK] More animation work.
 *
 * 2.0  050400 : [SM] Even more animation work.
 *
 * 2.0  050500 : [SM] Oh no, more animation work.
 *
 * 2.0  050700 : [DK] A little tidying up.
 *
 * 2.0  060300 : [DK] More tidying and adding type casts to prevent
 *               compiler warnings.
 *
 * 2.0  061800 : [DK] Merged in Stefan's latest animation changes.
 *
 * 2.0  070900 : [DK] Figured out what the final argument to the animation
 *               command 0x01 means, with the result that nearly all the
 *               animations in Wonderland play correctly.
 *
 * 2.0  072300 : [DK] Added an extra check to A0E3 to stop graphics being
 *               accidentally turned off in the Collection Vol. 1 games.
 *               Added animation command 0x00 (stop) for Corruption.
 *
 * 2.0  080600 : [SM] Minor change to A0DF code for preventing mysterious
 *               crash in MW-Fish!
 *
 * 2.0  081700 : [SM] Adjusted lower code offsets for A0DF, now the missing 
 *               Wonderland image comes up correctly.
 *
 * 2.0  081700 : [SM] Changed 03 animation command to act as "repeat comannd"
 *               with Collection games and "Stop" for Wonderland.
 *
 * 2.0  081700 : [SM] Changes to the 0x05 animation command for "outpal"
 *               picture.
 *
 * 2.0  082800 : [DK] Added ms_anim_is_repeating(), plus a few more minor
 *               corrections.
 *
 * 2.0  092300 : [DK] Added a workaround for problems with Wonderland's
 *               "catter" animation.
 *
 * 2.1  032002 : [SM] Added support for Magnetic Windows' online hints.
 *
 * 2.1  040202 : [SM] Added small A0DF workaround for non-supported functions.
 *
 * 2.1  042002 : [DK] Reset hint variables in ms_freemem().
 *
 * 2.2  020303 : [DK] Cleaned up hint code.
 *
 * 2.2  022303 : [DK] Added fallback hint support if ms_showhints() returns 0.
 *               [DK] Added argument to ms_getchar() to indicate if #undo
 *               should be translated.
 *
 * 2.2  022503 : [DK] Memory for hints is only allocated when needed.
 *
 * 2.2  031203 : [DK] Reduced the memory used for hints to about 45k, which
 *               seems to be small enough for MS-DOS.
 *
 * 2.3  080722 : [SM] Added support for Wonderland music scores. The scores 
 *               are identified by the accompanying picture name.
 *
 * 2.3  080723 : [SM] Added #sound control command to toggle sound support 
 *               on interpreter level
 *
 * 2.3  080725 : [SM] Added isSoundEnabled to check sound status on game level
 *
 * 2.3  080804 : [SM] line_A DF code completely rewritten. SoundEnabled,
 *               basetable and gfx_table removed 
 *
 * 2.3  080811 : [DK] Changed prototype for ms_sndextract and removed
 *               memory leaks.
 *
 * 2.3  080811 : [SM] line_A DF rewrite broke hints support. Fixed.
 *
 * 2.3  080812 : [DK] Changed prototype for ms_playmusic and removed the
 *               need for ms_sndextract as an externally visible function.
 *
\****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>
#include "defs.h"
#include "hunks.h"
#include "cutils.h"
#include "roomsjinxter.h"

// picture colour profiles
#include "pccp.h"

#if defined(__MSDOS__) && defined(__BORLANDC__)

#include <alloc.h>

#define fread(p,s,n,f) lfread(p,s,n,f)
#define fopen(f,m)     lfopen(f,m)
#define malloc(s)      farmalloc(s)
#define free(p)        farfree(p)

extern long lfread(void far *, long, long, FILE far *);
extern FILE far * lfopen(const char far *, const char far *);

#endif

static type32 dreg[8], areg[8], i_count, string_size, pc, arg1i, mem_size;
static type16 properties, fl_sub, fl_tab, fl_size, fp_tab, fp_size;
static type8 zflag, nflag, cflag, vflag, byte1, byte2, regnr, admode, opsize;
static type8 *arg1, *arg2, is_reversible, running = 0, tmparg[4] = {0, 0, 0, 0};
static type8 lastchar = 0, version = 0, sd = 0, gameid = 0x00;
static type8 *decode_table, *restart = 0, *code = 0, *string = 0, *string2 = 0;
static type8 *string3 = 0, *dict = 0;
static type8 quick_flag = 0, gfx_ver = 0, *gfx_buf = 0, *gfx_data = 0;
static type8 *gfx2_hdr = 0, *gfx2_buf = 0;
static type8s *gfx2_name = 0;
static type16 gfx2_hsize = 0;
static FILE *gfx_fp = 0;
static type8 *snd_buf = 0, *snd_hdr = 0;
static type16 snd_hsize = 0;
static FILE *snd_fp = 0;
static type8 big = 0, period = 0, epipe = 0, escape = 0;
static MagHeader ms_head;
static int sectionCount;
type8 prog_format = 0;
static int getCharCount;
static int ScnMsgCount;

// prog format only
static int MsgMSGBASE;
static int MsgSCNBASE; 

Item* items;
size_t itemCount;
Word* adjectives;
size_t adjectiveCount;
size_t maxRoom;
size_t bytesPerRoom;
size_t npcCount;

const type8s undo_ok[] = "\n[Previous turn undone.]";
const type8s undo_fail[] = "\n[You can't undo what hasn't been done!]";
static type32 undo_regs[2][18], undo_pc, undo_size;
static type8 *undo[2] = {0, 0}, undo_stat[2] = {0, 0};
static type16 gfxtable = 0, table_dist = 0;
static type16 v4_id = 0, next_table = 1;

struct picture
{
    type8 * data;
    type32 data_size;
    type16 width;
    type16 height;
    type16 wbytes;
    type16 plane_step;
    type16 mask_wbytes;
    type16 transp;
    type8* mask;
    
};

#ifndef NO_ANIMATION

struct lookup
{
    type16s flag;
    type16s count;
};

#define MAX_POSITIONS 20
#define MAX_ANIMS 200
#define MAX_FRAMES 20

struct picture anim_frame_table[MAX_ANIMS];
type16 pos_table_size = 0;
type16 pos_table_count[MAX_POSITIONS];
struct ms_position pos_table[MAX_POSITIONS][MAX_ANIMS];
type8 * command_table = 0;
type16s command_index = -1;
struct lookup anim_table[MAX_POSITIONS];
type16s pos_table_index = -1;
type16s pos_table_max = -1;
struct ms_position pos_array[MAX_FRAMES];
type8 anim_repeat = 0;

#endif

/* Hint support */
#define MAX_HINTS 260
#define MAX_HCONTENTS 30000
struct ms_hint* hints = 0;
type8* hint_contents = 0;
const type8s no_hints[] = "[Hints are not available.]\n";
const type8s not_supported[] = "[This function is not supported.]\n";

// define a type for LOGGING or DISASSEMBLY
#if defined(LOGEMU) || defined(DISASM)
#define LOGDIS
#endif

#if defined(LOGEMU) || defined(LOGGFX) || defined(LOGHNT) || defined(DISASM)
static FILE *dbg_log;
#ifndef LOG_FILE
#error LOG_FILE must be defined to be the name of the log file.
#endif
#endif

/* prototypes */
type32 read_reg(int, int);
void write_reg(int, int, type32);
static void do_findprop(void);

#define MAX_STRING_SIZE  0xFF00
#define MAX_PICTURE_SIZE 0xC800
#define MAX_MUSIC_SIZE   0x4E20

#ifdef LOGDIS
#define DASM_ADDR    0
#define DASM_OP      1
#define DASM_OPSIZE  2
#define DASM_ARG1    3
#define DASM_ARG2    4
#define DASM_COM     5
#define DASM_SIZE    6
#define DASM_SCODE   7
#define DASM_ACODE   8

void *disassem=0;
void *curr_ins=0;
type8 swapflag=0;
static int emit_log; 
static int hit_mem;
type8 arglooping=0;
type8 op_words=0;
type8 no_branch=0;
type8 nextjmp=1;
int num_unaddr=0;
int* unaddr;
char* dasmfile=0;
int jmpidx=0;
int contdasm=0;
int ins_addr=0;
#endif

extern void init_items(); // in emux
extern void init_syms(FILE*); // in emux
extern int get_sym_value(const char* s);

#ifdef STANDALONE
static void _dump_regs(FILE* fp, const char* s)
{
    int i;
    if (s) fprintf(fp, "%s:\n", s);
    for (i = 0; i < 8; ++i)
    {
        fprintf(fp, "D%d=%08X ", i, read_reg(i,3));
        if (!((i+1) & 3)) fprintf(fp, "\n");
    }
    for (i = 0; i < 8; ++i)
    {
        fprintf(fp, "A%d=%08X ", i, read_reg(8 + i, 3));
        if (!((i+1) & 3)) fprintf(fp, "\n");
    }
 }

static void dump_regs(const char* s)
{
    _dump_regs(stdout, s);
}

#endif

#ifdef LOGDIS
void out(char *format,...)
{
//    va_list a;

//    va_start(a, format);
//    vfprintf(dbg_log, format, a);
//    va_end(a);
}

static void push_msg_end()
{
    if (emit_log)
    {
        if (hit_mem)
        {
            fprintf(dbg_log, "\t*\n"); // show hits on log
            //_dump_regs(dbg_log, 0);
        }
        else fprintf(dbg_log, "\n");    
    }
}

static void push_msg_begin()
{
    hit_mem = 0; // counts mem hits throughout instruction
    if (emit_log)
        fprintf(dbg_log, "%.8X:", pc);
}

static void push_msg_flush()
{
    if (emit_log) fflush(dbg_log);
}

static void push_msg(int attr, char * format,...)
{
    char buffer[200]="";
    va_list a;
    va_start(a, format);
    vsprintf(buffer,format,a);
    va_end(a);
#ifdef LOGEMU
    if (emit_log) fprintf(dbg_log," %s",buffer);
#else
    if (!disassem) disassem=create_dasm();

    if (curr_ins)
    {
        switch(attr)
        {
        case DASM_ADDR:    ins_setaddr(curr_ins,atoi(buffer));break;
        case DASM_OP:      ins_setop(curr_ins,buffer);break;
        case DASM_OPSIZE:  ins_setopsize(curr_ins,buffer);break;
        case DASM_ARG1:    if (swapflag || op_words > 2)
                if (arglooping)
                    ins_setarg2(curr_ins,buffer);
                else
                    ins_appendarg2(curr_ins,buffer);
            else
                if (arglooping)
                    ins_setarg1(curr_ins,buffer);
                else
                    ins_appendarg1(curr_ins,buffer);
            break;
        case DASM_ARG2:    if (swapflag)
                if (arglooping)
                    ins_setarg1(curr_ins,buffer);
                else
                    ins_appendarg1(curr_ins,buffer);
            else
                if (arglooping)
                    ins_setarg2(curr_ins,buffer);
                else
                    ins_appendarg2(curr_ins,buffer);
            break;
        case DASM_COM:     ins_appendcomment(curr_ins,buffer);break;
        case DASM_SIZE:    ins_setsize(curr_ins,atoi(buffer));break;
        case DASM_SCODE:   ins_setcode(curr_ins,0,buffer);break;
        case DASM_ACODE:   ins_setcode(curr_ins,1,buffer);break;
        }
    }
#endif // LOGEMU
}
#endif // LOGEMU || DISASSEM

#if defined(LOGGFX) || defined(LOGHNT) || defined(LOGGFX_EXT)
void out2(char *format,...)
{
    va_list a;

    va_start(a, format);
    //vaprintf(dbg_log, format, a);
    printf(format, a);

    va_end(a);
    //fflush(dbg_log);
}
#endif

typedef struct
{
    int         room;
    const char* sound;
} RoomSound;

#if 0
static const RoomSound jinxterAmbience[] =
{
    //{ JRONBUS, "onbus.ogg" },
    { JRFRONTGARDEN1, "garden.ogg" },
};

static const RoomSound* findSound(int room, const RoomSound* table, int size)
{
    while (size--)
    {
        if (table->room == room) return table;
        ++table;
    }
    return 0;
}

static void _ms_playsound(int room)
{
    int g = get_game();
    const RoomSound* rs = 0;
    
    switch (g)
    {
    case 3: // jinxter
        rs = findSound(room, jinxterAmbience, ASIZE(jinxterAmbience));
        break;
    }

    ms_playsound(room, rs ? rs->sound : 0);

}
#endif

static void _ms_showpic(type32 c, type32 picAddr, int mode, int picVer)
{
    // shader values: Contrast Brightness Saturation Lightness Gamma
    float* profile = 0;

    int g = get_game();
    if (g > 0)
    {
#if 1
        assert(c >= 0 && c < MAX_PICS);
        if (mode == 1)
        {
            profile = pcc[g-1][c];
        }
#else
        if (gfx_ver < 2)
        {
            profile = pcc[g-1][c];
        }
        else
        {
            // in the original releases, all image names were lowercase with the
            // exception of static alternatives for animated images which were all
            // in uppercase
            // the original collection game had all guild pictures start with 'g'
            // all corruption images start with 'c' and the fish! images start with 'f'
            // wonderland names were not prefixed
            // for the remastered versions we use the image name until the '.', convert
            // it to lowercase, prepend the necessary character and cut after 6 chars
            char picname[7];
            if (g==2) picname[0]='g';
            else if (g==4) picname[0]='c';
            else if (g==5) picname[0]='f';

            const char* p = (const char*)getcode() + c;
            char* q = picname+1;
            while (*p && (q-picname < 6))
            {
                //printf("Distance: %ld\n",q-picname);
                if (*p == '.') break;
                *q++ = u_tolower(*p);
                ++p;
            }
            *q = 0;
            //printf("Searching index of %s#\n",picname);
            type16s header_pos=0;
            while (header_pos < gfx2_hsize)
            {
                type8s* hname = (type8s*)(gfx2_hdr + header_pos);

                if (u_strnicmp(hname,picname,6) == 0)
                   break;

                header_pos += 16;
            }
            int idx = header_pos/16;
            //printf("Calculated index: %d\n",idx);
            profile = pcc[g-1][idx];
        }
#endif        

        // to guard against versions not yet set, look for zero gamma
        if (profile)
        {
            float gamma = profile[4];
            if (gamma == 0.0) profile = 0;
        }
    }
    ms_showpic(c, picAddr, mode, gfx_ver, profile, picVer);
}

static type8 _ms_getchar(type8 trans)
{
    type8 c = ms_getchar(trans);
    if (c > 1) ++getCharCount;
    return c;
}

/* Convert virtual pointer to effective pointer */
type8 *effective(type32 ptr)
{
    if ((version < 4) && (mem_size == 0x10000))
        return &(code[ptr & 0xffff]);
    if (ptr >= mem_size)
    {
        char buf[256];
        sprintf(buf, "outside memory experience %08X", (int)ptr);
        ms_fatal(buf);
        return code;
    }
    return code + ptr;
}

#ifdef _MSC_VER
// MSVC does not like "inline" in C programs
#define INLINE
#else
#define INLINE inline
#endif

static INLINE type32 _read_l(type8 * ptr)
{
    return (type32) ((type32) ptr[0] << 24 | (type32) ptr[1] << 16 | (type32) ptr[2] << 8 | (type32) ptr[3]);
}

static INLINE type16 _read_w(type8 * ptr)
{
    return (type16) (ptr[0] << 8 | ptr[1]);
}

static INLINE type8 _read_b(type8* ptr)
{
    return *ptr;
}

static INLINE void _write_l(type8 * ptr, type32 val)
{
    ptr[3] = (type8) val;
    val >>= 8;
    ptr[2] = (type8) val;
    val >>= 8;
    ptr[1] = (type8) val;
    val >>= 8;
    ptr[0] = (type8) val;
}

static INLINE void _write_w(type8 * ptr, type16 val)
{
    ptr[1] = (type8) val;
    val >>= 8;
    ptr[0] = (type8) val;
}

static INLINE void _write_b(type8* ptr, type8 val)
{
    *ptr = val;
}

#ifdef LOGDIS
// define wrappers around implementation so we can test memory access

static void access_mem(type8* p, int sz)
{
    int s = p - code;
    int e = (p + sz) - code;
    
    // put your debugging memory access check here!!
    //if (s >= 0x4cb8 && e <= 0x4cc8)
    //    printf("%.4X - > %.4X\n",pc,s); // save game region

    // example:
    //if (s >= 0x1910 && e < 0x3599) ++hit_mem; // save game region
}

static type32 read_l(type8* ptr)
{
    access_mem(ptr, 4);
    return _read_l(ptr);
}

static type16 read_w(type8* ptr)
{
    access_mem(ptr, 2);
    return _read_w(ptr);
}

static type8 read_b(type8* ptr)
{
    access_mem(ptr, 1);
    return _read_b(ptr);
}

static void write_l(type8* ptr, type32 val)
{
    access_mem(ptr, 4);
    _write_l(ptr, val);
}

static void write_w(type8* ptr, type16 val)
{
    access_mem(ptr, 2);
    _write_w(ptr, val);
}

static void write_b(type8* ptr, type8 val)
{
    access_mem(ptr, 1);
    _write_b(ptr, val);
}


#else
// map to implementation
#define read_l _read_l
#define read_w _read_w
#define read_b _read_b
#define write_l _write_l
#define write_w _write_w
#define write_b _write_b
#endif

static INLINE type32 read_l2(type8 * ptr)
{
    return ((type32) ptr[1] << 24 | (type32) ptr[0] << 16 | (type32) ptr[3] << 8 | (type32) ptr[2]);
}

static INLINE type16 read_w2(type8 * ptr)
{
    return (type16) (ptr[1] << 8 | ptr[0]);
}

/* Standard rand - for equal cross-platform behaviour */


#if 0
static type32 rseed;
void ms_seed(type32 seed)
{
    rseed = seed;
}

/* this doesn't work well with subsequent modulo, nor for a sequence
 * of random numbers. for example when r(4)==0, r(2) is never 0.
 *
 * see numerical recipes in C (2nd edition) page 276-277
 */
type32 rand_emu(void)
{
    rseed = 1103515245L * rseed + 12345L;
    return rseed & 0x7fffffffL;
}
#else

/* from numerical recipes (3rd edition) page 351 */
static uint64_t rand_val = 4101842887655102017LL;

void ms_seed(type32 seed)
{
    rand_val = seed;
}

static uint64_t rand_gen64()
{
    rand_val ^= rand_val >> 21;
    rand_val ^= rand_val << 35;
    rand_val ^= rand_val >> 4;
    return rand_val*2685821657736338717LL;
}

type32 rand_emu()
{
    uint32_t v = (uint32_t)rand_gen64();
    return v;
}


#endif

static void _freeWord(Word* word)
{
    free(word->name);
}

static void _freeWords(Word* words, size_t n)
{
    size_t i;
    for (i = 0; i < n; ++i) _freeWord(words + i);
    free(words);
}

static void _freeItems(Item* items, size_t n)
{
    size_t i;
    for (i = 0; i < n; ++i) _freeWord(&items[i].word);
    free(items);
}

void ms_freemem(void)
{
    if (code)
        free(code);
    if (string)
        free(string);
    if (string2)
        free(string2);
    if (string3)
        free(string3);
    if (dict)
        free(dict);
    if (undo[0])
        free(undo[0]);
    if (undo[1])
        free(undo[1]);
    if (restart)
        free(restart);
    code = string = string2 = string3 = dict = undo[0] = undo[1] = restart = 0;
    if (gfx_data)
        free(gfx_data);
    if (gfx_buf)
        free(gfx_buf);
    if (gfx2_hdr)
        free(gfx2_hdr);
    if (gfx2_buf)
        free(gfx2_buf);
    if (gfx_fp)
        fclose(gfx_fp);
    gfx_data = gfx_buf = gfx2_hdr = gfx2_buf = 0;
    gfx2_name = 0;
    gfx_fp = 0;
    gfx_ver = 0;
    gfxtable = table_dist = 0;
#ifndef NO_ANIMATION
    pos_table_size = 0;
    command_index = 0;
    anim_repeat = 0;
    pos_table_index = -1;
    pos_table_max = -1;
#endif
    lastchar = 0;
    if (hints)
        free(hints);
    if (hint_contents)
        free(hint_contents);
    hints = 0;
    hint_contents = 0;
    if (snd_hdr)
        free(snd_hdr);
    if (snd_buf)
        free(snd_buf);
    snd_hdr = 0;
    snd_hsize = 0;
    snd_buf = 0;

    _freeWords(adjectives, adjectiveCount);
    _freeItems(items, itemCount);
}

type8 ms_is_running(void)
{
    return running;
}

type8 ms_is_magwin(void)
{
    return (version == 4) ? 1 : 0;
}

void ms_stop(void)
{
    running = 0;
}

static int ms_restart()
{
    // non-zero means ok
    // do not call this directly from outside
    return ms_init(0,0,0,0);
}

static ms_hook sr_hook;
static void* sr_hook_ctx;

void set_stop_or_restart_hook(ms_hook hook, void* ctx)
{
    sr_hook = hook;
    sr_hook_ctx = ctx;
}

static void ms_stop_or_restart(int r)
{
    if (sr_hook)
    {
        // allow external hook to change behaviour
        // first call as request
        r = (*sr_hook)(r, sr_hook_ctx, 1);
    }
    
    if (r)
    {
        // restart, fall through on fail
        r = ms_restart();
    }

    if (!r)
    {
        // stop
        ms_stop();
    }

    if (sr_hook)
    {
        // call again after not as request
        (*sr_hook)(r, sr_hook_ctx, 0);
    }
}

void ms_restart_request()
{
    // call from outside to initiate restart
    ms_stop_or_restart(1);
}

type8 init_gfx1(type8* header)
{
#ifdef SAVEMEM
    type32 i;
#endif

    if (!(gfx_buf = malloc(MAX_PICTURE_SIZE)))
    {
        fclose(gfx_fp);
        gfx_fp = 0;
        return 1;
    }
#ifdef SAVEMEM
    if (!(gfx_data = malloc(128)))
#else
    if (!(gfx_data = malloc(read_l(header + 4) - 8)))
#endif
    {
        free(gfx_buf);
        fclose(gfx_fp);
        gfx_buf = 0;
        gfx_fp = 0;
        return 1;
    }
#ifdef SAVEMEM
    if (!fread(gfx_data, 128, 1, gfx_fp))
#else
    if (!fread(gfx_data, read_l(header + 4) - 8, 1, gfx_fp))
#endif
    {
        free(gfx_data);
        free(gfx_buf);
        fclose(gfx_fp);
        gfx_data = gfx_buf = 0;
        gfx_fp = 0;
        return 1;
    }
#ifdef SAVEMEM
    for (i = 0; i < 128; i += 4)
        if (!read_l(gfx_data + i))
            write_l(gfx_data + i, read_l(header + 4));
#else
    fclose(gfx_fp);
    gfx_fp = 0;
#endif
    gfx_ver = 1;
    return 2;
}

type8 init_gfx2(type8* header)
{
    if (!(gfx_buf = malloc(MAX_PICTURE_SIZE)))
    {
        fclose(gfx_fp);
        gfx_fp = 0;
        return 1;
    }

    gfx2_hsize = read_w(header + 4);
    if (!(gfx2_hdr = malloc(gfx2_hsize)))
    {
        free(gfx_buf);
        fclose(gfx_fp);
        gfx_buf = 0;
        gfx_fp = 0;
        return 1;
    }

    fseek(gfx_fp, 6, SEEK_SET);
    if (!fread(gfx2_hdr, gfx2_hsize, 1, gfx_fp))
    {
        free(gfx_buf);
        free(gfx2_hdr);
        fclose(gfx_fp);
        gfx_buf = 0;
        gfx2_hdr = 0;
        gfx_fp = 0;
        return 1;
    }

    gfx_ver = 2;
    return 2;
}

type8 init_snd(type8* header)
{
    if (!(snd_buf = malloc(MAX_MUSIC_SIZE)))
    {
        fclose(snd_fp);
        snd_fp = 0;
        return 1;
    }

    snd_hsize = read_w(header + 4);
    if (!(snd_hdr = malloc(snd_hsize)))
    {
        free(snd_buf);
        fclose(snd_fp);
        snd_buf = 0;
        snd_fp = 0;
        return 1;
    }

    fseek(snd_fp, 6, SEEK_SET);
    if (!fread(snd_hdr, snd_hsize, 1, snd_fp))
    {
        free(snd_buf);
        free(snd_hdr);
        fclose(snd_fp);
        snd_buf = 0;
        snd_hdr = 0;
        snd_fp = 0;
        return 1;
    }

    return 2;
}

static int get_word(FILE* fp)
{
    int a = getc(fp);
    int b = getc(fp);
    return (a << 8) | b;
}

static int get_int(FILE* fp)
{
    int a = get_word(fp);
    int b = get_word(fp);
    return (a << 16) | b;
}

static int ms_load_prog_header(const char* name, FILE* fp)
{
    /* format of a very simple AmigaDOS load file:

       hunk_header L
       0 L
       1 L (n sections, 2=>BSS, but none present)
       0 L
       0 L
       size/4
       hunk_code
       size of code/4 (also size_of_code == size)
       the code (size_of_code longwords)

       hunk_reloc32
       [n then n longwords of reloc 32] repeats until n == 0

       hunk_symbol

       [sym_len/4 sym_len bytes (sym name) value.L ] repeat until sym_len == 0
       hunk_end
    */

    int n = get_int(fp); // expect 0
    assert(n == 0);
    n = get_int(fp); // sections#

    if (n == 0)
    {
        printf("No sections!\n");
        return 0;
    }

    if (n > 3)
    {
        printf("too many sections, %d\n", n);
        return 0;
    }

    sectionCount = n;

    int t = get_int(fp);
    assert(t == 0);
    t = get_int(fp);
    assert(t == n-1);

    int sizes[3];

    memset(sizes, 0, sizeof(sizes));
    int i=0;
    for (i = 0; i < n; ++i) sizes[i] = get_int(fp);

    ms_head.code_size = sizes[0]*4;

    int hc = get_int(fp);
    if (hc != hunk_code)
    {
        printf("expected code section\n");
        return 0;
    }

    int sz = get_int(fp)*4; // code_size/4
    if (sz != ms_head.code_size)
    {
        printf("size wrong in header\n");
        return 0;
    }

    ms_head.version = 2; // remastered
    ms_head.gameid = 0; // unknown
    ms_head.dict_size = sizes[1]*4;

    // final file name in path (if given)
    const char* p = strrchr(name, '/');
    if (!p) p = strrchr(name, '\\');
    if (p) ++p;
    else p = name;

    // XX not very clever
    if (!strncmp(p, "guild", 5)) ms_head.gameid = 0x20;
    if (!strncmp(p, "pawn", 4)) ms_head.gameid = 0x10;
    if (!strncmp(p, "jinxter", 7)) ms_head.gameid = 0x30;

    //if (!ms_head.gameid) fprintf(stderr, "load_prog, unknown game for file '%s'\n", name);

    // following is code_size bytes of code

    return 1; // ok
}

static INLINE void fix_long(type32* p)
{
    // reverse long work in place
    *p = _read_l((type8*)p);
}


/* zero all registers and flags and load the game */
type8 ms_init(type8s * name, type8s * gfxname, type8s * hntname, type8s * sndname)
{
    FILE *fp;
    type8 header2[8],header3[4];
    type32 i;

    assert(sizeof(ms_head) == 42);
    assert(sizeof(type8) == 1);
    assert(sizeof(type16) == 2);
    assert(sizeof(type32) == 4);

#if defined(LOGEMU) || defined(LOGGFX) || defined(LOGHNT)
    // we can emit log to stderr. 
    // but we don't set `emit_log` here. this can be done
    // manually (eg debugging)
    if (!strcmp(LOG_FILE, "stderr")) dbg_log = stderr;
    else if (!dbg_log) // eg restart
    {
        dbg_log = fopen(LOG_FILE, "wt");
        emit_log = 1; // always when to file
    }
#endif
    ms_stop();

    if (!name)
    {
        if (!restart)
            return 0;
        else
        {
            // restore game state from restart buffer
            memcpy(code, restart, undo_size);
            undo_stat[0] = undo_stat[1] = 0;
            _ms_showpic(0, 0, 0, 0);
        }
    }
    else
    {
        ms_freemem();

        // try to load the external message file.
        // if this fails, we fallback to using the original game messages
        undo_stat[0] = undo_stat[1] = 0;
        ms_seed(time(0));
        if (!(fp = fopen(name, "rb")))
            return 0;

        int ok;
                
        // initially just read the magic number,
        ok = fread(&ms_head.magic, 1, 4, fp) == 4;

        if (!ok)
        {
            printf("can't read '%s' file\n", name);
            fclose(fp);
            return 0;
        }

        fix_long(&ms_head.magic);
                 
        if (ms_head.magic == hunk_header) 
        {
            // AmigaDOS prog file
            prog_format = 1;
            ok = ms_load_prog_header(name, fp);
        }
        else
        {
            if (ms_head.magic != 0x4d615363)
            {
                printf("bad magic number, %08X\n", ms_head.magic);
                fclose(fp);
                return 0;
            }

            ok = ok && (fread((type8*)&ms_head + 4,
                              1, sizeof(ms_head) - 4, fp) == sizeof(ms_head)-4);

            // fix header
            fix_long(&ms_head.code_size);
            fix_long(&ms_head.string_size);
            fix_long(&ms_head.string2_size);
            fix_long(&ms_head.dict_size);
            fix_long(&ms_head.undo_size);
            fix_long(&ms_head.undo_pc);
            fix_long(&ms_head.dec);
        }

        if (!ok)
        {
            printf("can't read header\n");
            fclose(fp);
            return 0;
        }

        if (prog_format)
        {
            mem_size = ms_head.code_size;
            if (mem_size < 128000) mem_size = 128000;
        }
        else
        {
            if (version < 4 && ms_head.code_size < 65536)
                mem_size = 65536;
            else
            {
                if (ms_head.code_size > 65536) mem_size = 128000;
                else mem_size = ms_head.code_size;
            }
        }
        
        ok = (code = malloc(mem_size)) != 0;
        ok = ok && fread(code, 1, ms_head.code_size, fp) == ms_head.code_size;

        sd = (type8)((ms_head.dict_size != 0L) ? 1 : 0); /* if (sd) => separate dict */
        
        if (sd)
            ok = ok && (dict = malloc(ms_head.dict_size)) != 0;

        if (prog_format)
        {
            int data_loaded = 0;
            
            while (ok)
            {
                int v = get_int(fp); // section type

                ok = 0;
                
                if (v == hunk_end)
                {
                    ok = 1;
                    if (!--sectionCount) break;
                }
                else if (v == hunk_reloc32)
                {
                    ok = 1;
                    
                    // skip reloc
                    for (;;)
                    {
                        int n = get_int(fp);
                        if (n <= 0) break;
                        get_int(fp); // hn
                        while (n--) get_int(fp);
                    }
                }
                else if (v == hunk_data)
                {
                    int n = get_int(fp)*4; // size
                    ok = n == ms_head.dict_size;
                    if (ok)
                    {
                        ok = fread(dict, 1, ms_head.dict_size, fp) == ms_head.dict_size;
                        data_loaded = ok;
                    }
                    else printf("dictionary size mismatch\n");
                }
                else if (v == hunk_symbol)
                {
                    // load symbol table if present
                    init_syms(fp);

                    //int save_start = get_sym_value("SLADDR");
                    int save_end = get_sym_value("SLADDR.E");

                    //printf("save_start = 0x%x\n", save_start);
                    //printf("save_end = 0x%x\n", save_end);
                    ms_head.undo_size = save_end;
                    
                    ok = 1;
                }
                else
                {
                    printf("unknown section in prog 0x%x\n", v);
                }
            }

            if (ok && sd && !data_loaded)
            {
                printf("Did not load dictionary data from prog\n");
                ok = 0;
            }
        }
        
        gameid = ms_head.gameid;
        version = ms_head.version;
        string_size = ms_head.string_size;
        undo_size = ms_head.undo_size;
        undo_pc = ms_head.undo_pc;

#if 0
        printf("gameid = 0x%x\n", gameid);
        printf("version = %d\n", version);
        printf("code_size 0x%x\n", ms_head.code_size);
        printf("string size 0x%x\n", string_size);
        printf("string2 size 0x%x\n", ms_head.string2_size);
        printf("dict size = 0x%x\n", ms_head.dict_size);
        printf("undo size = 0x%x\n", undo_size);
        printf("undo pc = 0x%x\n", undo_pc);
        printf("dec = 0x%x\n", ms_head.dec);
        printf("mem size = 0x%x\n", mem_size);
#endif

        if (ms_head.string2_size)
            ok = ok && (string2 = malloc(ms_head.string2_size)) != 0;

        if (undo_size)
        {
            ok = ok && (restart = malloc(undo_size)) != 0;

            if (!prog_format)
            {
                ok = ok && (undo[0] = malloc(undo_size)) != 0;
                ok = ok && (undo[1] = malloc(undo_size)) != 0;
            }
            
            if (ok)
            {
                //printf("enable undo restart\n");
                memcpy(restart, code, undo_size);
            }
        }
        

        if (string_size)
        {
            ok = ok && (string = malloc(MAX_STRING_SIZE)) != 0;
            
            if (string_size > MAX_STRING_SIZE)
            {
                ok = ok && (string3 = malloc(string_size - MAX_STRING_SIZE)) != 0;
            }
        }
        
        if (string_size > MAX_STRING_SIZE)
        {
            ok = ok && fread(string, 1, MAX_STRING_SIZE, fp) == MAX_STRING_SIZE;

            ok = ok && (fread(string3, 1, string_size - MAX_STRING_SIZE, fp)
                        == string_size - MAX_STRING_SIZE);
        }
        else
        {
            ok = ok && fread(string, 1, string_size, fp) == string_size;
        }

        ok = ok && (fread(string2, 1, ms_head.string2_size, fp) == ms_head.string2_size);

        if (sd && ok)
        {
            if (!prog_format)
            {
                ok = fread(dict, 1, ms_head.dict_size, fp) == ms_head.dict_size;
            }
        }

        fclose(fp);

        if (!ok)
        {
            printf("error reading '%s'\n", name);
            ms_freemem();
            return 0;
        }
        
        if (ms_head.dec && ms_head.dec >= string_size)
        {
            assert(string2);
            decode_table = string2 + ms_head.dec - string_size;
        }
        else
        {
            if (ms_head.dec >= MAX_STRING_SIZE)
            {
                assert(string3);
                decode_table = string3 + ms_head.dec - MAX_STRING_SIZE;
            }
            else
            {
                if (string)
                    decode_table = string + ms_head.dec;
            }
        }
    }

    for (i = 0; i < 8; i++) dreg[i] = areg[i] = 0;

    int stack_ptr;
    if (mem_size > 0x10000)
    {
        stack_ptr = 0x1f400; // 128k
    }
    else
    {
        stack_ptr = 0x10000-2;
    }

    /* Stack-pointer, -2 due to MS-DOS segments */
    //printf("setting stack to 0x%08X\n", stack_ptr);
    write_reg(8 + 7, 4, stack_ptr);
    
    pc = 0;
    zflag = nflag = cflag = vflag = 0;
    i_count = 0;
    running = 1;

    if (!name)
        return (type8) (gfx_buf ? 2 : 1);       /* Restarted */

    if (version == 4)
    {
        /* Try loading a hint file */
        FILE *hnt_fp;
        if (hntname && (hnt_fp = fopen(hntname, "rb")))
        {
            if ((fread(&header3, 1, 4, hnt_fp) == 4) && (read_l(header3) == 0x4D614874))
            {
                type8 buf[8];
                type16 i,j,blkcnt,elcnt,ntype,elsize,conidx;

                /* Allocate memory for hints */
                hints = malloc(MAX_HINTS * sizeof(struct ms_hint));
                hint_contents = malloc(MAX_HCONTENTS);
                if ((hints != 0) && (hint_contents != 0))
                {
                    /* Read number of blocks */
                    fread(&buf, 1, 2, hnt_fp);
                    blkcnt = read_w2(buf);
#ifdef LOGHNT
                    out2("Blocks: %d\n",blkcnt);
#endif
                    conidx = 0;
                    for (i = 0; i < blkcnt; i++)
                    {
#ifdef LOGHNT
                        out2("\nBlock No. %d\n",i);
#endif
                        /* Read number of elements */
                        fread(&buf, 1, 2, hnt_fp);
                        elcnt = read_w2(buf);
#ifdef LOGHNT
                        out2("Elements: %d\n",elcnt);
#endif
                        hints[i].elcount = elcnt;

                        /* Read node type */
                        fread(&buf, 1, 2, hnt_fp);
                        ntype = read_w2(buf);
#ifdef LOGHNT
                        if (ntype == 1)
                            out2("Type: Node\n");
                        else
                            out2("Type: Leaf\n");
#endif
                        hints[i].nodetype = ntype;
                        hints[i].content = hint_contents+conidx;
#ifdef LOGHNT
                        out2("Elements:\n");
#endif
                        for (j = 0; j < elcnt; j++)
                        {
                            fread(&buf, 1, 2, hnt_fp);
                            elsize = read_w2(buf);
                            fread(hint_contents+conidx, 1, elsize, hnt_fp);
                            hint_contents[conidx+elsize-1] = '\0';
#ifdef LOGHNT
                            out2("%s\n",hint_contents+conidx);
#endif
                            conidx += elsize;
                        }

                        /* Do we need a jump table? */
                        if (ntype == 1)
                        {
#ifdef LOGHNT
                            out2("Jump to block:\n");
#endif
                            for (j = 0; j < elcnt; j++)
                            {
                                fread(&buf, 1, 2, hnt_fp);
                                hints[i].links[j] = read_w2(buf);
#ifdef LOGHNT
                                out2("%d\n",hints[i].links[j]);
#endif
                            }
                        }

                        /* Read the parent block */
                        fread(&buf, 1, 2, hnt_fp);
                        hints[i].parent = read_w2(buf);
#ifdef LOGHNT
                        out2("Parent: %d\n",hints[i].parent);
#endif
                    }
                }
                else
                {
                    if (hints)
                        free(hints);
                    if (hint_contents)
                        free(hint_contents);
                    hints = 0;
                    hint_contents = 0;
                }
            }
            fclose(hnt_fp);
        }

        /* Try loading a music file */
        if (sndname && (snd_fp = fopen(sndname, "rb")))
        {
            if (fread(&header2, 1, 8, snd_fp) != 8)
            {
                fclose(snd_fp);
                snd_fp = 0;
            }
            else
            {
                if (read_l(header2) == 0x4D615364) /* MaSd */
                {
                    init_snd(header2);
#ifdef LOGSND
                    out2("Sound file loaded.\n");
#endif
                }
            }
        }
    }

    if (!gfxname || !(gfx_fp = fopen(gfxname, "rb")))
        return 1;
    if (fread(&header2, 1, 8, gfx_fp) != 8)
    {
        fclose(gfx_fp);
        gfx_fp = 0;
        return 1;
    }

    if (version < 4 && read_l(header2) == 0x4D615069) /* MaPi */
        return init_gfx1(header2);
    else if (read_l(header2) == 0x4D615032) /* MaP2 */
        return init_gfx2(header2);
    fclose(gfx_fp);
    gfx_fp = 0;
    return 1;
}

type8 is_blank(type16 line, type16 width)
{
    type32s i;

    for (i = line * width; i < (line + 1) * width; i++)
        if (gfx_buf[i])
            return 0;
    return 1;
}

type8 *ms_extract1(type8 pic, type16 * w, type16 * h, type16 * pal)
{
    type8 *decode_table, *data, bit, val, *buffer;
    type16 tablesize, count;
    type32 i, j, upsize, offset;

    offset = read_l(gfx_data + 4 * pic);
    
#ifdef SAVEMEM
    type32 datasize;
    
    if (fseek(gfx_fp, offset, SEEK_SET) < 0)
        return 0;
    datasize = read_l(gfx_data + 4 * (pic + 1)) - offset;
    if (!(buffer = malloc(datasize)))
        return 0;
    if (fread(buffer, 1, datasize, gfx_fp) != datasize)
        return 0;
#else
    buffer = gfx_data + offset - 8;
#endif

    for (i = 0; i < 16; i++)
        pal[i] = read_w(buffer + 0x1c + 2 * i);
    w[0] = (type16)(read_w(buffer + 4) - read_w(buffer + 2));
    h[0] = read_w(buffer + 6);

    tablesize = read_w(buffer + 0x3c);
    //datasize = read_l(buffer + 0x3e);
    decode_table = buffer + 0x42;
    data = decode_table + tablesize * 2 + 2;
    upsize = h[0] * w[0];

    for (i = 0, j = 0, count = 0, val = 0, bit = 7; i < upsize; i++, count--)
    {
        if (!count)
        {
            count = tablesize;
            while (count < 0x80)
            {
                if (data[j] & (1 << bit))
                    count = decode_table[2 * count];
                else
                    count = decode_table[2 * count + 1];
                if (!bit)
                    j++;
                bit = (type8)(bit ? bit - 1 : 7);
            }
            count &= 0x7f;
            if (count >= 0x10)
                count -= 0x10;
            else
            {
                val = (type8)count;
                count = 1;
            }
        }
        gfx_buf[i] = val;
    }
    for (j = w[0]; j < upsize; j++)
        gfx_buf[j] ^= gfx_buf[j - w[0]];

#ifdef SAVEMEM
    free(buffer);
#endif
    for (; h[0] > 0 && is_blank((type16)(h[0] - 1), w[0]); h[0]--);
    for (i = 0; h[0] > 0 && is_blank((type16)i, w[0]); h[0]--, i++);
    return gfx_buf + i * w[0];
}

//#define DEBUG_NAMES

type16s find_name_in_header(type8s * name, type8 upper)
{
    type16s header_pos = 0;
    type8s pic_name[8];
    type8 i, j;

#ifdef DEBUG_NAMES
    int cc = 0;
#endif

    j = 0;

    // XX HACK
    // the image names all start with a G inside the gfx file
    if (upper)
        pic_name[j++] = 'G';
    else
        pic_name[j++] = 'g';


    
    for (i = 0; i < 6; ++i)
    {
        char c = name[i];
        if (!c || c == '.') break;
        if (upper) c = u_toupper(c);
        else c= u_tolower(c);
        pic_name[j++] = c;
    }
    pic_name[j] = 0;

#ifdef DEBUG_NAMES
    printf("GFX, looking for '%s'\n", pic_name);
#endif
    
    while (header_pos < gfx2_hsize)
    {
        type8s* hname = (type8s*)(gfx2_hdr + header_pos);

#ifdef DEBUG_NAMES
        {
            int i;
            printf("GFX name (%d): '", ++cc);
            for (i = 0; i < 6; ++i) putchar(hname[i]);
            putchar('\'');
            putchar('\n');
        }
#endif
        /*
        if (upper)
        {
            if (strncmp(hname,pic_name,6) == 0)
                return header_pos;
        }
        else
        {
            if (u_strnicmp(hname,pic_name,6) == 0)
                return header_pos;
        }
        */
        if (strncmp(hname,pic_name,6) == 0)
            return header_pos;
        header_pos += 16;
    }
    return -1;
}

#if 0
type8* extract_mask(struct picture * pic)
{
    // caller must free return mask
    
    type8* maskp = 0;
    if (pic->mask)
    {
        int w = pic->width;
        int h = pic->height;
        int stride = pic->mask_wbytes;
        type8* sp;
        type8* dp;

        assert(stride > 0);

        printf("extract_mask w:%d, h:%d, stride:%d\n", w, h, stride);
        
        int sum = 0;
        for (int i = 0; i < stride*h; ++i) sum += pic->mask[i];
        if (!sum)
        {
            // mask is all zero. a blank mask. ignore it.
            printf("mask is all zero\n");
            return 0;
        }

        for (int i = 0; i < stride*h; ++i) printf("%02X ", pic->mask[i]);
        printf("\n");
        
        // extracted mask is 1 byte per pixel, no stride padding
        dp = maskp = (type8*)malloc(stride*h);
        
        for (int i = 0; i < h; ++i)
        {
            sp = pic->mask + i*stride;

            int v;
            for (int j = 0; j < w; ++j)
            {
                // every 8, get a new byte
#if 1
                if ((j & 7) == 0) v = (*sp++) << 24;
                *dp++ = (v < 0) ? 0 : 0xff;
                v <<= 1;
#else
                if ((j & 7) == 0) v = *sp++;
                *dp++ = (v & 1) ? 0 : 0xff;
                v >>= 1;
#endif

                //printf("(%d,%d):%d ", j, i, dp[-1]);
            }
            
            //printf("\n");
        }

    }

    return maskp;
}
#endif

void extract_frame(struct picture * pic)
{
    type32 i, x, y, bit_x, mask, ywb, yw, value, values[4];

    if (pic->width * pic->height > MAX_PICTURE_SIZE)
    {
        ms_fatal("picture too large");
        return;
    }

    for (y = 0; y < pic->height; y++)
    {
        ywb = y * pic->wbytes;
        yw = y * pic->width;

        for (x = 0; x < pic->width; x++)
        {
            if ((x % 8) == 0)
            {
                for (i = 0; i < 4; i++)
                    values[i] = pic->data[ywb + (x / 8) + (pic->plane_step * i)];
            }

            bit_x = 7 - (x & 7);
            mask = 1 << bit_x;
            value = ((values[0] & mask) >> bit_x) << 0|
                ((values[1] & mask) >> bit_x) << 1|
                ((values[2] & mask) >> bit_x) << 2|
                ((values[3] & mask) >> bit_x) << 3;
            value &= 15;

            gfx_buf[yw + x] = (type8)value;
        }
    }
}

type8 *ms_extract2(type8s * name, type16 * w, type16 * h, type16 * pal, type8 * is_anim)
{
    struct picture main_pic;
    type32 offset = 0, length = 0, i;
    type16s header_pos = -1;
#ifndef NO_ANIMATION
    type8* anim_data;
    type32 j;
#endif

    if (is_anim != 0)
        *is_anim = 0;
    gfx2_name = name;

#ifndef NO_ANIMATION
    pos_table_size = 0;
#endif

#ifdef NO_ANIMATION
    /* Find the uppercase (no animation) version of the picture first. */
    header_pos = find_name_in_header(name,1);
#endif
    if (header_pos < 0)
        header_pos = find_name_in_header(name,0);
    if (header_pos < 0)
        return 0;

    offset = read_l(gfx2_hdr + header_pos + 8);
    length = read_l(gfx2_hdr + header_pos + 12);

    if (offset != 0)
    {
        if (gfx2_buf)
        {
            free(gfx2_buf);
            gfx2_buf = 0;
        }

        gfx2_buf = malloc(length);
        if (!gfx2_buf) return 0;

        if (fseek(gfx_fp, offset, SEEK_SET) < 0)
        {
            free(gfx2_buf);
            gfx2_buf = 0;
            return 0;
        }
        
        if (!fread(gfx2_buf, length, 1, gfx_fp))
        {
            free(gfx2_buf);
            gfx2_buf = 0;
            return 0;
        }

        int startcode = read_w2(gfx2_buf);
        if (startcode != 0x4)  // RAW_MAGIC_NUMBER
            printf("expected RAW magic number, %x\n", startcode);

        startcode = read_w2(gfx2_buf + 2);
        if (startcode != 0x101)  // BACKGROUND_CODE
            printf("expected background code, %x", startcode);

        // LoadPalette
        for (i = 0; i < 16; i++)
            pal[i] = read_w2(gfx2_buf + 4 + (2 * i));

        // offset = 36

        // short transp;        +0
        // short monoW = 0;     +2
        // short size;          +4
        // short w;             +6
        // short h;             +8
        // short d;             +10
        // char data[size];     +12

        // indicates transparency present
        main_pic.transp = read_w2(gfx2_buf + 36);

        // main picture should not have transparency
        assert(main_pic.transp > 15);

        // if non-zero, we have a mono image.
        int monoW = read_w2(gfx2_buf + 38);

        // main pic should not be mono
        assert(monoW == 0);
        
        main_pic.data_size = read_w2(gfx2_buf + 40);
        main_pic.width = read_w2(gfx2_buf + 42);
        main_pic.height = read_w2(gfx2_buf + 44);
        //int depth = read_w2(gfx2_buf + 46);
        main_pic.data = gfx2_buf + 48;
        main_pic.wbytes = (type16)(main_pic.data_size / main_pic.height);
        main_pic.plane_step = (type16)(main_pic.wbytes / 4);
        main_pic.mask = (type8*)0;
        extract_frame(&main_pic);

        *w = main_pic.width;
        *h = main_pic.height;

        // offset = 48 + size

#ifndef NO_ANIMATION
        /* Check for an animation */
        anim_data = gfx2_buf + 48 + main_pic.data_size;

        // short start_time
        // short n_images;
        
        //int start_time = read_w2(anim_data);
        
        if ((anim_data[0] != 0xD0) || (anim_data[1] != 0x5E))
        {
            //type8 *current;
            type16 frame_count, command_count;

            if (is_anim != 0) *is_anim = 1;

            frame_count = read_w2(anim_data + 2); // n_images
            //printf("Frame count: %d\n",frame_count);

            type8* fi = anim_data + 4;
            
            //current = anim_data + 6;
            if (frame_count > MAX_ANIMS)
            {
                ms_fatal("animation frame array too short");
                return 0;
            }

            /* Loop through each animation frame */
            for (i = 0; i < frame_count; i++)
            {
                // short transp;        +0
                // short monoW          +2
                // short size;          +4
                // short w;             +6
                // short h;             +8
                // short d;             +10
                // char data[size];     +12
                
                anim_frame_table[i].transp = read_w2(fi);
                
                int monoW = read_w2(fi + 2);
                assert(monoW == 0); // expect colour frames

                int size = read_w2(fi + 4);
                anim_frame_table[i].data_size = size;
                anim_frame_table[i].width = read_w2(fi + 6);
                anim_frame_table[i].height = read_w2(fi + 8);
                anim_frame_table[i].data = fi + 12;
                anim_frame_table[i].wbytes = (type16)(anim_frame_table[i].data_size / anim_frame_table[i].height);
                anim_frame_table[i].plane_step = (type16)(anim_frame_table[i].wbytes / 4);
                anim_frame_table[i].mask = (type8*)0;

                fi += 12 + size;

                /*
                printf("frame %d:\ntransp:%u, size:%d,w:%u,h:%u,data:%x\n",
                       i,
                       anim_frame_table[i].transp,
                       size, 
                       anim_frame_table[i].width,
                       anim_frame_table[i].height,
                       (unsigned int)anim_frame_table[i].data);
                */
                       
                if (anim_frame_table[i].transp < 16)
                {
                    int monoW = read_w2(fi);
                    int w, h, mask_size;

                    if (monoW)
                    {
                        w = monoW;
                        h = read_w2(fi + 2);
                        mask_size = read_w2(fi + 4);

                        // mask must be same size
                        assert(w == anim_frame_table[i].width);
                        assert(h == anim_frame_table[i].height);

                        anim_frame_table[i].mask_wbytes = mask_size/h;
                        anim_frame_table[i].mask = fi + 6;
                        
                        fi += 6 + mask_size;
                    }
                    else
                    {
                        assert(0); // colour mask??
                        
                        mask_size = read_w2(fi + 2);
                        w = read_w2(fi + 4);
                        h = read_w2(fi + 6);
                        //depth = read_w2(fi + 8);

                        // mask must be same size
                        assert(w == anim_frame_table[i].width);
                        assert(h == anim_frame_table[i].height);
                        
                        anim_frame_table[i].mask_wbytes = mask_size/h;
                        anim_frame_table[i].mask = fi + 10;

                        fi += 10 + mask_size;
                    }

                    //printf("mask %d: mono:%d, w:%d, h:%d, transp size:%d\n", i, monoW, w, h, mask_size);
                }

            }

            //printf("Start of positioning tables: %x %x %x %x\n",fi[0],fi[1],fi[2],fi[3]);

            // short n_sequences;

            /* Get the positioning tables */
            pos_table_size = read_w2(fi); fi += 2;
            //printf("Position table size: %d\n",pos_table_size);
                        
            if (pos_table_size > MAX_POSITIONS)
            {
                ms_fatal("animation position array too short");
                return 0;
            }

#ifdef LOGGFX_EXT
            out2("POSITION TABLE DUMP\n");
#endif
            for (i = 0; i < pos_table_size; i++)
            {
                // short priority;
                // short length;
                
                int priority = read_w2(fi);
                (void)priority; // no warning
                int length = read_w2(fi + 2);
                fi += 4;

                pos_table_count[i] = length;
                //printf("Position table %d size: %d\n",i, length);

                if (pos_table_count[i] > MAX_ANIMS)
                {
                    ms_fatal("animation position array too short");
                    return 0;
                }

                for (j = 0; j < length; j++)
                {
                    // short posx;
                    // short posy;
                    // short index;
                    // short interval;

                    pos_table[i][j].x = read_w2(fi);
                    pos_table[i][j].y = read_w2(fi + 2);
                    pos_table[i][j].number = read_w2(fi + 4) - 1;

                    int interval = read_w2(fi + 6);
                    (void)interval; // no warning
#ifdef LOGGFX_EXT
                    printf("Position entry: Table: %hu  Entry: %hu  X: %hi Y: %hi Frame: %hi\n",
                         i,j,pos_table[i][j].x,pos_table[i][j].y,pos_table[i][j].number);
#endif
                    fi += 8;
                }
            }

            // short n;
            // char data[n];

            /* Get the command sequence table */
            command_count = read_w2(fi);
            command_table = fi + 2;
            fi += 2 + command_count;

            /*
            printf("Script size: %d\n",command_count);
            printf("Script: ");
            for (i = 0; i < command_count; ++i) printf(" %02X",command_table[i]);
            printf("\n");
            */

            for (i = 0; i < MAX_POSITIONS; i++)
            {
                anim_table[i].flag = -1;
                anim_table[i].count = -1;
            }
            command_index = 0;
            anim_repeat = 0;
            pos_table_index = -1;
            pos_table_max = -1;
        }
#endif
        return gfx_buf;
    }
    return 0;
}

type8 *ms_extract(type32 pic, type16 * w, type16 * h, type16 * pal, type8 * is_anim)
{
    if (is_anim)
        *is_anim = 0;

    if (gfx_buf)
    {
        switch (gfx_ver)
        {
        case 1:
            return ms_extract1((type8)pic,w,h,pal);
        case 2:
            return ms_extract2((type8s*)(code + pic),w,h,pal,is_anim);
        }
    }
    return 0;
}


#define         AN_OP_END                       0
#define         AN_OP_RUN_SEQUENCE              1
#define         AN_OP_WAIT_FOR                  2
#define         AN_OP_GOTO                      3
#define         AN_OP_RANDOM_WAIT               4
#define         AN_OP_GOTO_IF_RANDOM            5
#define         AN_OP_GOTO_IF_RUNNING           6

type8 ms_animate(struct ms_position ** positions, type16 * count)
{
#ifndef NO_ANIMATION
    type8 got_anim = 0;
    type16 i, j, ttable;

    if ((gfx_buf == 0) || (gfx2_buf == 0) || (gfx_ver != 2))
        return 0;
    if ((pos_table_size == 0) || (command_index < 0))
        return 0;

    *count = 0;
    *positions = (struct ms_position*)0;

    while (got_anim == 0)
    {
        if (pos_table_max >= 0)
        {
            if (pos_table_index < pos_table_max)
            {
                for (i = 0; i < pos_table_size; i++)
                {
                    if (anim_table[i].flag > -1)
                    {
                        if (*count >= MAX_FRAMES)
                        {
                            ms_fatal("returned animation array too short");
                            return 0;
                        }

                        pos_array[*count] = pos_table[i][anim_table[i].flag];
#ifdef LOGGFX_EXT
                        out2("Adding frame %hu to buffer\n",pos_array[*count].number);
#endif
                        (*count)++;

                        if (anim_table[i].flag < (pos_table_count[i]-1))
                            anim_table[i].flag++;
                        if (anim_table[i].count > 0)
                            anim_table[i].count--;
                        else
                            anim_table[i].flag = -1;
                    }
                }
                if (*count > 0)
                {
                    *positions = pos_array;
                    got_anim = 1;
                }
                pos_table_index++;
            }
        }

        if (got_anim == 0)
        {
            type8 command = command_table[command_index];
            command_index++;

            pos_table_max = -1;
            pos_table_index = -1;

            switch (command)
            {
            case AN_OP_END:
                command_index = -1;
                return 0;
            case AN_OP_RUN_SEQUENCE:
#ifdef LOGGFX_EXT
                out2("Load Frame Table: %hi  Start at: %hi  Count: %hi\n",
                     command_table[command_index],command_table[command_index+1],
                     command_table[command_index+2]);
#endif
                ttable = command_table[command_index];
                command_index++;

                if (ttable-1 >= MAX_POSITIONS)
                {
                    ms_fatal("animation table too short");
                    return 0;
                }

                anim_table[ttable-1].flag = (type16s)(command_table[command_index]-1);
                command_index++;
                anim_table[ttable-1].count = (type16s)(command_table[command_index]-1);
                command_index++;

                /* Workaround for Wonderland "catter" animation */
                if (v4_id == 0)
                {
                    if (strcmp(gfx2_name,"catter") == 0)
                    {
                        if (command_index == 96)
                            anim_table[ttable-1].count = 9;
                        if (command_index == 108)
                            anim_table[ttable-1].flag = -1;
                        if (command_index == 156)
                            anim_table[ttable-1].flag = -1;
                    }
                }
                break;
            case AN_OP_WAIT_FOR:
#ifdef LOGGFX_EXT
                out2("Animate: %hi\n", command_table[command_index]);
#endif
                pos_table_max = command_table[command_index];
                pos_table_index = 0;
                command_index++;
                break;
            case AN_OP_GOTO:
#ifdef LOGGFX_EXT
                out2("Stop/Repeat Param: %hi\n", command_table[command_index]);
                command_index = -1;
                return 0;
#else
                if (v4_id == 0)
                {
                    command_index = -1;
                    return 0;
                }
                else
                {
                    command_index = 0;
                    anim_repeat = 1;
                    pos_table_index = -1;
                    pos_table_max = -1;
                    for (j = 0; j < MAX_POSITIONS; j++)
                    {
                        anim_table[j].flag = -1;
                        anim_table[j].count = -1;
                    }
                }
                break;
#endif
            case AN_OP_RANDOM_WAIT:
#ifdef LOGGFX_EXT
                out2("Unknown Command: %hi Prop1: %hi  Prop2: %hi WARNING:not parsed\n", command,
                     command_table[command_index],command_table[command_index+1]);
#endif
                command_index += 3;
                return 0;
            case AN_OP_GOTO_IF_RANDOM:
                ttable = next_table;
                command_index++;

                anim_table[ttable-1].flag = 0;
                anim_table[ttable-1].count = command_table[command_index];

                pos_table_max = command_table[command_index];
                pos_table_index = 0;
                command_index++;
                command_index++;
                next_table++;
                break;
            case AN_OP_GOTO_IF_RUNNING:
            default:
                ms_fatal("unknown animation command");
                command_index = -1;
                return 0;
            }
        }
    }
#if 0
    out2("ms_animate() returning %hu frames\n",*count);
#endif
    return got_anim;
#else
    return 0;
#endif
}

type8 * ms_get_anim_frame(type16s number, type16 * width, type16 * height, type8 ** mask)
{
#ifndef NO_ANIMATION
    if (number >= 0)
    {
        extract_frame(anim_frame_table + number);
        *width = anim_frame_table[number].width;
        *height = anim_frame_table[number].height;
        *mask = anim_frame_table[number].mask;
        return gfx_buf;
    }
#endif
    return 0;
}

type8 ms_anim_is_repeating(void)
{
#ifndef NO_ANIMATION
    return anim_repeat;
#else
    return 0;
#endif
}

type16s find_name_in_sndheader(type8s * name)
{
    type16s header_pos = 0;

    while (header_pos < snd_hsize)
    {
        type8s* hname = (type8s*)(snd_hdr + header_pos);
        if (strcmp(hname,name) == 0)
            return header_pos;
        header_pos += 18;
    }
    return -1;
}

type8 *sound_extract(type8s * name, type32 * length, type16 * tempo)
{
    type32 offset = 0;
    type16s header_pos = -1;

    if (header_pos < 0)
        header_pos = find_name_in_sndheader(name);
    if (header_pos < 0)
        return 0;

    *tempo = read_w(snd_hdr + header_pos + 8);
    offset = read_l(snd_hdr + header_pos + 10);
    *length = read_l(snd_hdr + header_pos + 14);

    if (offset != 0)
    {
        if (!snd_buf)
            return 0;
        if (fseek(snd_fp, offset, SEEK_SET) < 0)
            return 0;
        if (!fread(snd_buf, (int)(*length), 1, snd_fp))
            return 0;
        return snd_buf;
    }
    return 0;
}

static void undo_swap()
{
    // swap undo buffers and registers
    
    type8* tmp;
    int i;

    tmp = undo[0];
    undo[0] = undo[1];
    undo[1] = tmp;

    for (i = 0; i < 18; i++)
    {
        type32 tmp32 = undo_regs[0][i];
        undo_regs[0][i] = undo_regs[1][i];
        undo_regs[1][i] = tmp32;
    }
}

void save_undo(void)
{
    int i;

    if (prog_format) return; // not done here
    
    undo_swap();

    // save into buf1
    memcpy(undo[1], code, undo_size);
    for (i = 0; i < 8; i++)
    {
        undo_regs[1][i] = dreg[i];
        undo_regs[1][8 + i] = areg[i];
    }
    undo_regs[1][16] = i_count;
    undo_regs[1][17] = pc;  /* status flags intentionally omitted */

    undo_stat[0] = undo_stat[1];
    undo_stat[1] = 1;

    game_state_notify(0); // unknown move count for non prog-format
    
}

type8 ms_undo(void)
{
    type8 i;

    if (prog_format) return 0; // not done here

    ms_flush();
    if (!undo_stat[0])
        return 0;

    // restore from buf0
    memcpy(code, undo[0], undo_size);
    for (i = 0; i < 8; i++)
    {
        dreg[i] = undo_regs[0][i];
        areg[i] = undo_regs[0][8 + i];
    }
    i_count = undo_regs[0][16];
    pc = undo_regs[0][17];  /* status flags intentionally omitted */

    // buf 0 -> 1
    undo_swap();
    undo_stat[0] = 0;

    return 1;
}

#ifdef LOGEMU
void log_status(void)
{
    int j;

    fprintf(dbg_log, "\nD0:");
    for (j = 0; j < 8; j++)
        fprintf(dbg_log, " %8.8x", read_reg(j, 3));
    fprintf(dbg_log, "\nA0:");
    for (j = 0; j < 8; j++)
        fprintf(dbg_log, " %8.8x", read_reg(8 + j, 3));
    fprintf(dbg_log, "\nPC=%5.5x (%8.8x) ZCNV=%d%d%d%d - %d instructions\n\n",
            pc, code, zflag & 1, cflag & 1, nflag & 1, vflag & 1, i_count);
}
#endif

void ms_status(void)
{
    int j;

    fprintf(stderr, "D0:");
    for (j = 0; j < 8; j++)
        fprintf(stderr, " %8.8lx", (long) read_reg(j, 3));
    fprintf(stderr, "\nA0:");
    for (j = 0; j < 8; j++)
        fprintf(stderr, " %8.8lx", (long) read_reg(8 + j, 3));
    fprintf(stderr, "\nPC=%5.5lx ZCNV=%d%d%d%d - %ld instructions\n",
            (long) pc, zflag & 1, cflag & 1, nflag & 1, vflag & 1, (long) i_count);
}

type32 ms_count(void)
{
    return i_count;
}

/* align register pointer for word/byte accesses */

type8 *reg_align(type8 * ptr, type8 size)
{
    if (size == 1)
        ptr += 2;
    if (size == 0)
        ptr += 3;
    return ptr;
}

type32 read_reg(int i, int s)
{
    type8 *ptr;

    if (i > 15)
    {
        ms_fatal("invalid register in read_reg");
        return 0;
    }
    if (i < 8)
        ptr = (type8 *) & dreg[i];
    else
        ptr = (type8 *) & areg[i - 8];

    switch (s)
    {
    case 0:
        return read_b(reg_align(ptr, 0));
    case 1:
        return read_w(reg_align(ptr, 1));
    default:
        return read_l(ptr);
    }
}

void write_reg(int i, int s, type32 val)
{
    type8 *ptr;

    if (i > 15)
    {
        ms_fatal("invalid register in write_reg");
        return;
    }
    if (i < 8)
        ptr = (type8 *) & dreg[i];
    else
        ptr = (type8 *) & areg[i - 8];

    switch (s)
    {
    case 0:
        write_b(reg_align(ptr, 0), (type8)val);
        break;
    case 1:
        write_w(reg_align(ptr, 1), (type16)val);
        break;
    default:
        write_l(ptr, val);
        break;
    }
}

/* [35c4] */

void char_out(type8 c)
{
    _char_out(c, ms_putchar);
}

void _char_out(type8 c, outfn fn)
{
    if (c == 0xff)
    {
        big = 1;
        return;
    }

    if (c == '\\')
    {
        // set escape mode. This inhibits all processing until the next
        // newline
        lastchar = c;
        escape = !escape;
        return;
    }

    if (lastchar == '\\')
    {
        if (c == 'n') c = '\n';
    }

    if (c == '\n')
    {
        escape = 0;
    }

    c &= 0x7f;

    if (read_reg(3, 0))
    {
        if (c == '_' || c == '@') c = ' ';
        if (c >= 'a' && c <= 'z') c &= 0xdf;
        if (version < 4) ms_statuschar(c);
        return;
    }

    if (!escape)
    {
        if (c == '^') c = 0x0a;
        if (c == '@')
        {
            // plural agreement
            if (read_reg(2, 0)) return;
            c = 's';
        }
        if (version < 3 && c == '~')
        {
            lastchar = '~';
            c = 0x0a;
        }
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        {
            if (big)
            {
                c &= 0xdf;
                big = 0;
            }
            if (period)
                _char_out(' ', fn);
        }
        period = 0;
        if (version < 4)
        {
            if (c == '.' || c == '?' || c == '!' || c == 0x0a)
                big = 1;
            else if (c == '"')
                big = 0;
        }
        else
        {
            if (c == ' ' && lastchar == 0x0a)
                return;
            if (c == '.' || c == '?' || c == '!' || c == 0x0a)
                big = 1;
            else if (c == '"')
                big = 0;
        }

        // eat extra space
        //if ((c == ' ' || c == 0x0a) && c == lastchar) return;
        if (c == ' ' && c == lastchar) return;
        
        if (version < 3)
        {
            if (epipe)
            {
                epipe = 0;
                return;
            }
            if (c == '|')
            {
                epipe = 1;
                return;
            }
        }
        else
        {
            if (c == '~')
            {
                c = 0x0a;
                if (lastchar != 0x0a)
                    _char_out(0x0a, fn);
            }
        }

        if (c == '_') c = ' ';
        else if (c == '.' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?')
            period = 1;
    }
    
    lastchar = c;
    (*fn)(c);
}


/* extract addressing mode information [1c6f] */

void set_info(type8 b)
{
    regnr = (type8)(b & 0x07);
    admode = (type8)((b >> 3) & 0x07);
    opsize = (type8)(b >> 6);
}

/* read a word and increase pc */

void read_word(void)
{
    type8 *epc;
    epc = effective(pc);
    byte1 = read_b(epc);
    byte2 = read_b(epc + 1);
    pc += 2;
#ifdef LOGDIS
    op_words++;
#endif
}

/* get addressing mode and set arg1 [1c84] */

void set_arg1(void)
{
    type8 tmp[2], l1c;

    is_reversible = 1;
    switch (admode)
    {
    case 0:
        arg1 = reg_align((type8 *) & dreg[regnr], opsize);      /* Dx */
        is_reversible = 0;
#ifdef LOGDIS
        push_msg(DASM_ARG1,"d%.1d", regnr);
#endif
        break;
    case 1:
        arg1 = reg_align((type8 *) & areg[regnr], opsize);      /* Ax */
        is_reversible = 0;
#ifdef LOGDIS
        push_msg(DASM_ARG1,"a%.1d", regnr);
#endif
        break;
    case 2:
        arg1i = read_reg(8 + regnr, 2);         /* (Ax) */
#ifdef LOGDIS
        push_msg(DASM_ARG1,"(a%.1d)", regnr);
#endif
        break;
    case 3:
        arg1i = read_reg(8 + regnr, 2);         /* (Ax)+ */
        write_reg(8 + regnr, 2, read_reg(8 + regnr, 2) + (1 << opsize));
#ifdef LOGDIS
        push_msg(DASM_ARG1,"(a%.1d)+", regnr);
#endif
        break;
    case 4:
        write_reg(8 + regnr, 2, read_reg(8 + regnr, 2) - (1 << opsize));
        arg1i = read_reg(8 + regnr, 2);         /* -(Ax) */
#ifdef LOGDIS
        push_msg(DASM_ARG1,"-(a%.1d)", regnr);
#endif
        break;
    case 5:
        {
            type16s i = (type16s) read_w(effective(pc));
            arg1i = read_reg(8 + regnr, 2) + i;
#ifdef LOGDIS
            op_words++;
#endif
            pc += 2;        /* offset.w(Ax) */
#ifdef LOGDIS
            push_msg(DASM_ARG1,"$%X(a%.1d)", i, regnr);
#endif
        }
        break;
    case 6:
        tmp[0] = byte1;
        tmp[1] = byte2;
        read_word();    /* offset.b(Ax, Dx/Ax) [1d1c] */
#ifdef LOGDIS
        push_msg(DASM_ARG1,"$%.2X(a%.1d,", (int) byte2, regnr);
#endif
        arg1i = read_reg(regnr + 8, 2) + (type8s) byte2;
#ifdef LOGDIS
        if ((byte1 >> 4) > 8)
            push_msg(DASM_ARG1,"a%.1d", (byte1 >> 4) - 8);
        else
            push_msg(DASM_ARG1,"d%.1d", byte1 >> 4);
#endif
        if (byte1 & 0x08)
        {
#ifdef LOGDIS
            push_msg(DASM_ARG1,".l)");
#endif
            arg1i += (type32s) read_reg((byte1 >> 4), 2);
        }
        else
        {
#ifdef LOGDIS
            push_msg(DASM_ARG1,".w)");
#endif
            arg1i += (type16s) read_reg((byte1 >> 4), 1);
        }
        byte1 = tmp[0];
        byte2 = tmp[1];
        break;
    case 7:         /* specials */
        switch (regnr)
        {
        case 0:
            arg1i = read_w(effective(pc));  /* $xxxx.W */
#ifdef LOGDIS
            op_words++;
#endif
            pc += 2;
#ifdef LOGDIS
            push_msg(DASM_ARG1,"$%.4X.w", arg1i);
#endif
            break;
        case 1:
            arg1i = read_l(effective(pc));  /* $xxxx */
#ifdef LOGDIS
            push_msg(DASM_ARG1,"$%.8X", arg1i);
            op_words += 2;
#endif
            pc += 4;
            break;
        case 2:
            arg1i = (type16s) read_w(effective(pc)) + pc;   /* $xxxx(PC) */
#ifdef LOGDIS
            op_words++;
#endif
            pc += 2;
#ifdef LOGDIS
            push_msg(DASM_ARG1,"$%.4X(pc)", arg1i);
#endif
            break;
        case 3:
            l1c = read_b(effective(pc));     /* $xx(PC,A/Dx) */
#ifdef LOGDIS
            push_msg(DASM_ARG1,"$%.2", arg1i);
#endif
            if (l1c & 0x08)
                arg1i = pc + (type32s) read_reg((l1c >> 4), 2);
            else
                arg1i = pc + (type16s) read_reg((l1c >> 4), 1);
            l1c = read_b(effective(pc) + 1);
#ifdef LOGDIS
            op_words++;
#endif
            pc += 2;
            arg1i += (type8s) l1c;
            break;
        case 4:
            arg1i = pc;     /* #$xxxx */
            if (opsize == 0)
                arg1i += 1;

            pc += 2;
            if (opsize == 2)
                pc += 2;

#ifdef LOGDIS
            if (opsize==0) {
                push_msg(DASM_ARG1,"#$%.2X", read_b(effective(arg1i)));
                op_words++;
            }
            else if (opsize==1)
            {
                push_msg(DASM_ARG1,"#$%.4X", read_w(effective(arg1i)));
                op_words++;
            }
            else
            {
                push_msg(DASM_ARG1,"#$%.8X", read_l(effective(arg1i)));
                op_words+=2;
            }
#endif
            break;
        }
        break;
    }
    if (is_reversible)
        arg1 = effective(arg1i);
}

/* get addressing mode and set arg2 [1bc5] */

void set_arg2_nosize(int use_dx, type8 b)
{
    if (use_dx)
        arg2 = (type8 *) dreg;
    else
        arg2 = (type8 *) areg;
    arg2 += (b & 0x0e) << 1;
}

void set_arg2(int use_dx, type8 b)
{
    set_arg2_nosize(use_dx, b);
    arg2 = reg_align(arg2, opsize);
}

/* [1b9e] */

void swap_args(void)
{
    type8 *tmp;

    tmp = arg1;
    arg1 = arg2;
    arg2 = tmp;
#ifdef DISASM
    swapflag=1;
#endif
}

/* [1cdc] */

void push(type32 c)
{
    write_reg(15, 2, read_reg(15, 2) - 4);
    write_l(effective(read_reg(15, 2)), c);
}

/* [1cd1] */

type32 pop(void)
{
    type32 c;

    c = read_l(effective(read_reg(15, 2)));
    write_reg(15, 2, read_reg(15, 2) + 4);
    return c;
}

static int pop16(void)
{
    int c = read_w(effective(read_reg(15, 2)));
    write_reg(15, 2, read_reg(15, 2) + 2);
    return c;
}

static int pop32(void)
{
    int a = pop16();
    int b = pop16();
    return (a << 16) + b;
}

/* check addressing mode and get argument [2e85] */

void get_arg(void)
{
    set_info(byte2);
    arg2 = effective(pc);
#ifdef LOGDIS
    if (opsize == 0) {
        push_msg(DASM_OPSIZE,"b");
        push_msg(DASM_ARG1,"#$%.2X",effective(pc)[1]);
        op_words++;
    }
    else if (opsize == 1){
        push_msg(DASM_OPSIZE,"w");
        push_msg(DASM_ARG1,"#$%.2X%.2X",effective(pc)[0],effective(pc)[1]);
        op_words++;
    }
    else if (opsize == 2) {
        push_msg(DASM_OPSIZE,"l");
        push_msg(DASM_ARG1,"#$%.2X%.2X%.2X%.2X",effective(pc)[0],effective(pc)[1],effective(pc)[2],effective(pc)[3]);
        op_words+=2;
    }
#endif
    pc += 2;
    if (opsize == 2)
        pc += 2;
    if (opsize == 0)
        arg2 += 1;
#ifdef LOGDIS
    swapflag=1;
#endif
    set_arg1();
#ifdef LOGDIS
    swapflag=0;
#endif
}

void set_flags(void)
{
    type16 i;
    type32 j;

    zflag = nflag = 0;
    switch (opsize)
    {
    case 0:
        if (read_b(arg1) > 127)
            nflag = 0xff;
        if (read_b(arg1) == 0)
            zflag = 0xff;
        break;
    case 1:
        i = read_w(arg1);
        if (i == 0)
            zflag = 0xff;
        if ((i >> 15) > 0)
            nflag = 0xff;
        break;
    case 2:
        j = read_l(arg1);
        if (j == 0)
            zflag = 0xff;
        if ((j >> 31) > 0)
            nflag = 0xff;
        break;
    }
}

/* [263a] */

int condition(type8 b)
{
    switch (b & 0x0f)
    {
    case 0:
        return 0xff;
    case 1:
        return 0x00;
    case 2:
        return (zflag | cflag) ^ 0xff;
    case 3:
        return (zflag | cflag);
    case 4:
        return cflag ^ 0xff;
    case 5:
        return cflag;
    case 6:
        return zflag ^ 0xff;
    case 7:
        return zflag;
    case 8:
        return vflag ^ 0xff;
    case 9:
        return vflag;
    case 10:
    case 12:
        return nflag ^ 0xff;
    case 11:
    case 13:
        return nflag;
    case 14:
        return (zflag | nflag) ^ 0xff;
    case 15:
        return (zflag | nflag);
    }
    return 0x00;
}

/* [26dc] */

void branch(type8 b)
{
    type32 newpc;
    if (b == 0)
    {
#ifdef LOGDIS
        op_words++;
#endif
        newpc = pc + (type16s) read_w(effective(pc));
#ifdef LOGDIS
        if (!no_branch)
#endif
        pc = newpc;
#ifdef LOGDIS
        else
            pc+=2;
#endif
    }
    else
    {
        newpc = pc + (type8s) b;
#ifdef LOGDIS
        if (!no_branch)
#endif
        pc = newpc;
    }
#ifdef LOGDIS
    push_msg(DASM_ARG1,"$%.4X", newpc);
#endif
}

/* [2869] */

void do_add(type8 adda)
{
#ifdef LOGDIS
    if (opsize == 0)
        push_msg(DASM_OPSIZE,"b");
    else if (opsize == 1)
        push_msg(DASM_OPSIZE,"w");
    else if (opsize == 2)
        push_msg(DASM_OPSIZE,"l");
#endif

    if (adda)
    {
        if (opsize == 0)
            write_l(arg1, read_l(arg1) + (type8s) read_b(arg2));
        if (opsize == 1)
            write_l(arg1, read_l(arg1) + (type16s) read_w(arg2));
        if (opsize == 2)
            write_l(arg1, read_l(arg1) + (type32s) read_l(arg2));
    }
    else
    {
        cflag = 0;
        if (opsize == 0)
        {
            write_b(arg1, read_b(arg1) + read_b(arg2));
            if (arg2[0] > arg1[0])
                cflag = 0xff;
        }
        if (opsize == 1)
        {
            write_w(arg1, (type16)(read_w(arg1) + read_w(arg2)));
            if (read_w(arg2) > read_w(arg1))
                cflag = 0xff;
        }
        if (opsize == 2)
        {
            write_l(arg1, read_l(arg1) + read_l(arg2));
            if (read_l(arg2) > read_l(arg1))
                cflag = 0xff;
        }
        if (version < 3 || !quick_flag)
        {
            /* Corruption onwards */
            vflag = 0;
            set_flags();
        }
    }
}

/* [2923] */

void do_sub(type8 suba)
{
#ifdef LOGDIS
    if (opsize == 0)
        push_msg(DASM_OPSIZE,"b");
    else if (opsize == 1)
        push_msg(DASM_OPSIZE,"w");
    else if (opsize == 2)
        push_msg(DASM_OPSIZE,"l");
#endif
    if (suba)
    {
        if (opsize == 0)
            write_l(arg1, read_l(arg1) - (type8s) read_b(arg2));
        if (opsize == 1)
            write_l(arg1, read_l(arg1) - (type16s) read_w(arg2));
        if (opsize == 2)
            write_l(arg1, read_l(arg1) - (type32s) read_l(arg2));
    }
    else
    {
        cflag = 0;
        if (opsize == 0)
        {
            if (arg2[0] > arg1[0])
                cflag = 0xff;
            write_b(arg1, read_b(arg1) - read_b(arg2));
        }
        if (opsize == 1)
        {
            if (read_w(arg2) > read_w(arg1))
                cflag = 0xff;
            write_w(arg1, (type16)(read_w(arg1) - read_w(arg2)));
        }
        if (opsize == 2)
        {
            if (read_l(arg2) > read_l(arg1))
                cflag = 0xff;
            write_l(arg1, read_l(arg1) - read_l(arg2));
        }
        if (version < 3 || !quick_flag)
        {
            /* Corruption onwards */
            vflag = 0;
            set_flags();
        }
    }
}

/* [283b] */

void do_eor(void)
{
    if (opsize == 0)
        write_b(arg1, read_b(arg1) ^ read_b(arg2));
    if (opsize == 1)
        write_w(arg1, (type16)(read_w(arg1) ^ read_w(arg2)));
    if (opsize == 2)
        write_l(arg1, read_l(arg1) ^ read_l(arg2));
    cflag = vflag = 0;
    set_flags();
}

/* [280d] */

void do_and(void)
{
    if (opsize == 0)
        write_b(arg1, read_b(arg1) & read_b(arg2));
    if (opsize == 1)
        write_w(arg1, (type16)(read_w(arg1) & read_w(arg2)));
    if (opsize == 2)
        write_l(arg1, read_l(arg1) & read_l(arg2));
    cflag = vflag = 0;
    set_flags();
}

/* [27df] */

void do_or(void)
{
    if (opsize == 0)
        write_b(arg1, read_b(arg1) | read_b(arg2));
    if (opsize == 1)
        write_w(arg1, (type16)(read_w(arg1) | read_w(arg2)));
    if (opsize == 2)
        write_l(arg1, read_l(arg1) | read_l(arg2));
    cflag = vflag = 0;
    set_flags();    /* [1c2b] */
}

/* [289f] */

void do_cmp(void)
{
    type8 *tmp;

    tmp = arg1;
    tmparg[0] = arg1[0];
    tmparg[1] = arg1[1];
    tmparg[2] = arg1[2];
    tmparg[3] = arg1[3];
    arg1 = tmparg;
    quick_flag = 0;
    do_sub(0);
    arg1 = tmp;
}

/* [2973] */

void do_move(void)
{

    if (opsize == 0)
        write_b(arg1, read_b(arg2));
    if (opsize == 1)
        write_w(arg1, read_w(arg2));
    if (opsize == 2)
        write_l(arg1, read_l(arg2));
    if (version < 2 || admode != 1)
    {
        /* Jinxter: no flags if destination Ax */
        cflag = vflag = 0;
        set_flags();
    }
}

type8 do_btst(type8 a)
{
    a &= admode ? 0x7 : 0x1f;
    while (admode == 0 && a >= 8)
    {
        a -= 8;
        arg1 -= 1;
    }
    zflag = 0;
    if ((read_b(arg1) & (1 << a)) == 0)
        zflag = 0xff;
    return a;
}

/* bit operation entry point [307c] */

void do_bop(type8 b, type8 a)
{
#ifdef LOGDIS
    push_msg(DASM_OP,"btst");
#endif
    b = b & 0xc0;
    a = do_btst(a);
#ifdef LOGDIS
    //if (b == 0x00)
    //    push_msg(DASM_OP,"?????");
#endif
    if (b == 0x40)
    {
        write_b(arg1, read_b(arg1) ^ (1 << a));    /* bchg */
#ifdef LOGDIS
        push_msg(DASM_OP,"bchg");
#endif
    }
    if (b == 0x80)
    {
        write_b(arg1, read_b(arg1) & ((1 << a) ^ 0xff));   /* bclr */
#ifdef LOGDIS
        push_msg(DASM_OP,"bclr");
#endif
    }
    if (b == 0xc0)
    {
        write_b(arg1, read_b(arg1) | (1 << a));    /* bset */
#ifdef LOGDIS
        push_msg(DASM_OP,"bset");
#endif
    }
}

void check_btst(void)
{
#ifdef LOGDIS
    push_msg(DASM_OP,"btst");
#endif
    set_info((type8)(byte2 & 0x3f));
    set_arg1();
    set_arg2(1, byte1);
    do_bop(byte2, arg2[0]);
}

void check_lea(void)
{
#ifdef LOGDIS
    push_msg(DASM_OP,"lea");
#endif
    if ((byte2 & 0xc0) == 0xc0)
    {
        set_info(byte2);
        opsize = 2;
        set_arg1();
        set_arg2(0, byte1);
#ifdef LOGDIS
        push_msg(DASM_ARG2,"a%.1d",(byte1>>1)-0x20);
#endif
        write_w(arg2, 0);
        if (is_reversible)
            write_l(arg2, arg1i);
        else
            ms_fatal("illegal addressing mode for LEA");
    }
    else
    {
        ms_fatal("unimplemented instruction CHK");
    }
}

/* [33cc] */

void check_movem(void)
{
    type8 l1c;

#ifdef LOGDIS
    type8 hit=0;
    push_msg(DASM_OP,"movem");
#endif
    set_info((type8)(byte2 - 0x40));
    read_word();
#ifdef LOGDIS
    arglooping=1;
    if (opsize == 2)
        push_msg(DASM_OPSIZE,"l");
    if (opsize == 1)
        push_msg(DASM_OPSIZE,"w");
#endif
    for (l1c = 0; l1c < 8; l1c++)
    {
        if (byte2 & 1 << l1c)
        {
#ifdef LOGDIS
            if (hit)
                push_msg(DASM_ARG1,"/",l1c);
            else
                hit=1;

            push_msg(DASM_ARG1,"d%.1x",l1c);
            swapflag=1;
#endif
            set_arg1();
#ifdef LOGDIS
            swapflag=0;
#endif
            if (opsize == 2)
                write_l(arg1, read_reg(15 - l1c, 2));
            if (opsize == 1)
                write_w(arg1, (type16)read_reg(15 - l1c, 1));
        }
    }
    for (l1c = 0; l1c < 8; l1c++)
    {
        if (byte1 & 1 << l1c)
        {
#ifdef LOGDIS
            if (hit)
                push_msg(DASM_ARG1,"/",l1c);
            else
                hit=1;

            push_msg(DASM_ARG1,"a%.1x",l1c);
            swapflag=1;
#endif
            set_arg1();
#ifdef LOGDIS
            swapflag=0;
#endif
            if (opsize == 2)
                write_l(arg1, read_reg(7 - l1c, 2));
            if (opsize == 1)
                write_w(arg1, (type16)read_reg(7 - l1c, 1));
        }
    }
#ifdef LOGDIS
    arglooping = 0;
    swapflag=0;
#endif
}

/* [3357] */

void check_movem2(void)
{
    type8 l1c;

#ifdef LOGDIS
    type8 hit=0;
    push_msg(DASM_OP,"movem");
#endif
    set_info((type8)(byte2 - 0x40));
    read_word();
#ifdef LOGDIS
    arglooping=1;
    if (opsize == 2)
        push_msg(DASM_OPSIZE,"l");
    if (opsize == 1)
        push_msg(DASM_OPSIZE,"w");
#endif
    for (l1c = 0; l1c < 8; l1c++)
    {
        if (byte2 & 1 << l1c)
        {
#ifdef LOGDIS
            if (hit)
                push_msg(DASM_ARG2,"/",l1c);
            else
                hit=1;

            push_msg(DASM_ARG2,"d%.1x",l1c);
#endif
            set_arg1();
            if (opsize == 2)
                write_reg(l1c, 2, read_l(arg1));
            if (opsize == 1)
                write_reg(l1c, 1, read_w(arg1));
        }
    }
    for (l1c = 0; l1c < 8; l1c++)
    {
        if (byte1 & 1 << l1c)
        {
#ifdef LOGDIS
            if (hit)
                push_msg(DASM_ARG2,"/",l1c);
            else
                hit=1;

            push_msg(DASM_ARG2,"a%.1x",l1c);
#endif
            set_arg1();
            if (opsize == 2)
                write_reg(8 + l1c, 2, read_l(arg1));
            if (opsize == 1)
                write_reg(8 + l1c, 1, read_w(arg1));
        }
    }
#ifdef LOGDIS
    arglooping=0;
#endif
}

Item* find_item(int n)
{
    if (n > 0 && n <= itemCount)
    {
        // assume items are sequential and ordered by ID
        return items + (n-1);
    }
    return 0;
}
        
Word* find_adjective(int n)
{
    if (n > 0 && n <= adjectiveCount)
        return adjectives + (n-1);
            
    return 0;
}

extern int emu_word_subst(int gameid, int section,
                          Word* w, const char* p, int len);

size_t _extract_words(int section,
                   type8* sp, type8* ep, size_t recSize, Word** words)
{
    // extract vocab table words
            
    size_t i;
    size_t n = 0;
    type8* p = sp;

    int gameid = get_game();

    while (p != ep) if (*p++ & 0x80) ++n;
            
    *words = (Word*)malloc(n*recSize);
    memset(*words, 0, n*recSize);

    p = sp;
    for (i = 0; i < n; ++i)
    {
        Word* w = (Word*)(((char*)(*words)) + i*recSize);
        char* q;
        int len;
                
        w->index = i + 1;
                
        ep = p;
        while ((*ep++ & 0x80) == 0) ;

        len = ep - p;

        if (emu_word_subst(gameid, section, w, p, len))
        {
            p = ep;
            continue;
        }
                
        q = w->name = (type8*)malloc(len + 1);

        do
        {
            char c = *p++ & 0x7f;
            if (c >= 'A' && c <= 'Z') c += 'a' - 'A'; // lcase
            *q++ = c;
        } while (p != ep);

        *q = 0; // terminate
    }
    return n;
}

static void _dump(type8* p, size_t n)
{
    while (n)
    {
        int i;
        for (i = 0; i < 8; ++i)
        {
            int c = *p++;
            printf("%02x", c);
            if (c < 0x20 || c > 0x7f) c = ' ';
            putchar(c);
            if (!--n) break;
        }
        printf("\n");
    }
}

#ifdef STANDALONE
static type8* _find_data(type8* data, size_t n)
{
    type8* p = code;

    for (;;)
    {
        type8* q = p;
        size_t i;
        for (i = 0; i < n; ++i) if (*q++ != data[i]) break;
        if (i == n) return p;
        ++p;

        if (p - code > 0xFFFF) break;
    }
    return 0;
}
#endif
/*
 * The current room is stored in the writable data part of the game.
 * The memory location is game and version dependend, so the game image
 * must be identified. IDs are stored in byte 13 of the mag file header.
 * IDs and offsets:
 * 0x10 The Pawn 2.3 Acorn     6508
 * 0x11 The Pawn 2.3 C-64      6464
 * 0x12 The Pawn 2.3 CPC       6460
 * 0x13 The Pawn 2.3 PC        6508
 * 0x14 The Pawn 2.4 S+3       6452
 * 0x15 The Pawn 2.3 ZX        6452
 * 0x20 Guild 1.3 Acorn        2520
 * 0x21 Guild 1.0 C-64         9802
 * 0x22 Guild 1.0 CPC          9802
 * 0x23 Guild 1.1 PC           9838
 * 0x24 Guild 1.3 S+3          2446
 * 0x30 Jinxter 1.3 Acorn      1996
 * 0x31 Jinxter 1.05 C-64      2002
 * 0x32 Jinxter 1.22 CPC       1998
 * 0x33 Jinxter 1.05 PC        2004
 * 0x34 Jinxter 1.1 PC         2004
 * 0x35 Jinxter 1.05 S+3       1996
 * 0x40 Corruption 1.12 Acorn  1534
 * 0x41 Corruption 1.11 C-64   1530
 * 0x42 Corruption 1.11 CPC    1528
 * 0x43 Corruption 1.11 PC     1534
 * 0x44 Corrutpion 1.11 S+3    1530
 * 0x50 Fish! 1.08 Acorn       1194
 * 0x51 Fish! 1.07 C-64        1188
 * 0x52 Fish! 1.02 PC          1192
 * 0x53 Fish! 1.03 S+3         1636
 * 0x60 Myth 1.0 C-64          650
 * 0x61 Myth 1.0 PC            656
 * 0x70 Wonderland 1.27 Acorn  2872
 * 0x71 Wonderland 1.21 PC     2872
 * 0x80 Guild MW 1.3           2456
 * 0x90 Corruption MW 1.12     2088
 * 0xA0 Fish MW 1.10           2270
 */
static type16 _get_set_current_room(type16 v)
{
    int d = 0;
    if (prog_format)
    {
        static int s_room;
        if (!s_room) s_room = get_sym_value("ROOM");
        d = s_room;
    }
    else
    {
        static const size_t offset[10][6] = { {6508,6464,6460,6508,6452,6452},
                                              {2520,9802,9802,9838,2446,0},
                                              {1996,2002,1998,2004,2004,1996},
                                              {1534,1530,1528,1534,1530,0},
                                              {1194,1188,1192,1636,0,0},
                                              {650,656,0,0,0,0},
                                              {2872,2872,0,0,0,0},
                                              {2456,0,0,0,0,0},
                                              {2088,0,0,0,0,0},
                                              {2270,0,0,0,0,0} };

        d = offset[(gameid >> 4)-1][(gameid & 0x0F)];
    }

    if (d)
    {
        type8* p = code+d;
        //printf("room# at %04x, current value %d\n", d, _read_w(p));
        if (v) write_w(p, v);
        d = read_w(p);
    }

    if (!d) d = 1; // default to #1
    
    return d;
    
}

type16 get_current_room() 
{
    return _get_set_current_room(0);
}

type16 set_current_room(type16 v)
{
    return _get_set_current_room(v);
}

const type8* get_psudat()
{
    // return the address of the psudat table.
    if (prog_format)
    {
        static int s_psudat;
        if (!s_psudat) s_psudat = get_sym_value("PSUDAT");
        assert(s_psudat);
        return code + s_psudat;
    }
    else
    {
        int g = get_game();
        if (g == 1)
        {
            // the pawn.
            // XXX will be in different locations for each version
            return code + 0x1ab8;
        }
    }
    return 0;
}

static type8* get_npcdata()
{
    if (prog_format)
    {
        static int s_npcdata;
        if (!s_npcdata) s_npcdata = get_sym_value("NPC.DATA");
        assert(s_npcdata);
        return code + s_npcdata;
    }
    else
    {
        // there are 4 bytes of data per NPC (see emux.h)
        // the table is terminated by 4 bytes of zeros.
        int g = get_game();            
        if (g == 1)
        {
            return code + 0x1b5e;
        }
    }
    return 0;
}

type16 get_object_player_on()
{
    // players can be on objects (chairs, horses etc).
    if (prog_format)
    {
        static int s_seated;
        if (!s_seated) s_seated = get_sym_value("SEATED");
        assert(s_seated);
        return read_w(code + s_seated);
    }
    else
    {
        int g = get_game();            
        if (g == 1)
        {
            return read_w(code + 0x1a0a);
        }
    }
    return 0;            
}

type16 get_stopron()
{
    // get the stored pronoun (stopron).
    // only valid for prog_format binaries.
    //
    // this pronoun is used for ' @ ' in messages to substitute
    // "the object" as decoded by `tnWord`
    //
    int obj = 0;
    if (prog_format)
    {
        static int s_stopron;
        if (!s_stopron) s_stopron = get_sym_value("STOPRON");
        assert(s_stopron);
        obj = read_w(code + s_stopron);
    }
    return obj;
}

type16 get_cantsave()
{
    // GUILD remastered onward has a symbol "CANTSAVE" which contains
    // a flag indicating the auto save should not be performed.
    //
    // this flag is set when, for example, a timer is started that could
    // involve the player dieing unless certain actions are taken.
    // the upshot of not autosaving is that the _last_ autosave will be
    // immediately before this timer begins and constitutes a sensible
    // restore point.
    
    type16 val = 0;
    if (prog_format)
    {
        static int s_cantsave;
        if (!s_cantsave)
        {
            static int no_cantsave;
            if (!no_cantsave)
            {
                s_cantsave = get_sym_value("CANTSAVE");

                // we don't have this symbol, so dont try again
                if (!s_cantsave) no_cantsave = 1;
            }
        }

        if (s_cantsave) val = read_w(code + s_cantsave);
    }
    return val;
}

#define DEF_BYTE_SYM(_sym)                              \
static type8* get_ ## _sym ## _addr()                   \
{                                                       \
    type8* addr = 0;                                    \
    if (prog_format)                                    \
    {                                                   \
        static int s_addr;                              \
        if (!s_addr) s_addr = get_sym_value(#_sym);     \
        assert(s_addr);                                 \
        addr = code + s_addr;                           \
    }                                                   \
    return addr;                                        \
}                                                       \
type8 get_ ## _sym ()                                   \
{                                                       \
    type8* a = get_ ## _sym ## _addr();                 \
    return a ? *a : 0;                                  \
}                                                       \
int set_ ## _sym (int v)                                \
{                                                       \
    type8* a = get_ ## _sym ## _addr();                 \
    if (a)                                              \
    {                                                   \
        *a = (type8)v;                                  \
        return 1;                                       \
    }                                                   \
    return 0;                                           \
}

// get the current output mode;
// 0 => normal, 1=>verbose, 2=>brief
// set the current output mode
// return 1 for success (ie prog_format), else 0
DEF_BYTE_SYM(OUTMODE);

// new or classic mode
DEF_BYTE_SYM(REMASTER);

// get the current picture as understood by the game
DEF_BYTE_SYM(PICTNUM);

type8* getcode()
{
    return code;
}

/*
 * Games:
 * 01 The Pawn
 * 02 Guild of Thieves
 * 03 Jinxter
 * 04 Corruption
 * 05 Fish!
 * 06 Myth
 * 07 Wonderland
 * 08 Guild (MW)
 * 09 Corruption (MW)
 * 10 Fish! (MW)
 */
type16 get_game()
{
    return (gameid >> 4);
}

static void _scan_synonyms(type16 synlist)
{
#if 0
    if (version > 0)
    {
        type8* synp = effective(synlist);

        //printf("synlist = %x\n", synlist);
        //printf("%08X\n", (int)synp);
        //printf("version = %d\n", version);

        for (int i = 0; i < 20; ++i)
        {
            int v = read_w(synp);
            int section = v & 0x1f;
            int word = (v >> 5) & 0x7ff;
            synp += 2;
            printf("syn: word %d, section %d\n", word, section);
        }
    }
#endif 
}

static void _scan_adjectives(type16 adjlist)
{
    type8* adjp = effective(adjlist);

    int v = 0; // debug

    size_t i;
    for (i = 1; i <= itemCount; ++i)
    {
        Item* ii = find_item(i);
        int ac = 0;
        if (*adjp)
        {
            if (v) printf("(%d %s)",  (int)i, ii->word.name);
            do
            {
                int a = *adjp - 3; // why, i don't know.
                        
                if (ac < ITEM_MAX_ADJ)
                    ii->adj[ac++] = a;

                if (v)
                {
                    Word* aw = find_adjective(a);
                    if (aw) printf(" %s", aw->name);
                    printf("(%02x)", *adjp);
                }
                        
            } while (*++adjp);
                  
            if (v) printf("\n");
        }
        ++adjp;
    }
}

static void _init_items()
{
    size_t i;

    unsigned char* baseprop;
    unsigned char* roomprop;
            
    for (i = 1; i <= itemCount; ++i)
    {
        Item* ii = find_item(i);
        assert(ii);
        type16 ptr;

        // the id is the item number
        ii->id = i;

        // initialise item data
        write_reg(0, 1, i); // d0 = item
        do_findprop(); // props of d0 -> a0
        ptr = (type16)read_reg(8 + 0, 1);  // a0
        ii->data = effective(ptr);

        if (i == 1)
        {
            // base of item data table
            baseprop = ii->data;

            if (prog_format)
            {
                static int s_network;
                if (!s_network) s_network = get_sym_value("NETWORK");
                roomprop = code + s_network;
            }
            else
            {
                // XX ASSUME room connection data follows item data

                roomprop = baseprop + 14*itemCount;

                // Guild
                if (gameid==0x21 || gameid==0x22 || gameid==0x23) // Guild < 1.3, offset 2 bytes low
                    roomprop += 2;

                // Jinxter 1.0 shows some oddities with map update, later versions are fixed

                // Corruption
                // itemCount is 339, but item table seems to only have 284 entries, count back 55 entries
                if (get_game()==4)
                    roomprop -= 14*55;

                // Fish!
                // calculated offset is 210 bytes too high
                if (get_game()==5)
                    roomprop -= 210;

                // Myth
                // calculated offset is 154 bytes too high
                if (get_game()==6)
                    roomprop -= 154;

                // Wonderland
                // calculated offset is 1942 bytes too high
                if (get_game()==7)
                    roomprop -= 1942;

                // Guild MW
                // calculated offset is 4 bytes too low
                if (get_game()==8)
                    roomprop += 4;

                // Corruption MW
                // calculated offset is 764 bytes too high
                if (get_game()==9)
                    roomprop -= 764;

                // Fish MW
                // calculated offset is 206 bytes too high
                if (get_game()==10)
                    roomprop -= 206;
            }
        }
                
        if (ii->data[6] & 0x40) // is a room
        {
            int r = read_w(ii->data + 8);
            if (r > maxRoom) maxRoom = r;
        }
    }

#if 0
    printf("baseprop = %x\n", (unsigned int)(baseprop - code));
    printf("roomprop = %x\n", (unsigned int)(roomprop - code));
    printf("itemCount: %d\n",itemCount);
    printf("maxRoom: %d\n", maxRoom);
#endif

    // The Pawn has 10 bytes/room, later games 12 bytes/room
    bytesPerRoom = 10;
    if (get_game() > 1) bytesPerRoom = 12;

    npcCount = 0;
    for (i = 1; i <= itemCount; ++i)
    {
        Item* ii = find_item(i);
        if (ii->data[6] & 0x40) // room?
        {
            int r = read_w(ii->data + 8);
                    
            // The Pawn has 10 bytes/room, later games 12 bytes/room
            ii->exitData = roomprop + bytesPerRoom*(r - 1);
        }

        if (ii->data[4] & 0x40) // is npc?
        {
            type8* np = get_npcdata();
            if (np)
            {
                ii->npcData = np + npcCount*4;
            }
            ++npcCount;
        }
    }

    // over in emux
    init_items();
}

static void _scan_dict(int doff)
{
    // we scan the dictionary memory and extract words for items
    // and adjectives.
            
    // print out as we go for debugging...
    int v = 0; // verbose
            
    type8* dp = sd ? dict + doff : effective(doff);
    int sec = 0;
    int word = 0;
    int atword = 1;

    // offset of sections
    type8* section[32];
    section[0] = dp;
            
    for (;;)
    {
        if (atword)
        {
            if (v) printf("(%d) ", word);
            atword = 0;
        }
                    
        while ((*dp & 0x80) == 0)
        {
            if (v) putchar(*dp);
            ++dp;
        }

        if (*dp == 0x81)
        {
            // end of vocab table
            section[++sec] = ++dp;
            break;
        }
        else if (*dp == 0x82)
        {
            ++sec;
            if (v) printf("###### section %d\n", sec);
            word = 0;
            ++dp;

            section[sec] = dp;
        }
        else
        {
            if (v)
            {
                putchar(*dp & 0x7f);
                putchar('\n');
            }
            ++word;
            ++dp;
        }

        atword = 1;
    }

    adjectiveCount = _extract_words(5, section[5], section[6]-1,
                                 (int)sizeof(Word), &adjectives);
    if (v) printf("%d adjectives\n", (int)adjectiveCount);
            
    // make items
    itemCount = _extract_words(6, section[6], section[7]-1, sizeof(Item),
                            (Word**)&items);
    if (v) printf("%d nouns\n", (int)itemCount);
}

/* [30e4] in Jinxter, ~540 lines of 6510 spaghetti-code */
/* The mother of all bugs, but hey - no gotos used :-) */

void dict_lookup(void)
{
    type16 dtab, doff, output, output_bak, bank, word, output2;
    type16 tmp16, i, obj_adj, adjlist, adjlist_bak;
    type8 c, c2, c3, flag, matchlen, longest, flag2;
    type8 restart = 0, accept = 0;

    /*
      dtab=A5.W                    ;dict_table offset <L22>
      output=output_bak=A2.W       ;output <L24>
      A6.W                         ;input word
      A5.W =                       ; synonym table, if > pawn
      doff=A3.W                    ;lookup offset (doff) <L1C>
      adjlist=A0.W ;adjlist <L1E>
    */

    dtab = (type16)read_reg(8 + 5, 1);      /* used by version>0 */
    output = (type16)read_reg(8 + 2, 1);
    write_reg(8 + 5, 1, read_reg(8 + 6, 1));
    doff = (type16)read_reg(8 + 3, 1);
    adjlist = (type16)read_reg(8 + 0, 1);

    // first time in, scan dictionary and extract info
    if (!items)
    {
        _scan_dict(doff);
        _scan_adjectives(adjlist);
        _scan_synonyms(dtab);

        // build item data
        _init_items();
    }
                    
    bank = (type16)read_reg(6, 0);  /* l2d */
    flag = 0;               /* l2c */
    word = 0;               /* l26 */
    matchlen = 0;           /* l2e */
    longest = 0;            /* 30e2 */
    write_reg(0, 1, 0);     /* apostroph */

    while ((c = sd ? dict[doff] : read_b(effective(doff))) != 0x81)
    {
        if (c >= 0x80)
        {
            if (c == 0x82)
            {
                flag = matchlen = 0;
                word = 0;
                write_reg(8 + 6, 1, read_reg(8 + 5, 1));
                bank++;
                doff++;
                continue;
            }
            c3 = c;
            c &= 0x5f;
            c2 = read_b(effective(read_reg(8 + 6, 1))) & 0x5f;
            if (c2 == c)
            {
                write_reg(8 + 6, 1, read_reg(8 + 6, 1) + 1);
                c = read_b(effective(read_reg(8 + 6, 1)));
                if ((!c) || (c == 0x20) || (c == 0x27) || (!version && (matchlen > 6)))
                {
                    if (c == 0x27)
                    {
                        write_reg(8 + 6, 1, read_reg(8 + 6, 1) + 1);
                        write_reg(0, 1, 0x200 + read_b(effective(read_reg(8 + 6, 1))));
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
            c2 = read_b(effective(read_reg(8 + 6, 1))) & 0x5f;
            if ((c2 == c && c) || (version && !c2 && (c == 0x5f)))
            {
                if (version && !c2 && (c == 0x5f))
                    flag = 0x80;
                matchlen++;
                write_reg(8 + 6, 1, read_reg(8 + 6, 1) + 1);
                doff++;
            }
            else if (!version && matchlen > 6 && !c2)
                accept = 1;
            else
                restart = 1;
        }
        if (accept)
        {
            write_b(effective(read_reg(8 + 2, 1)), (version) ? flag : 0);
            write_b(effective(read_reg(8 + 2, 1)) + 1, (type8)bank);
            write_w(effective(read_reg(8 + 2, 1) + 2), word);
            write_reg(8 + 2, 1, read_reg(8 + 2, 1) + 4);
            if (matchlen >= longest)
                longest = matchlen;
            restart = 1;
            accept = 0;
        }
        if (restart)
        {
            write_reg(8 + 6, 1, read_reg(8 + 5, 1));
            flag = matchlen = 0;
            word++;
            if (sd)
                while (dict[doff++] < 0x80);
            else
                while (read_b(effective(doff++)) < 0x80);
            restart = 0;
        }
    }
    write_w(effective(read_reg(8 + 2, 1)), 0xffff);

    if (version)
    {
        /* version > 0 */
        output_bak = output;    /* check synonyms */
        while ((c = effective(output)[1]) != 0xff)
        {
            if (c == 0x0b)
            {
                if (sd)
                    tmp16 = read_w(&dict[dtab + read_w(effective(output + 2)) * 2]);
                else
                    tmp16 = read_w(effective(dtab + read_w(effective(output + 2)) * 2));
                effective(output)[1] = tmp16 & 0x1f;
                write_w(effective(output + 2), (type16)(tmp16 >> 5));
            }
            output += 4;
        }
        output = output_bak;
    }

    /* l22 = output2,     l1e = adjlist, l20 = obj_adj, l26 = word, l2f = c2 */
    /* l1c = adjlist_bak, 333C = i,      l2d = bank,    l2c = flag, l30e3 = flag2 */

    write_reg(1, 1, 0);     /* D1.W=0  [32B5] */
    flag2 = 0;
    output_bak = output;
    output2 = output;
    while ((bank = effective(output2)[1]) != 0xff)
    {
        obj_adj = (type16)read_reg(8 + 1, 1);   /* A1.W - obj_adj, ie. adjs for this word */
        write_reg(1, 0, 0);     /* D1.B=0 */
        flag = read_b(effective(output2));   /* flag */
        word = read_w(effective(output2 + 2));  /* wordnumber */
        output2 += 4;   /* next match */
        if ((read_w(effective(obj_adj))) && (bank == 6))
        {
            /* Any adjectives? */
            if ((i = word) != 0)
            {
                /* Find list of valid adjs */
                do
                {
                    while (read_b(effective(adjlist++)));
                }
                while (--i > 0);
            }
            adjlist_bak = adjlist;
            do
            {
                adjlist = adjlist_bak;
                c2 = effective(obj_adj)[1];     /* given adjective */
                if ((tmp16 = read_w(effective(obj_adj))) != 0)
                {
                    obj_adj += 2;
                    while ((c = read_b(effective(adjlist++))) && (c - 3 != c2));
                    if (c - 3 != c2)
                        write_reg(1, 0, 1);     /* invalid adjective */
                }
            }
            while (tmp16 && !read_reg(1, 0));
            adjlist = (type16)read_reg(8 + 0, 1);
        }
        if (!read_reg(1, 0))
        {
            /* invalid_flag */
            flag2 |= flag;
            write_b(effective(output), flag2);
            write_b(effective(output) + 1, (type8)bank);
            write_w(effective(output + 2), word);
            output += 4;
        }
    }
    write_reg(8 + 2, 1, output);
    output = output_bak;

    if (flag2 & 0x80)
    {
        tmp16 = output;
        output -= 4;
        do
        {
            output += 4;
            c = read_b(effective(output));
        }
        while (!(c & 0x80));
        write_l(effective(tmp16), read_l(effective(output)) & 0x7fffffff);
        write_reg(8 + 2, 2, tmp16 + 4);
        if (longest > 1)
        {
            write_reg(8 + 5, 1, read_reg(8 + 5, 1) + longest - 2);
        }
    }
    write_reg(8 + 6, 1, read_reg(8 + 5, 1) + 1);
}

/* A0=findproperties(D0) [2b86], properties_ptr=[2b78] A0FE */

void do_findprop(void)
{
    type16 tmp;

    if ((version > 2) && ((read_reg(0, 1) & 0x3fff) > fp_size))
    {
        tmp = (type16)(((fp_size - (read_reg(0, 1) & 0x3fff)) ^ 0xffff) << 1);
        tmp += fp_tab;
        tmp = read_w(effective(tmp));
    }
    else
    {
        if (version < 2)
            write_reg(0, 2, read_reg(0, 2) & 0x7fff);
        else
            write_reg(0, 1, read_reg(0, 1) & 0x7fff);
        tmp = (type16)read_reg(0, 1);
    }
    tmp &= 0x3fff;
    write_reg(8 + 0, 2, tmp * 14 + properties);
}

static int _write_string(type16 ptr, type8* more, outfn fn)
{
    static type32 offset_bak;
    static type8 mask_bak;
    type8 c, b, mask;
    type32 offset;

    // return non-zero if done
    if (msg_write_string(ptr, char_out)) return 0; // handled
    
    // if strings not loaded
    if (!decode_table) return 0;

    if (!*more)
    {
        /* new string */
                
        if (!ptr)
            offset = 0;
        else
        {
            offset = read_w(&decode_table[0x100 + 2 * ptr]);
            if (read_w(&decode_table[0x100]))
            {
                if (ptr >= read_w(&decode_table[0x100]))
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

    do
    {
        c = 0;
        while (c < 0x80)
        {
            if (offset >= string_size)
            {
                assert(string2);
                b = string2[offset - string_size];
            }
            else
            {
                if (offset >= MAX_STRING_SIZE)
                    b = string3[offset - MAX_STRING_SIZE];
                else
                    b = string[offset];
            }
            if (b & mask)
                c = decode_table[0x80 + c];
            else
                c = decode_table[c];
            mask <<= 1;
            if (!mask)
            {
                mask = 1;
                offset++;
            }
        }
        c &= 0x7f;
        if (c && (c != '@' || lastchar != ' '))
            _char_out(c, fn);
        
    } while (c && (c != '@' || lastchar != ' '));
    
    *more = c ? 0xff : 0;
    if (c)
    {
        offset_bak = offset;
        mask_bak = mask;
    }
    return *more;
}

#ifdef STANDALONE
int decode_string(int n, outfn fn)
{
    // return 1 if ok
    type8 more = 0;
    lastchar = 0;
    big = 0;
    period = 0;
    epipe = 0;
    escape = 1;
    if (_write_string(n, &more, fn)) return 0; // fail
    return 1;
}
#endif

void write_string(void)
{
    type16 ptr = (type16)read_reg(0, 1);
    _write_string(ptr, &cflag, ms_putchar);
}

void output_number(type16 number)
{
    type16 tens = number / 10;

    if (tens > 0)
        ms_putchar('0'+tens);
    number -= (tens*10);
    ms_putchar('0'+number);
}

type16 output_text(const type8* text)
{
    type16 i;

    for (i = 0; text[i] != 0; i++)
        ms_putchar(text[i]);
    return i;
}

type16s hint_input(void)
{
    type8 c1, c2, c3;

    output_text(">>");
    ms_flush();

    do
    {
        c1 = _ms_getchar(0);
    }
    while (c1 == '\n');
    if (c1 == 1)
        return -1; /* New game loaded, abort hints */

    c2 = _ms_getchar(0);
    if (c2 == 1)
        return -1;

    /* Get and discard any remaining characters */
    c3 = c2;
    while (c3 != '\n')
    {
        c3 = _ms_getchar(0);
        if (c3 == 1)
            return -1;
    }
    ms_putchar('\n');

    if ((c1 >= '0') && (c1 <= '9'))
    {
        type16 number = c1 - '0';
        if ((c2 >= '0') && (c2 <= '9'))
        {
            number *= 10;
            number += c2 - '0';
        }
        return number;
    }

    if ((c1 >= 'A') && (c1 <= 'Z'))
        c1 = 'a' + (c1 - 'A');
    if ((c1 >= 'a') && (c1 <= 'z'))
    {
        switch (c1)
        {
        case 'e':
            return -2; /* End hints */
        case 'n':
            return -3; /* Next hint */
        case 'p':
            return -4; /* Show parent hint list */
        }
    }
    return 0;
}

type16 show_hints_text(struct ms_hint* hints, type16 index)
{
    type16 i = 0, j = 0;
    type16s input;
    struct ms_hint* hint = hints+index;

    while (1)
    {
        switch (hint->nodetype)
        {

        case 1: /* folders */
            output_text("Hint categories:\n");
            for (i = 0, j = 0; i < hint->elcount; i++)
            {
                output_number(i+1);
                output_text(". ");
                j += output_text(hint->content+j)+1;
                ms_putchar('\n');
            }
            output_text("Enter hint category number, ");
            if (hint->parent != 0xffff)
                output_text("P for the parent hint menu, ");
            output_text("or E to end hints.\n");

            input = hint_input();
            switch (input)
            {
            case -1: /* A new game is being loaded */
                return 1;
            case -2: /* End hints */
                return 1;
            case -4: /* Show parent hint list */
                if (hint->parent != 0xffff)
                    return 0;
            default:
                if ((input > 0) && (input <= hint->elcount))
                {
                    if (show_hints_text(hints,hint->links[input-1]) == 1)
                        return 1;
                }
                break;
            }
            break;

        case 2: /* hints */
            if (i < hint->elcount)
            {
                output_number(i+1);
                output_text(". ");
                j += output_text(hint->content+j)+1;

                if (i == hint->elcount-1)
                {
                    output_text("\nNo more hints.\n");
                    return 0; /* Last hint */
                }
                else
                {
                    output_text("\nEnter N for the next hint, ");
                    output_text("P for the parent hint menu, ");
                    output_text("or E to end hints.\n");
                }

                input = hint_input();
                switch (input)
                {
                case -1: /* A new game is being loaded */
                    return 1;
                case -2: /* End hints */
                    return 1;
                case -3:
                    i++;
                    break;
                case -4: /* Show parent hint list */
                    return 0;
                }
            }
            else
                return 0;
            break;
        }
    }
    return 0;
}

#define InfoAtScreen 1
#define InfoAtCommand 2
#define InfoDump 3

void do_line_a(void)
{
    type8 l1c;
    type8s *str;
    type16 ptr, ptr2, tmp16, dtype;
    type32 a1reg, tmp32;

#ifdef LOGGFX
    /*
      if (((byte2-0xdd) == 4) || ((byte2-0xdd) == 13))
      out2("--> %d\n",byte2-0xdd);
      else
      out2("LINE A %d\n",byte2-0xdd);
    */
#endif
    int vcheck = (version < 4 && byte2 < 0xe4) || (version < 2 && byte2 < 0xed);
    if ((byte2 < 0xdc) || (!prog_format && vcheck))
    {
        ms_flush();     /* flush output-buffer */
        rand_emu();     /* Increase game randomness */
        l1c = _ms_getchar(1);    /* 0 means UNDO */
        if (l1c == 1)
            return;
        if (l1c)
            write_reg(1, 2, l1c);   /* d1=getkey() */
        else
        {
            if ((l1c = ms_undo()) != 0)
                output_text(undo_ok);
            else
                output_text(undo_fail);
            if (!l1c)
                write_reg(1, 2, '\n');
        }
    }
    else
    {
        if (byte2 == 0xDC)
        {
            static int lastScnMsgCount;

            // A0DC
            int op = pop16();

            //printf("##### InfoTrap %d\n", op);

            switch (op)
            {
            case InfoAtScreen:
                // called at the start of the main loop
                lastScnMsgCount = ScnMsgCount;
                break;
            case InfoAtCommand:
                // called before a command is parsed
                {
                    int d = ScnMsgCount - lastScnMsgCount;
                    ms_event_hook(!d);
                }
                break;
            case InfoDump:
                {
                    int a = pop32();
                    printf("dump address %08X\n", a);
                    _dump(effective(a), 16);
                }
                break;
            }
        }
        
        //printf("LINE-A %d\n", byte2 - 0xdd);
        switch (byte2 - 0xdd)
        {

        case 0: /* A0DD - Won't probably be needed at all */
            break;

        case 1: /* A0DE */
            write_reg(1, 0, 1);     /* Should remove the manual check */
            break;

        case 2: /* A0DF */ 
            // NOTIFY 

            a1reg = (type32)read_reg(9,2);
            dtype = read_b(code+a1reg+2);

            switch (dtype)
            {
            case 7: /* Picture */
#ifdef LOGGFX
                out2("PICTURE IS %s\n", code + a1reg + 3);
#endif
#ifdef LOGDIS
                push_msg(DASM_COM,"Show picture");
#endif
                /* gfx mode = normal, df is not called if graphics are off */
                //_ms_showpic(a1reg + 3, 2);
                break;

            case 10: /* Open window commands */
#ifdef LOGDIS
                push_msg(DASM_COM,"Open window");
#endif
                switch (read_b(code+a1reg+3))
                {
                case 4: /* Help/Hints */
                    if (hints != 0)
                    {
                        if (ms_showhints(hints) == 0)
                            show_hints_text(hints,0);
                    }
                    else
                        output_text(no_hints);
                    break;
                case 0: /* Carried items */
                case 1: /* Room items */
                case 2: /* Map */
                case 3: /* Compass */
                    output_text(not_supported);
                    break;
                }
                break;

            case 13: /* Music */
#ifdef LOGDIS
                push_msg(DASM_COM,"Play music");
#endif
                switch (read_b(code+a1reg+3))
                {
                case 0: /* stop music */
                    ms_playmusic(0,0,0);
                    break;
                default: /* play music */
#ifdef LOGSND
                    out2("MUSIC IS %s\n", code + a1reg + 3);
#endif
                    {
                        type32 length = 0;
                        type16 tempo = 0;
                        type8* midi = sound_extract(code + a1reg + 3,&length,&tempo);
                        if (midi != NULL)
                            ms_playmusic(midi,length,tempo);
                    }
                    break;
                }
                break;
            }
            break;

        case 3: /* A0E0 */ // PROMPT_EV
#ifdef LOGDIS
            push_msg(DASM_COM,"NOT IMPLEMENTED");
#endif
            break;

        case 4: /* A0E1 Read from keyboard to (A1), status in D1 (0 for ok) */
#ifdef LOGDIS
            // GET_NEXT_EV

            push_msg(DASM_COM,"Read from keyboard to (A1), status in D1 (0 for ok)");
#endif
            ms_flush();
            rand_emu();
            tmp32 = read_reg(8 + 1, 2);
            str = (type8s*)effective(tmp32);
            tmp16 = 0;
            do
            {
                if (!(l1c = _ms_getchar(1)))
                {
                    if ((l1c = ms_undo()) != 0)
                        output_text(undo_ok);
                    else
                        output_text(undo_fail);
                    if (!l1c)
                    {
                        tmp16 = 0;
                        str[tmp16++] = '\n';
                        l1c = '\n';
                        output_text("\n>");
                    }
                    else
                    {
                        ms_putchar('\n');
                        return;
                    }
                }
                else
                {
                    if (l1c == 1)
                        return;
                    str[tmp16++] = l1c;
#ifdef LOGGFX
                    out2("%c",l1c);
#endif
                }
            }
            while (l1c != '\n' && tmp16 < 256);
            write_reg(8 + 1, 2, tmp32 + tmp16 - 1);
            if (tmp16 != 256 && tmp16 != 1)
                write_reg(1, 1, 0);
            else
                write_reg(1, 1, 1);
            break;

        case 5: /* A0E2 */
             // UNDO_DIFF

            ms_undo_signal();
            break;

        case 6: /* A0E3 */
            // MAKE_UNDO
            // MAKE_DIFF
            if (prog_format)
            {
                int n = read_reg(0,1); // D0 = CLOCK
                game_state_notify(n);
            }
            break;

        case 7: /* A0E4 sp+=4, RTS */
            // POPSP_RET
#ifdef LOGDIS
            push_msg(DASM_COM,"sp+=4, RTS");
#endif
            write_reg(8 + 7, 1, read_reg(8 + 7, 1) + 4);
#ifdef LOGDIS
            if (!no_branch)
#endif
            pc = pop();
            break;

        case 8: /* A0E5 set z, RTS */
            /* SETEQ_RET
               MOVE	#4,CCR
               RET
             */
        case 9: /* A0E6 clear z, RTS */
            /* SETNE_RET
               MOVE	#0,CCR
               RET
             */
#ifdef LOGDIS
            push_msg(DASM_COM,"set z/clear z, RTS");
            if (!no_branch)
#endif
                pc = pop();
            zflag = (byte2 == 0xe5) ? 0xff : 0;
            break;

        case 10: /* A0E7 set z */
            // SETEQ
            // MOVE    #4,CCR             ;SET ZERO FLAG

#ifdef LOGDIS
            push_msg(DASM_COM,"set z");
#endif
            zflag = 0xff;
            break;

        case 11: /* A0E8 clear z */
            // SETNE
            // MOVE    #0,CCR             ;CLEAR ZERO FLAG

#ifdef LOGDIS
            push_msg(DASM_COM,"clear z");
#endif
            zflag = 0;
            break;

        case 12: /* A0E9 [3083 - j] */
            // COPYWORD
            // copy hibit term string form (A1) to (A0)

            ptr = (type16)read_reg(8 + 0, 1);
            ptr2 = (type16)read_reg(8 + 1, 1);
            do
            {
                l1c = dict[ptr2++];
                write_b(effective(ptr++), l1c);
            }
            while ((l1c & 0x80) == 0);
            write_reg(8 + 0, 1, ptr);
            write_reg(8 + 1, 1, ptr2);
            break;

        case 13: /* A0EA A1=write_dictword(A1,D1=output_mode) */
            // PrtWTabWord
            // print hibit terminated string -> A1
            // D1 = CHANNEL

            ptr = (type16)read_reg(8 + 1, 1); // A1
            tmp32 = read_reg(3, 0);
            write_reg(3, 0, read_reg(1, 0));
            do
            {
                l1c = dict[ptr++];
                char_out(l1c);
            }
            while (l1c < 0x80);
            write_reg(8 + 1, 1, ptr);
            write_reg(3, 0, tmp32);
            break;

        case 14: /* A0EB [3037 - j] */
            // PutWTabByte

            dict[read_reg(8 + 1, 1)] = (type8)read_reg(1, 0);
            break;

        case 15: /* A0EC */
            // GetWTabByte
            write_reg(1, 0, dict[read_reg(8 + 1, 1)]);
            break;

        case 16:
            // QUIT
            /* infinite loop A0ED */
            ms_stop_or_restart(0); // stop
            break;
        case 17:
            /* restart game ie. pc, sp etc. A0EE */
            // RESTART
            ms_stop_or_restart(1); // restart
            break;
        case 18:        /* printer A0EF */
            // PRINTER
            break;
        case 19: // A0F0 GRAPHICS
            {
                // gfx_ver >= 2 will pass a code offset

                int picNumber = read_reg(0,1); // d0
                int picAddr = 0; // not known
                int picOp = read_reg(1, 0); // d1. 1 = show, 0 = hide
                int picVer = read_reg(2, 0); // d2 = picture version
                int room = read_reg(3, 1); // d3.w = room

                // Quick fix for missing pic_ver in guild, needs proper fix
                //if (!picOp || !prog_format) picVer = 0;
                if (!picOp || !prog_format || (get_game() < 3)) picVer = 0;

                if (gfx_ver > 1 || prog_format)
                    picAddr = read_reg(8,1); // A0
                
                /* Do_picture(D0) A0F0 */
                _ms_showpic(picNumber, picAddr, picOp, picVer);
            }
            break;
        case 20: // A0F1 FINDZERO

            /* 
               FINDZERO -  finds the Nth zero in a list
               
               On entry    A1.L points at start of list
               D0.W is the no. of zeros (-1) to find.

               On exit     A1.L points at byte after Nth zero
               D0.W = -1 (optional)
            */

            /* A1=nth_string(A1,D0) A0F1 */
            ptr = (type16)read_reg(8 + 1, 1);

            tmp32 = read_reg(0, 1);
            while (tmp32-- > 0)
            {
                while (read_b(effective(ptr++))) ;
            }
            write_reg(8 + 1, 1, ptr);
            break;

        case 21: /* [2a43] A0F2 */
            // GETTIED
            cflag = 0;
            write_reg(0, 1, read_reg(2, 1));
            do_findprop();
            ptr = (type16)read_reg(8 + 0, 1);
            while (read_reg(2, 1) > 0)
            {
                if (read_w(effective(ptr + 12)) & 0x3fff)
                {
                    cflag = 0xff;
                    break;
                }
                if (read_reg(2, 1) == (read_reg(4, 1) & 0x7fff))
                {
                    cflag = 0xff;
                    break;
                }
                ptr -= 0x0e;
                write_reg(2, 1, read_reg(2, 1) - 1);
            }
            break;

        case 22:
            // CHAROUT2
            char_out((type8)read_reg(1, 0));        /* A0F3 */
            break;

        case 23: /* D7=Save_(filename A0) D1 bytes starting from A1  A0F4 */
            // SAVEFILE
            {
#ifdef LOGDIS
                push_msg(DASM_COM,"D7=Save_(filename A0) D1 bytes starting from A1");
#endif
                type16 size;
                type32 pos;
                        
                str = (version < 4) ? (type8s*)effective(read_reg(8 + 0, 1)) : 0;
                pos = read_reg(8 + 1, 1); // A1.L
                size = (type16)read_reg(1, 1); // D1.W

                //printf("save at %x, size %d (%x)\n", pos, (int)size, (int)size);
                write_reg(7, 0,
                          ms_save_file(str, effective(pos), size, pos));
            }
            break;

        case 24: /* D7=Load_(filename A0) D1 bytes starting from A1  A0F5 */
            // LOADFILE
            {
#ifdef LOGDIS
                push_msg(DASM_COM,"D7=Load_(filename A0) D1 bytes starting from A1");
#endif
                type16 size;
                type32 pos;
                        
                str = (version < 4) ? (type8s*)effective(read_reg(8 + 0, 1)) : 0;
                pos = read_reg(8 + 1, 1);
                size = (type16)read_reg(1, 1);

                //printf("load at %x, size %d (0x%x)\n", pos, (int)size, (int)size);
                write_reg(7, 0, ms_load_file(str, effective(pos), size, pos));
            }
            break;

        case 25: /* D1=Random(0..D1-1) [3748] A0F6 */
            // RANDOM2
#ifdef LOGDIS
            push_msg(DASM_COM,"D1=Random(0..D1-1)");
#endif
            {
                l1c = (type8)read_reg(1, 0);
                int v = rand_emu() % (l1c ? l1c : 1);
                //printf("RANDOM(%d) = %d\n", l1c, v);
                write_reg(1, 1, v);
            }
            break;

        case 26: /* D0=Random(0..255) [3742] A0F7 */
            // RANDOM1
#ifdef LOGDIS
            push_msg(DASM_COM,"D0=Random(0..255)");
#endif
            tmp16 = (type16)rand_emu();
            write_reg(0, 0, tmp16 + (tmp16 >> 8));
            break;

        case 27: /* write string [D0] [2999] A0F8 */
            // TEXT
#ifdef LOGDIS
            push_msg(DASM_COM,"write string [D0]");
#endif
            write_string();
            break;

        case 28: /* Z,D0=Get_inventory_item(D0) [2a9e] A0F9 */
            // QCARRIED

#ifdef LOGDIS
            push_msg(DASM_COM,"Z,D0=Get_inventory_item(D0)");
#endif
            zflag = 0;
            ptr = (type16)read_reg(0, 1);
            do
            {
                write_reg(0, 1, ptr);
                do
                {
                    do_findprop();
                    ptr2 = (type16)read_reg(8 + 0, 1);      /* object properties */
                    if ((effective(ptr2)[5]) & 1)
                        break;  /* is_described or so */
                    l1c = effective(ptr2)[6];       /* some_flags */
                    tmp16 = read_w(effective(ptr2 + 8));    /* parent_object */
                    if (!l1c)
                    {
                        /* ordinary object? */
                        if (!tmp16)
                            zflag = 0xff;   /* return if parent()=player */
                        break;  /* otherwise try next */
                    }
                    if (l1c & 0xcc)
                        break;  /* skip worn, bodypart, room, hidden */
                    if (tmp16 == 0)
                    {
                        /* return if parent()=player? */
                        zflag = 0xff;
                        break;
                    }
                    write_reg(0, 1, tmp16); /* else look at parent() */
                }
                while (1);
                ptr--;
            }
            while ((!zflag) && ptr);
            write_reg(0, 1, ptr + 1);
            break;

        case 29: /* [2b18] A0FA */

            /* 
               GETNOUNS -  scans noundata for nouns with given attributes

               Entry:  D5.B == 0=> bytewise else wordwise
               D2.B/W      value to match
               D3.W        no. of last noun matched
               D4.W        no. of nouns in table (NOT no. left to do - total!)
               A0.L        pointer to first noun to compare

               Exit:   A0.L        Points to NEXT byte/word to compare
               D3.W        no. of noun that matched
               carry       SET=> match found, else not
            */

            ptr = (type16)read_reg(8, 1);
            do
            {
                if (read_reg(5, 0))
                {
                    l1c = ((type32)((read_w(effective(ptr)) & 0x3fff)) == read_reg(2, 1));
                }
                else
                {
                    l1c = (read_b(effective(ptr)) == read_reg(2, 0));
                }
                if (read_reg(3, 1) == read_reg(4, 1))
                {
                    cflag = 0;
                    write_reg(8, 1, ptr);
                }
                else
                {
                    write_reg(3, 1, read_reg(3, 1) + 1);
                    ptr += 0x0e;
                    if (l1c)
                    {
                        cflag = 0xff;
                        write_reg(8, 1, ptr);
                    }
                }
            }
            while ((!l1c) && (read_reg(3, 1) != read_reg(4, 1)));
            break;

        case 30: /* [2bd1] A0FB */

            /*
              WORDSCAN -  finds the Nth word in the dictionary
              words being hi-bit terminated strings 

              entry   D2.W = no. of words to scan
              A1.L = points at dictionary

              exit    D2.W = 0
              A1.L = points at end of correct word
            */
            
            ptr = (type16)read_reg(8 + 1, 1);
            do
            {
                if (dict)
                    while (dict[ptr++] < 0x80);
                else
                    while (read_b(effective(ptr++)) < 0x80);
                write_reg(2, 1, read_reg(2, 1) - 1);
            }
            while (read_reg(2, 1));
            write_reg(8 + 1, 1, ptr);
            break;

        case 31: /* [2c3b] A0FC */ 

            /* 
               NOUNSCAN -  like WORDSCAN but scans adjective list simultaneously
               adjective list is zero terminated bytes

               Entry:  A0.L    points at word list
               A1.L    points at adjective list
               D0.W    no. of words to skip

               Exit:   A0.L    pointer to end of word
               A1.L    pointer to end of its adjective list
               D0.W    now zero
            */

            ptr = (type16)read_reg(8 + 0, 1);
            ptr2 = (type16)read_reg(8 + 1, 1);
            do
            {
                if (dict)
                    while (dict[ptr++] < 0x80);
                else
                    while (read_b(effective(ptr++)) < 0x80);
                while (read_b(effective(ptr2++)));
                write_reg(0, 1, read_reg(0, 1) - 1);
            }
            while (read_reg(0, 1));
            write_reg(8 + 0, 1, ptr);
            write_reg(8 + 1, 1, ptr2);
            break;

        case 32:  // SETNOUNS
            /* Set properties pointer from A0 [2b7b] A0FD */
#ifdef LOGDIS
            push_msg(DASM_COM,"Set properties pointer from A0");
#endif

            // A0 = object properties (aka noundata)
            properties = (type16)read_reg(8 + 0, 1);  // A0
            if (version > 0)
            {
                // A3 = MessageCode PC address
                fl_sub = (type16)read_reg(8 + 3, 1); // A3
            }
            if (version > 1)
            {
                // A5 = SpaceTable
                fl_tab = (type16)read_reg(8 + 5, 1);

                // D7 = #MinSpaceEntry
                fl_size = (type16)read_reg(7, 1) + 1;
                /* A3 [routine], A5 [table] and D7 [table-size] */
            }
            if (version > 2)
            {
                fp_tab = (type16)read_reg(8 + 6, 1); // A6
                fp_size = (type16)read_reg(6, 1); // D1
            }
            //printf("Sub: %x",fl_sub);
            //printf("Tab: %x",fl_tab);
            //printf("Size: %x",fl_size);

            if (prog_format)
            {

                MsgMSGBASE = read_reg(6, 1); // D6
                MsgSCNBASE = read_reg(5, 1); // D5

                //printf("MsgMSGBASE=%d, MsgSCNBASE=%d\n", MsgMSGBASE, MsgSCNBASE);
                
                // NB: these might also be valid in non-prog format
                // A1 = save game start
                // D0 = save game len

                int savearea = read_reg(8 + 1, 2); // A1 = SLADDR
                int savesize = read_reg(0, 1); // D0

                //printf("save area = %04X\n", savearea);
                //printf("save size = %04X\n", savesize);

                update_game_save_area(effective(savearea), savesize, savearea);
            }
            break;

        case 33: /* GETDATA A0FE */
#ifdef LOGDIS
            push_msg(DASM_COM,"findprop");
#endif
            do_findprop();
            break;

        case 34: /* Dictionary_lookup A0FF */
            // SEARCH
#ifdef LOGDIS
            push_msg(DASM_COM,"Dictionary_lookup");
#endif
            dict_lookup();
            break;
        }
    }
}

#ifdef LOGDIS
   void dasm_fillgaps(int* utarr,int utarrnum, const char* filename)
   {
       no_branch=1;
       nextjmp=1;
       unaddr=utarr;
       num_unaddr=utarrnum;
       dasmfile = filename;
       jmpidx=0;
       //printf("Gaps to fill: %d\n",num_unaddr);
   }
#endif

/* emulate an instruction [1b7e] */

type8 ms_rungame(void)
{
    type8 l1c;
    type16 ptr;
    type32 tmp32;
#ifdef LOGEMU
    static int stat = 0;
#endif


    if (!running)
        return running;
    
    if (undo_pc && pc == undo_pc)
    {
        static int lastCharCount;

        // only undo if we've processed input since last time.
        if (lastCharCount != getCharCount) save_undo();
        
        lastCharCount = getCharCount;
    }

#ifdef LOGEMU
    if (pc == 0x0000)
        stat = 0;
    if (stat)
    {
        log_status();
        push_msg_flush();
    }
    push_msg_begin();
#endif

#ifdef DISASM
    if (!disassem)
        disassem = create_dasm();
    free(curr_ins);
    curr_ins = create_ins();
    swapflag=0;
    op_words=0;

    // Tracking ins only, to be removed later!
    //pc=0x6eb4;
    //if (pc==0x6eb4) printf("");
    //ms_seed(12345);

    if (no_branch)
    {
        //stop current sequence at bra, bsr, jmp, jsr, rts
        if (nextjmp && !contdasm)
        {
            if (jmpidx < num_unaddr )
            {
                nextjmp=0;
                pc =unaddr[jmpidx];
                jmpidx++;
            }
            else
            {
                dasm_jmp_reset(disassem,dasmfile);
                if (num_unaddr == 0)
                {
                    contdasm=1;
                    pc=(type32)dasm_nextgap(disassem);
                    //dasm_dump(disassem, dasmfile);
                    //dasm_listgaps(disassem);
                    //ms_stop();
                }
            }
        }
        if (contdasm)
        {
            if (pc==-1 || pc>=(type32)dasm_gapend(disassem))
            {
                dasm_dump(disassem, dasmfile);
                dasm_listgaps(disassem);
                ms_stop();
            }

        }
    }
    push_msg(DASM_ADDR,"%u", pc);
    ins_addr=pc;
#endif

    i_count++;
    read_word();
#ifdef LOGDIS
    // skip 0x0000 words
    if (no_branch)
        while (byte1==0x00 && byte2 == 0x00)
        {
            push_msg(DASM_OP,"dc.w");
            push_msg(DASM_ARG1,"#$0000");
            push_msg(DASM_COM,"Stop/Fill");
            push_msg(DASM_SIZE,"%d",1);
            push_msg(DASM_SCODE,"0000               ");
            if (!disassem)
                disassem = create_dasm();
            dasm_addins(disassem,curr_ins);
            free(curr_ins);
            curr_ins = create_ins();
            push_msg(DASM_ADDR,"%u", pc);
            ins_addr=pc;
            swapflag=0;
            op_words=0;
            read_word();
        }

#endif
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
#if defined(LOGEMU) || defined (DISASM)
                push_msg(DASM_OP,"ori");
                push_msg(DASM_ARG1,"#$%.2x",byte2);
                push_msg(DASM_ARG2,"CCR");
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
                /* OR [27df] */
#ifdef LOGDIS
                push_msg(DASM_OP,"or");
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
#if defined(LOGEMU) || defined (DISASM)
                push_msg(DASM_OP,"and");
                push_msg(DASM_ARG1,"#$%.2x",byte2);
                push_msg(DASM_ARG2,"CCR");
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
#ifdef LOGDIS
                push_msg(DASM_OP,"and");
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
#ifdef LOGDIS
            push_msg(DASM_OP,"subi");
#endif
            get_arg();
            do_sub(0);
        }
        else
            check_btst();
        break;

    case 0x03:
        if (byte1 == 0x06)
        {
            /* ADD */
#ifdef LOGDIS
            push_msg(DASM_OP,"addi");
#endif
            get_arg();
            do_add(0);
        }
        else
            check_btst();
        break;

    case 0x04:
        if (byte1 == 0x08)
        {
            /* bit operations (immediate) */
            set_info((type8)(byte2 & 0x3f));
            l1c = (effective(pc))[1];
#ifdef LOGDIS
            push_msg(DASM_ARG1,"#$%.2X",(effective(pc))[1]);
            op_words++;
#endif
            pc += 2;
#ifdef LOGDIS
            swapflag=1;
#endif
            set_arg1();
#ifdef LOGDIS
            swapflag=0;
#endif
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
#if defined(LOGEMU) || defined (DISASM)
                push_msg(DASM_OP,"eor");
                push_msg(DASM_ARG1,"#$%.2x",byte2);
                push_msg(DASM_ARG2,"CCR");
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
#ifdef LOGDIS
                push_msg(DASM_OP,"eor");
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
#ifdef LOGDIS
            push_msg(DASM_OP,"cmpi"); //CMPI
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

        /* 10-1F [3327] MOVE.B */
    case 0x08: case 0x09: case 0x0a: case 0x0b:
    case 0x0c: case 0x0d: case 0x0e: case 0x0f:

        set_info((type8)(byte2 & 0x3f));
        set_arg1();
        swap_args();
        l1c = (byte1 >> 1 & 0x07) | (byte2 >> 3 & 0x18) | (byte1 << 5 & 0x20);
        set_info(l1c);
#ifdef LOGDIS
        if (admode == 1)
            push_msg(DASM_OP,"movea");
        else
            push_msg(DASM_OP,"move");
        push_msg(DASM_OPSIZE,"b");
#endif
        set_arg1();
        do_move();
        break;

        /* 20-2F [32d1] MOVE.L */
    case 0x10: case 0x11: case 0x12: case 0x13:
    case 0x14: case 0x15: case 0x16: case 0x17:

        set_info((type8)((byte2 & 0x3f) | 0x80));
        set_arg1();
        swap_args();
        l1c = (byte1 >> 1 & 0x07) | (byte2 >> 3 & 0x18) | (byte1 << 5 & 0x20);
        set_info((type8)(l1c | 0x80));
#ifdef LOGDIS
        if (admode == 1)
            push_msg(DASM_OP,"movea");
        else
            push_msg(DASM_OP,"move");
        push_msg(DASM_OPSIZE,"l");
#endif
        set_arg1();
        do_move();
        break;

        /* 30-3F [3327] MOVE.W */
    case 0x18: case 0x19: case 0x1a: case 0x1b:
    case 0x1c: case 0x1d: case 0x1e: case 0x1f:

        set_info((type8)((byte2 & 0x3f) | 0x40));
        set_arg1();
        swap_args();
        l1c = (byte1 >> 1 & 0x07) | (byte2 >> 3 & 0x18) | (byte1 << 5 & 0x20);
        set_info((type8)(l1c | 0x40));
#ifdef LOGDIS
        if (admode == 1)
            push_msg(DASM_OP,"movea");
        else
            push_msg(DASM_OP,"move");
        push_msg(DASM_OPSIZE,"w");
#endif
        set_arg1();
#ifdef LOGDIS
        if (admode == 1)
            push_msg(DASM_OP,"movea");
        else
            push_msg(DASM_OP,"move");
        push_msg(DASM_OPSIZE,"w");
#endif
        do_move();
        break;

        /* 40-4F various commands */

    case 0x20:
        if (byte1 == 0x40)
        {
            /* [31d5] */
            ms_fatal("unimplemented instructions NEGX and MOVE SR,xx");
        }
        else
            check_lea();
        break;

    case 0x21:
        if (byte1 == 0x42)
        {
            /* [3188] */
            if ((byte2 & 0xc0) == 0xc0)
            {
                ms_fatal("unimplemented instruction MOVE CCR,xx");
            }
            else
            {
                /* CLR */
#ifdef LOGDIS
                push_msg(DASM_OP,"clr");
#endif
                set_info(byte2);
                set_arg1();
#ifdef LOGDIS
                if (opsize == 0)
                    push_msg(DASM_OPSIZE,"b");
                if (opsize == 1)
                    push_msg(DASM_OPSIZE,"w");
                if (opsize == 2)
                    push_msg(DASM_OPSIZE,"l");
#endif
                if (opsize == 0)
                    write_b(arg1, 0);
                if (opsize == 1)
                    write_w(arg1, 0);
                if (opsize == 2)
                    write_l(arg1, 0);
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
            /* [31a0] */
            if ((byte2 & 0xc0) == 0xc0)
            {
                /* MOVE to CCR */
#ifdef LOGDIS
                push_msg(DASM_OP,"move");
                push_msg(DASM_ARG2,"CCR");
#endif
                zflag = nflag = cflag = vflag = 0;
                set_info((type8)(byte2 & 0x7f));
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
#ifdef LOGDIS
                push_msg(DASM_OP,"neg");
#endif
                set_info(byte2);        /* NEG */
                set_arg1();
                cflag = 0xff;
                if (opsize == 0)
                {
                    write_b(arg1, -read_b(arg1));
                    cflag = arg1[0] ? 0xff : 0;
                }
                if (opsize == 1)
                {
                    write_w(arg1, (type16)(-1 * read_w(arg1)));
                    cflag = read_w(arg1) ? 0xff : 0;
                }
                if (opsize == 2)
                {
                    write_l(arg1, -1 * read_l(arg1));
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
#ifdef LOGDIS
                push_msg(DASM_OP,"not");
#endif
                set_info(byte2);        /* NOT */
                set_arg1();
                tmparg[0] = tmparg[1] = tmparg[2] = tmparg[3] = 0xff;
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
#ifdef LOGDIS
                push_msg(DASM_OP,"swap");
#endif
                opsize = 2;     /* SWAP */
                admode = 0;
                regnr = byte2 & 0x07;
                set_arg1();
                tmp32 = read_w(arg1);
                write_w(arg1, read_w(arg1 + 2));
                write_w(arg1 + 2, (type16)tmp32);
                set_flags();
            }
            else if ((byte2 & 0xf8) == 0x80)
            {
#ifdef LOGDIS
                push_msg(DASM_OP,"ext");
                push_msg(DASM_OPSIZE,"w");
#endif
                opsize = 1;     /* EXT.W */
                admode = 0;
                regnr = byte2 & 0x07;
                set_arg1();
                if (arg1[1] > 0x7f)
                    write_b(arg1, 0xff);
                else
                    write_b(arg1, 0);
                set_flags();
            }
            else if ((byte2 & 0xf8) == 0xc0)
            {
#ifdef LOGDIS
                push_msg(DASM_OP,"ext");
                push_msg(DASM_OPSIZE,"l");
#endif
                opsize = 2;     /* EXT.L */
                admode = 0;
                regnr = byte2 & 0x07;
                set_arg1();
                if (read_w(arg1 + 2) > 0x7fff)
                    write_w(arg1, 0xffff);
                else
                    write_w(arg1, 0);
                set_flags();
            }
            else if ((byte2 & 0xc0) == 0x40)
            {
#ifdef LOGDIS
                push_msg(DASM_OP,"pea");
#endif
                set_info((type8)((byte2 & 0x3f) | 0x80));       /* PEA */
                set_arg1();
                if (is_reversible)
                    push(arg1i);
                else
                    ms_fatal("illegal addressing mode for PEA");
            }
            else
            {
                check_movem();  /* MOVEM */
            }
        }
        else
            check_lea();
        break;

    case 0x25:
        if (byte1 == 0x4a)
        {
            /* [3219] TST */
            if ((byte2 & 0xc0) == 0xc0)
            {
                ms_fatal("unimplemented instruction TAS");
            }
            else
            {
#ifdef LOGDIS
                push_msg(DASM_OP,"tst");
#endif
                set_info(byte2);

#ifdef LOGDIS
                if (opsize == 0)
                    push_msg(DASM_OPSIZE,"b");
                if (opsize == 1)
                    push_msg(DASM_OPSIZE,"w");
                if (opsize == 2)
                    push_msg(DASM_OPSIZE,"l");
#endif

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
            check_movem2();         /* [3350] MOVEM.L (Ax)+,A/Dx */
        else
            check_lea();    /* LEA */
        break;

    case 0x27:
        if (byte1 == 0x4e)
        {
            /* [3290] */
            if (byte2 == 0x75)
            {
                /* RTS */
#ifdef LOGDIS
                push_msg(DASM_OP,"rts");
                if (no_branch)
                    nextjmp=1;
                else
#endif
                pc = pop();
            }
            else if (byte2 == 0x71)
            {
                /* NOP */
#ifdef LOGDIS
                push_msg(DASM_OP,"nop");
#endif
            }
            else if ((byte2 & 0xc0) == 0xc0)
            {
                /* indir JMP */
#ifdef LOGDIS
                push_msg(DASM_OP,"jmp");
                if (no_branch)
                    nextjmp=1;
#endif
                set_info((type8)(byte2 | 0xc0));
                set_arg1();
                if (is_reversible)
#ifdef LOGDIS
                if (!no_branch)
#endif
                    pc = arg1i;
                else
                    ms_fatal("illegal addressing mode for JMP");
            }
            else if ((byte2 & 0xc0) == 0x80)
            {
#ifdef LOGDIS
                push_msg(DASM_OP,"jsr");
                if (no_branch)
                    nextjmp=1;
#endif
                set_info((type8)(byte2 | 0xc0));                /* indir JSR */
                set_arg1();
                push(pc);
                if (is_reversible)
#ifdef LOGDIS
                if (!no_branch)
#endif
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
            check_lea();    /* LEA */
        break;

        /* 50-5F [2ed5] ADDQ/SUBQ/Scc/DBcc */
    case 0x28: case 0x29: case 0x2a: case 0x2b:
    case 0x2c: case 0x2d: case 0x2e: case 0x2f:

        if ((byte2 & 0xc0) == 0xc0)
        {
            set_info((type8)(byte2 & 0x3f)); // does not work for dbcc?
            set_arg1();
#ifdef LOGDIS
            char obuf[5]="";
#endif
            if (admode == 1)
            {
                /* DBcc */
#ifdef LOGDIS
                strcpy(obuf,"db");
                // fix dissassembly, for set_info/set_arg1
                arglooping=1;
                push_msg(DASM_ARG1,"d%.1d", regnr);
                arglooping=0;
#endif
                if (condition(byte1) == 0)
                {
                    arg1 = (arg1 - (type8 *) areg) + (type8 *) dreg - 1;
                    write_w(arg1, (type16)(read_w(arg1) - 1));
                    if (read_w(arg1) != 0xffff)
                    {
#ifdef LOGDIS
                        swapflag=1;
#endif
                        branch(0);
#ifdef LOGDIS
                        swapflag=0;
#endif
                    }
                    else
                    {

#ifdef LOGDIS
                        push_msg(DASM_ARG2,"$%.4X", pc + (type16s) read_w(effective(pc)));
                        op_words++;
#endif
                        pc += 2;
                    }
                }
                else
                {
#ifdef LOGDIS
                    push_msg(DASM_ARG2,"$%.4X",  pc + (type16s) read_w(effective(pc)));
                    op_words++;
#endif
                    pc += 2;
                }
            }
            else
            {
                /* Scc */
#ifdef LOGDIS
                strcpy(obuf,"s");
#endif
                write_b(arg1, condition(byte1) ? 0xff : 0);
            }
#ifdef LOGDIS
            switch (byte1 & 0x0f)
            {
            case 0:  strcat(obuf,"t");break;
            case 1:  strcat(obuf,"f");break;
            case 2:  strcat(obuf,"hi");break;
            case 3:  strcat(obuf,"ls");break;
            case 4:  strcat(obuf,"cc");break;
            case 5:  strcat(obuf,"cs");break;
            case 6:  strcat(obuf,"ne");break;
            case 7:  strcat(obuf,"eq");break;
            case 8:  strcat(obuf,"vc");break;
            case 9:  strcat(obuf,"vs");break;
            case 10: strcat(obuf,"pl");break;
            case 11: strcat(obuf,"mi");break;
            case 12: strcat(obuf,"ge");break;
            case 13: strcat(obuf,"lt");break;
            case 14: strcat(obuf,"gt");break;
            case 15: strcat(obuf,"le");break;
            }
            push_msg(DASM_OP,obuf);

#endif
        }
        else
        {
            set_info(byte2);
#ifdef LOGDIS
            swapflag=1;
#endif
            set_arg1();
#ifdef LOGDIS
            swapflag=0;
#endif
            quick_flag = (admode == 1) ? 0xff : 0;
            l1c = byte1 >> 1 & 0x07;
            tmparg[0] = tmparg[1] = tmparg[2] = 0;
            tmparg[3] = l1c ? l1c : 8;
            arg2 = reg_align(tmparg, opsize);
            {
#ifdef LOGDIS
                type32s outnum = 0;
                switch (opsize)
                {
                case 0:
                    outnum = (type8s) read_b(arg2);
                    push_msg(DASM_OPSIZE,"b");
                    push_msg(DASM_ARG1,"#$%.2X", outnum);
                    break;
                case 1:
                    outnum = (type16s) read_w(arg2);
                    push_msg(DASM_OPSIZE,"w");
                    push_msg(DASM_ARG1,"#$%.4X", outnum);
                    break;
                case 2:
                    outnum = (type32s) read_l(arg2);
                    push_msg(DASM_OPSIZE,"l");
                    push_msg(DASM_ARG1,"#$%.8X", outnum);
                    break;
                }
#endif
                if ((byte1 & 0x01) == 1)
                {
#ifdef LOGDIS
                    push_msg(DASM_OP,"subq");
#endif
                    do_sub(0);      /* SUBQ */
                }
                else
                {
#ifdef LOGDIS
                    push_msg(DASM_OP,"addq");
#endif
                    do_add(0);      /* ADDQ */
                }
            }
        }
        break;

        /* 60-6F [26ba] Bcc */

    case 0x30:
        if (byte1 == 0x61)
        {
            /* BRA, BSR */
#ifdef LOGDIS
            push_msg(DASM_OP,"bsr");
#endif
            if (byte2 == 0)
                push(pc + 2);
            else
                push(pc);
        }
#ifdef LOGDIS
        else
        {
            push_msg(DASM_OP,"bra");
        }
        if (no_branch)
            nextjmp=1;
#endif
        if ((byte1 == 0x60) && (byte2 == 0xfe))
        {
            ms_flush();     /* flush stdout */
#ifdef LOGDIS
            if (!no_branch)
#endif
            ms_stop();      /* infinite loop - just exit */
        }
        branch(byte2);
        break;

    case 0x31: case 0x32: case 0x33: case 0x34:
    case 0x35: case 0x36: case 0x37:

#ifdef LOGDIS
        switch (byte1 & 0x0f)
        {
        case 0:  push_msg(DASM_OP,"BXXXX");break;
        case 1:  push_msg(DASM_OP,"BXXXX");break;
        case 2:  push_msg(DASM_OP,"bhi");break;
        case 3:  push_msg(DASM_OP,"bls");break;
        case 4:  push_msg(DASM_OP,"bcc");break;
        case 5:  push_msg(DASM_OP,"bcs");break;
        case 6:  push_msg(DASM_OP,"bne");break;
        case 7:  push_msg(DASM_OP,"beq");break;
        case 8:  push_msg(DASM_OP,"bvc");break;
        case 9:  push_msg(DASM_OP,"bvs");break;
        case 10: push_msg(DASM_OP,"bpl");break;
        case 11: push_msg(DASM_OP,"bmi");break;
        case 12: push_msg(DASM_OP,"bge");break;
        case 13: push_msg(DASM_OP,"blt");break;
        case 14: push_msg(DASM_OP,"bgt");break;
        case 15: push_msg(DASM_OP,"ble");break;
        }
        if (byte2 == 0)
        {
            arglooping=1;
            push_msg(DASM_ARG1,"$%.4X",pc+(type16s)read_w(effective(pc)));
        }
#endif
        if (condition(byte1) == 0)
        {
            if (byte2 == 0)
            {
                pc += 2;
#ifdef LOGDIS
            op_words++;
#endif
            }
            else
            {
#ifdef LOGDIS
            push_msg(DASM_ARG1,"$%.4X",pc+(type8s)(byte2));
#endif
            }
        }
        else
        {
            branch(byte2);
        }
#ifdef LOGDIS
        arglooping=0;
#endif
        break;

        /* 70-7F [260a] MOVEQ */
    case 0x38: case 0x39: case 0x3a: case 0x3b:
    case 0x3c: case 0x3d: case 0x3e: case 0x3f:

        arg1 = (type8 *) & dreg[byte1 >> 1 & 0x07];
        if (byte2 > 127)
        {
            write_b(arg1, 0xff);
            write_b(arg1 + 1, 0xff);
            write_b(arg1 + 2, 0xff);
            nflag = 0xff;
        }
        else
        {
            write_b(arg1, 0);
            write_b(arg1 + 1, 0);
            write_b(arg1 + 2, 0);
            nflag = 0;
        }
        write_b(arg1 + 3, byte2);
        zflag = byte2 ? 0 : 0xff;
#ifdef LOGDIS
        push_msg(DASM_OP,"moveq");
        push_msg(DASM_ARG1,"#$%.2x",byte2);
        push_msg(DASM_ARG2,"d%.1x",byte1 >> 1 & 0x07);
#endif
        break;

        /* 80-8F [2f36] */
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
#ifdef LOGDIS
            push_msg(DASM_OP,"or???");
#endif
            set_info(byte2);
            set_arg1();
            set_arg2(1, byte1);
            if ((byte1 & 0x01) == 0)
                swap_args();
            do_or();
        }
        break;

        /* 90-9F [3005] SUB */
    case 0x48: case 0x49: case 0x4a: case 0x4b:
    case 0x4c: case 0x4d: case 0x4e: case 0x4f:

        quick_flag = 0;
        if ((byte2 & 0xc0) == 0xc0)
        {
#ifdef LOGDIS
            push_msg(DASM_OP,"suba");
#endif
            if ((byte1 & 0x01) == 1)
                set_info((type8)(byte2 & 0xbf));
            else
                set_info((type8)(byte2 & 0x7f));
            set_arg1();
            set_arg2_nosize(0, byte1);
#ifdef LOGDIS
            push_msg(DASM_ARG2,"a%.1X", (byte1 >> 1) & 0x07);
#endif
            swap_args();
            do_sub(1);
        }
        else
        {
#ifdef LOGDIS
            push_msg(DASM_OP,"sub");
#endif
            set_info(byte2);
            set_arg1();
            set_arg2(1, byte1);
#ifdef LOGDIS
            push_msg(DASM_ARG2,"d%.1X", (byte1 >> 1) & 0x07);
#endif
            if ((byte1 & 0x01) == 0)
                swap_args();
            do_sub(0);
        }
        break;

        /* A0-AF various special commands [LINE_A] */

    case 0x50: case 0x56: case 0x57:                /* [2521] */
#ifdef LOGDIS
        if (!no_branch)
#endif
        do_line_a();
#ifdef LOGDIS
        push_msg(DASM_OP,"LINEA");
        push_msg(DASM_ARG1,"A0%.2X", byte2);
#endif
        break;

    case 0x51:
#ifdef LOGDIS
        push_msg(DASM_OP,"rts");
        if (no_branch)
            nextjmp=1;
#endif
#ifdef LOGDIS
        if (!no_branch)
#endif
        pc = pop();     /* RTS */
        break;

    case 0x52:
#ifdef LOGDIS
        push_msg(DASM_OP,"bsr");
        if (no_branch)
            nextjmp=1;
#endif
        if (byte2 == 0)
            push(pc + 2);   /* BSR */
        else
            push(pc);
        branch(byte2);
        break;

    case 0x53:
        if ((byte2 & 0xc0) == 0xc0)
        {
            /* TST [321d] */
            ms_fatal("unimplemented instructions LINE_A #$6C0-#$6FF");
        }
        else
        {
#ifdef LOGDIS
            push_msg(DASM_OP,"tst");
#endif
            set_info(byte2);
#ifdef LOGDIS
            if (opsize == 0)
                push_msg(DASM_OPSIZE,"b");
            if (opsize == 1)
                push_msg(DASM_OPSIZE,"w");
            if (opsize == 2)
                push_msg(DASM_OPSIZE,"l");
#endif
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

        /* B0-BF [2fe4] */
    case 0x58: case 0x59: case 0x5a: case 0x5b:
    case 0x5c: case 0x5d: case 0x5e: case 0x5f:

        if ((byte2 & 0xc0) == 0xc0)
        {
            if ((byte1 & 0x01) == 1)
                set_info((type8)(byte2 & 0xbf));
            else
                set_info((type8)(byte2 & 0x7f));
            set_arg1();
#ifdef LOGDIS
            if (admode==1)
                push_msg(DASM_OP,"cmpa");
            else
                push_msg(DASM_OP,"cmp");
#endif

            set_arg2(0, byte1);
#ifdef LOGDIS
            if (admode==1)
                push_msg(DASM_ARG2,"a%.1d",(byte1>>1) & 7);
            else
                push_msg(DASM_ARG2,"d%.1d",(byte1>>1) & 7);
#endif
            swap_args();
            do_cmp();       /* CMP */
        }
        else
        {
            if ((byte1 & 0x01) == 0)
            {
                set_info(byte2);
                set_arg1();
#ifdef LOGDIS
                if (admode==1)
                    push_msg(DASM_OP,"cmpa");
                else
                    push_msg(DASM_OP,"cmp");
#endif
                set_arg2(1, byte1);
#ifdef LOGDIS
                if (admode==1)
                    push_msg(DASM_ARG2,"a%.1d",(byte1>>1) & 7);
                else
                    push_msg(DASM_ARG2,"d%.1d",(byte1>>1) & 7);

#endif
                swap_args();
                do_cmp();       /* CMP */
            }
            else
            {
#ifdef LOGDIS
                push_msg(DASM_OP,"eor");
#endif
                set_info(byte2);
#ifdef LOGDIS
                swapflag=1;
#endif
                set_arg1();
#ifdef LOGDIS
                swapflag=0;
#endif
                set_arg2(1, byte1);
#ifdef LOGDIS
                push_msg(DASM_ARG1,"a%.1d",(byte1>>1) & 7);
#endif
                do_eor();       /* EOR */
            }
        }
        break;

        /* C0-CF [2f52] EXG, AND */
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
#ifdef LOGDIS
                push_msg(DASM_OP,"and");
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
#ifdef LOGDIS
                push_msg(DASM_OP,"exg");
#endif
                opsize = 2;     /* EXG Dx,Dx */
                set_arg2(1, (type8)(byte2 << 1));
#ifdef LOGDIS
                push_msg(DASM_ARG2,"d%.1X",byte2 & (0x07));
#endif
                swap_args();
                set_arg2(1, byte1);
#ifdef LOGDIS
                swapflag=0;
                push_msg(DASM_ARG1,"d%.1X",(type8)(((byte1 >> 1) & 0x07)));
#endif
                tmp32 = read_l(arg1);
                write_l(arg1, read_l(arg2));
                write_l(arg2, tmp32);
            }
            else if ((byte2 & 0xf8) == 0x48)
            {
                opsize = 2;     /* EXG Ax,Ax */
#ifdef LOGDIS
                push_msg(DASM_OP,"exg");
#endif
                set_arg2(0, (type8)(byte2 << 1));
#ifdef LOGDIS
                push_msg(DASM_ARG2,"a%.1X",byte2 & (0x07));
#endif
                swap_args();
                set_arg2(0, byte1);
#ifdef LOGDIS
                swapflag=0;
                push_msg(DASM_ARG1,"a%.1X",(type8)(((byte1 >> 1) & 0x07)));
#endif
                tmp32 = read_l(arg1);
                write_l(arg1, read_l(arg2));
                write_l(arg2, tmp32);
            }
            else if ((byte2 & 0xf8) == 0x88)
            {
                opsize = 2;     /* EXG Dx,Ax */
#ifdef LOGDIS
                push_msg(DASM_OP,"exg");
#endif
                set_arg2(0, (type8)(byte2 << 1));
#ifdef LOGDIS
                push_msg(DASM_ARG2,"d%.1X",byte2 & (0x07));
#endif
                swap_args();
                set_arg2(1, byte1);
#ifdef LOGDIS
                swapflag=0;
                push_msg(DASM_ARG1,"a%.1X",(type8)(((byte1 >> 1) & 0x07)));
#endif
                tmp32 = read_l(arg1);
                write_l(arg1, read_l(arg2));
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

        /* D0-DF [2fc8] ADD */
    case 0x68: case 0x69: case 0x6a: case 0x6b:
    case 0x6c: case 0x6d: case 0x6e: case 0x6f:

        quick_flag = 0;
        if ((byte2 & 0xc0) == 0xc0)
        {
#ifdef LOGDIS
            push_msg(DASM_OP,"adda");
#endif
            if ((byte1 & 0x01) == 1)
                set_info((type8)(byte2 & 0xbf));
            else
                set_info((type8)(byte2 & 0x7f));
            set_arg1();
            set_arg2_nosize(0, byte1);
#ifdef LOGDIS
            push_msg(DASM_ARG2,"a%.1X", (byte1 >> 1) & 0x07);
#endif
            swap_args();
            do_add(1);
        }
        else
        {
#ifdef LOGDIS
            push_msg(DASM_OP,"add");
#endif
            set_info(byte2);
            set_arg1();
            set_arg2(1, byte1);
#ifdef LOGDIS
            push_msg(DASM_ARG2,"d%.1X", (byte1 >> 1) & 0x07);
#endif
            if ((byte1 & 0x01) == 0)
                swap_args();
            do_add(0);
        }
        break;

        /* E0-EF [3479] LSR ASL ROR ROL */
    case 0x70: case 0x71: case 0x72: case 0x73:
    case 0x74: case 0x75: case 0x76: case 0x77:

#ifdef LOGEMU
        out("lsr,asl,ror,rol ");
#endif
        if ((byte2 & 0xc0) == 0xc0)
        {
            set_info((type8)(byte2 & 0xbf));                /* OP Dx */
            set_arg1();
            l1c = 1;        /* steps=1 */
            byte2 = (byte1 >> 1) & 0x03;
        }
        else
        {
            set_info((type8)(byte2 & 0xc7));
#ifdef LOGDIS
            swapflag=1;
#endif
            set_arg1();
#ifdef LOGDIS
            swapflag=0;
#endif
            if ((byte2 & 0x20) == 0)
            {
                /* immediate */
                l1c = (byte1 >> 1) & 0x07;
                if (l1c == 0)
                    l1c = 8;
#ifdef LOGDIS
                push_msg(DASM_ARG1,"#$%.2X",l1c);
#endif
            }
            else
            {
                l1c = (type8)read_reg(byte1 >> 1 & 0x07, 0);
#ifdef LOGDIS
                push_msg(DASM_ARG1,"d%.1X",l1c);
#endif
            }
            byte2 = (byte2 >> 3) & 0x03;
        }
#ifdef LOGDIS
        if (opsize == 0)
            push_msg(DASM_OPSIZE,"b");
        else if (opsize == 1)
            push_msg(DASM_OPSIZE,"w");
        else if (opsize == 2)
            push_msg(DASM_OPSIZE,"l");
#endif
        if ((byte1 & 0x01) == 0)
        {
            /* right */
#ifdef LOGDIS
            if (byte2 == 0)
                push_msg(DASM_OP,"asr");
            else if (byte2 == 1)
                push_msg(DASM_OP,"lsr");
            else if (byte2 == 2)
                push_msg(DASM_OP,"roxr");
            else if (byte2 == 3)
                push_msg(DASM_OP,"ror");
#endif
            while (l1c-- > 0)
            {
                if (opsize == 0)
                {
                    cflag = arg1[0] & 0x01 ? 0xff : 0;
                    write_b(arg1, read_b(arg1) >> 1);
                    if (cflag && (byte2 == 3))
                        arg1[0] |= 0x80;
                }
                if (opsize == 1)
                {
                    cflag = read_w(arg1) & 0x01 ? 0xff : 0;
                    write_w(arg1, (type16)(read_w(arg1) >> 1));
                    if (cflag && (byte2 == 3))
                        write_w(arg1, (type16)(read_w(arg1) | ((type16) 1 << 15)));
                }
                if (opsize == 2)
                {
                    cflag = read_l(arg1) & 0x01 ? 0xff : 0;
                    write_l(arg1, read_l(arg1) >> 1);
                    if (cflag && (byte2 == 3))
                        write_l(arg1, read_l(arg1) | ((type32) 1 << 31));
                }
            }
        }
        else
        {
#ifdef LOGDIS
            if (byte2 == 0)
                push_msg(DASM_OP,"asl");
            else if (byte2 == 1)
                push_msg(DASM_OP,"lsl");
            else if (byte2 == 2)
                push_msg(DASM_OP,"roxl");
            else if (byte2 == 3)
                push_msg(DASM_OP,"rol");
#endif
            /* left */
            while (l1c-- > 0)
            {
                if (opsize == 0)
                {
                    cflag = arg1[0] & 0x80 ? 0xff : 0;      /* [3527] */
                    write_b(arg1, read_b(arg1) << 1);
                    if (cflag && (byte2 == 3))
                        arg1[0] |= 0x01;
                }
                if (opsize == 1)
                {
                    cflag = read_w(arg1) & ((type16) 1 << 15) ? 0xff : 0;
                    write_w(arg1, (type16)(read_w(arg1) << 1));
                    if (cflag && (byte2 == 3))
                        write_w(arg1, (type16)(read_w(arg1) | 0x01));
                }
                if (opsize == 2)
                {
                    cflag = read_l(arg1) & ((type32) 1 << 31) ? 0xff : 0;
                    write_l(arg1, read_l(arg1) << 1);
                    if (cflag && (byte2 == 3))
                        write_l(arg1, read_l(arg1) | 0x01);
                }
            }
        }
        set_flags();
        break;

        /* F0-FF [24f3] LINE_F */
    case 0x78: case 0x79: case 0x7a: case 0x7b:
    case 0x7c: case 0x7d: case 0x7e: case 0x7f:

#ifdef LOGDIS
            push_msg(DASM_OP,"LINEF");
#endif
        if (version == 0)
        {
            /* hardcoded jump */
            char_out(l1c = (type8)read_reg(1, 0));
        }
        else if (version == 1)
        {
            /* single programmable shortcut */
            push(pc);
#ifdef LOGDIS
            if (!no_branch)
#endif
            pc = fl_sub; // MessageCode function
        }
        else
        {
            /* programmable shortcuts from table */
#ifdef LOGDIS
            push_msg(DASM_ARG1,"%.2X",byte1);
            push_msg(DASM_ARG2,"%.2X",byte2);
#endif
            ptr = (byte1 & 7) << 8 | byte2;
            if (ptr >= fl_size)
            {
                if ((byte1 & 8) == 0)
                    push(pc);
                ptr = byte1 << 8 | byte2 | 0x0800;
                ptr = fl_tab + 2 * (ptr ^ 0xffff);
                int newpc = (type32) ptr + (type16s) read_w(effective(ptr));
#ifdef LOGDIS
                if (!no_branch)
#endif
                pc = newpc;
            }
            else
            {

                // message number
                int m = ((byte1 << 8) + byte2) & 0x7ff;
                m += MsgMSGBASE;

                if (m > MsgMSGBASE + MsgSCNBASE)
                {
                    ++ScnMsgCount;
                    
                    // scenario message
                    //printf("### SCN Message %d\n", m);
                }
                
                push(pc);
#ifdef LOGDIS
                if (!no_branch)
#endif
                pc = fl_sub;  // MessageCode function
#ifdef LOGDIS
                else
                    pc += 2;
#endif
            }
        }
        break;

    default:
        ms_fatal("Constants aren't and variables don't");
        break;
    }
#ifdef LOGEMU
    push_msg_end();
#endif
#ifdef DISASM
    push_msg(DASM_SIZE,"%d",op_words);
    push_msg(DASM_SCODE,"%.2X%.2X",effective(ins_addr)[0],effective(ins_addr)[1]);
    if (op_words>=2)
        push_msg(DASM_ACODE," %.2X%.2X",effective(ins_addr)[2],effective(ins_addr)[3]);
    else
        push_msg(DASM_ACODE,"     ");
    if (op_words>=3)
        push_msg(DASM_ACODE," %.2X%.2X",effective(ins_addr)[4],effective(ins_addr)[5]);
    else
        push_msg(DASM_ACODE,"     ");
    if (op_words==4)
        push_msg(DASM_ACODE," %.2X%.2X",effective(ins_addr)[6],effective(ins_addr)[7]);
    else
        push_msg(DASM_ACODE,"     ");
    dasm_addins(disassem,curr_ins);
#endif
    return running;
}
