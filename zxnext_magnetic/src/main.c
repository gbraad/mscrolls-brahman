/*******************************************************************************
 *
 * Magnetic Scrolls interpreter interface for Sinclair ZX Spectrum Next.
 *
 * Written by Stefan Bylund <stefan.bylund99@gmail.com>
 *
 * Copyright (C) 2018 Stefan Bylund
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
 * This program is configured in the configure.m4 file, which generates the
 * zconfig.h header file included by this program and the zpragma.inc file
 * containing the pragmas for this program.
 *
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <intrinsic.h>
#include <input.h>
#include <font/fzx.h>
#include <rect.h>
#include <adt.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <errno.h>

#include "zconfig.h"
#include "defs.h"
#include "sprite.h"
#include "layer2.h"
#include "status_bar.h"
#include "text_color.h"
#include "sound.h"
#include "mouse.h"
#include "gfx_util.h"
#include "esxdos_util.h"
#include "stack.h"
#include "ide_friendly.h"

#if USE_IMAGE_SLIDESHOW
#include "image_slideshow.h"
#endif

// FIXME: Remove when included in z88dk.
#define RTM_28MHZ 0x03

#if USE_TIMEX_HIRES
#define BUFFER_MEMORY_START 0x5800
#else
#define BUFFER_MEMORY_START 0x5B00
#endif

#define OUT_BUFFER_SIZE 1024
#define IN_BUFFER_SIZE 256
#define HISTORY_BUFFER_SIZE 128
#define MAX_FILENAME_LENGTH 16

#define FD_STDIN 0
#define FD_STDOUT 1

#define READ_FILE 1
#define WRITE_FILE 0

#define SCROLL_PROMPT_SPRITE_START_SLOT 0
#define REMASTER_SPRITE_START_SLOT 2
#define CLASSIC_SPRITE_START_SLOT 7

#define WAIT_IMAGE 99

#if USE_PROG_FORMAT
extern void ms_config(void);
extern type8 toggle_game_mode(void);
#endif
extern void page_in_rom(void);
extern void page_out_rom(void);
extern type8 current_page;

#if USE_GFX && USE_MOUSE
static void mouse_handler(type8 mouse_x, type8 mouse_y, type8 mouse_buttons, type8s wheel_delta);
#endif

static struct fzx_font *out_term_font;
static type16 out_term_line_width;

/*
 * Note: When using the Timex hi-res mode, we use the free 2 KB RAM between the
 * two Timex hi-res screen buffers (0x5800 - 0x5FFF) for storing the out_buffer,
 * in_buffer and buf_256 buffers of total size 1.5 KB and when using the ULA mode
 * we use the 1.5 KB RAM after the ULA screen buffer (0x5B00 - 0x60FF) for this.
 */
static type8 *out_buffer = (type8 *) BUFFER_MEMORY_START;
static type8 *in_buffer = (type8 *) (BUFFER_MEMORY_START + OUT_BUFFER_SIZE);
static type8 *buf_256 = (type8 *) (BUFFER_MEMORY_START + OUT_BUFFER_SIZE + IN_BUFFER_SIZE);

static type16 out_buffer_pos = 0;
static type16 in_buffer_pos = 0;

static type8 history_buffer[HISTORY_BUFFER_SIZE];
type16 history_buffer_size = 0;
bool edit_pressed = false;

static type8 num_commands = 0;
static type8 command_pos = 0;

static type8 statuschar_pos = 0;
static type8 statuschar_tab;

#if USE_GFX
type8 gfx_on = 0;

// Current graphics window height in pixels.
type8 gfx_window_height = 0;

// Accumulated adjustments of graphics window height by user.
type8s gfx_window_height_adjustment = 0;
#endif

static type8 filename[MAX_FILENAME_LENGTH];

#if USE_SCRIPT
static type8 script_on = 0;
static type8 script_file;
#endif

static void init_hardware(void)
{
    // Make sure the Spectrum ROM is paged in initially.
    IO_7FFD = IO_7FFD_ROM0;

    // Put Z80 in 28 MHz turbo mode.
    ZXN_NEXTREG(REG_TURBO_MODE, RTM_28MHZ);

    // Disable RAM memory contention.
    ZXN_NEXTREGA(REG_PERIPHERAL_3, ZXN_READ_REG(REG_PERIPHERAL_3) | RP3_DISABLE_CONTENTION);

    // Reset display palette and r/w palette selections to their defaults.
    ZXN_NEXTREG(REG_PALETTE_CONTROL, 0);

    // Reset clip windows to their defaults.
    ZXN_NEXTREG(REG_CLIP_WINDOW_CONTROL, 0x07);

    // Initialize all hardware sprites to invisible.
    sprite_select_slot(0);
    for (type8 i = 0; i != 64; ++i)
    {
        sprite_set_attributes(i, 0, 0, false);
    }

    // Set default sprite palette and sprite transparency palette index.
    sprite_set_default_palette();

    // Initialize hardware sprites and layers system:
    // Sprites over layer 2 screen over ULA screen.
    // Sprites visible and displayable on border.
    sprite_config(true, true);

    // Set black border.
    zx_border(INK_BLACK);

#if USE_TIMEX_HIRES
    // Enable Timex hi-res mode for the ULA screen with white ink on black paper.
    ZXN_NEXTREGA(REG_PERIPHERAL_3, ZXN_READ_REG(REG_PERIPHERAL_3) | RP3_ENABLE_TIMEX);
    IO_FF = IO_TVM_HIRES_WHITE;
#endif

    // Set initial text font color.
    set_initial_text_color(TEXT_FONT_COLOR_INDEX);

#if USE_GFX
    // Set layer 2 main and shadow screen banks and clear and enable layer 2 screen.
    layer2_set_main_screen_bank(8);
    layer2_set_shadow_screen_bank(11);
    layer2_clear_screen(MAIN_SCREEN, 0x00);
    layer2_config(true);
#endif

    // Enable interrupts (initialized in interrupt.asm).
    intrinsic_ei();
}

static void wait_key(void)
{
    in_wait_nokey();
    in_wait_key();
    in_wait_nokey();
}

#if USE_GFX
// Show title and credits images and optionally play intro music.
static void show_intro(void)
{
    // Load title and credits images and show title image.
    layer2_load_screen(MAIN_SCREEN, LAYER2_PALETTE_1, "gfx/title.nxi", 192, buf_256);
    layer2_load_screen(SHADOW_SCREEN, LAYER2_PALETTE_2, "gfx/credits.nxi", 192, buf_256);

#if USE_SOUND
    // Play intro music (MMU slots 0 and 1 cannot be used until music is stopped).
    play_music();
#endif

    // Flip to show credits image.
    wait_key();
    layer2_flip_main_shadow_screen();
    layer2_flip_display_palettes();
    wait_key();

#if USE_SOUND
    // Stop intro music.
    stop_music();
#endif

    // Clear layer 2 screen.
    layer2_clear_screen(MAIN_SCREEN, 0x00);
}
#endif

// FIXME: Use fastcall when z88dk bug #689 on github is fixed.
static void init_out_terminal(FILE *out)
{
    int fd;
    struct r_Rect16 paper;

    fd = fileno(out);
    out_term_font = (struct fzx_font *) ioctl(fd, IOCTL_OTERM_FONT, -1);
    ioctl(fd, IOCTL_OTERM_FZX_GET_PAPER_RECT, &paper);
    out_term_line_width = paper.width - ioctl(fd, IOCTL_OTERM_FZX_LEFT_MARGIN, -1) - 1;
}

static void create_screen(void)
{
#if USE_TIMEX_HIRES
    // Clear the ULA screen in Timex hi-res mode.
    memset((void *) 0x4000, 0, 0x1800);
    memset((void *) 0x6000, 0, 0x1800);
#else
    // Clear the ULA screen.
    zx_cls(INK_BLACK | PAPER_BLACK);
#endif

    // When graphics is enabled in ms_showpic(), the clip window of the layer 2 screen
    // is set so that the ULA screen shows through and its text area is visible.

    // Load and set scroll prompt and game mode hardware sprites using the default sprite palette.
    sprite_load_patterns("gfx/prompt.spr", buf_256, 2, SCROLL_PROMPT_SPRITE_START_SLOT);
#if USE_PROG_FORMAT
    sprite_load_patterns("gfx/remaster.spr", buf_256, 5, REMASTER_SPRITE_START_SLOT);
    sprite_load_patterns("gfx/classic.spr", buf_256, 5, CLASSIC_SPRITE_START_SLOT);
#endif

    // Init output terminal.
    init_out_terminal(stdout);

    // Init status bar.
    init_status_bar(out_term_font);

#if USE_GFX && USE_MOUSE
    // Init PS/2 Kempston mouse.
    // Note: From this point on, usage of the sprite slot port must be atomic (di/ei)
    // since the mouse interrupt handler will write to the sprite slot port each frame.
    init_mouse(buf_256, mouse_handler);
#endif
}

#if USE_SCRIPT
static void open_script(void)
{
    // Construct script filename from game filename.
    type8 *suffix;
    strcpy(filename, GAME_FILE);
#if USE_PROG_FORMAT
    suffix = strstr(filename, ".prg");
#else
    suffix = strstr(filename, ".mag");
#endif
    if (suffix == NULL)
    {
        printf("\nInvalid game filename %s\n", filename);
        return;
    }
#if USE_PROG_FORMAT
    strcpy(suffix, "_prg.rec");
#else
    strcpy(suffix, ".rec");
#endif

    // Open script file.
    errno = 0;
    script_file = esx_f_open(filename, ESX_MODE_OPEN_EXIST | ESX_MODE_R);
    if (errno)
    {
        printf("\nError opening script file %s\n", filename);
    }
    else
    {
        script_on = 1;
        // Disable scroll pause during script play.
        ioctl(FD_STDOUT, IOCTL_OTERM_PAUSE, 0);
    }
}

static void read_script_command(void)
{
    type8 *res;

    // Read script command.
    res = esx_gets(script_file, buf_256, 256);
    if (res == NULL)
    {
        return;
    }
    printf("%s\n", buf_256);

    // Handle script command.
    if (!strcmp(buf_256, "seed"))
    {
        type32 seed;

        res = esx_gets(script_file, buf_256, 256);
        if (res == NULL)
        {
            return;
        }
        printf("%s\n", buf_256);

        seed = (type32) atol(buf_256);
        if (seed != 0)
        {
            ms_seed(seed);
            printf("[New seed: %lu]", seed);
        }
        else
        {
            printf("[Invalid seed]");
        }
    }
    else if (!strcmp(buf_256, "stack"))
    {
        printf("[Peak stack usage: %u]", peak_stack_usage());
    }
    else
    {
        printf("[Invalid command]");
    }
}

static int read_script(type8 trans) __z88dk_fastcall
{
    int c;

    // Read input from script file.
    c = esx_getc(script_file);

    if (c != EOF)
    {
        // Print input from script file excluding newline.
        if (c != '\n')
        {
            fputc(c, stdout);
        }

        // Handle script command.
        if ((c == '#') && trans)
        {
            read_script_command();
            // Force new prompt.
            c = '\n';
        }
    }
    else
    {
        // End of script file, turn it off.
        close_script();

        // Enable scroll pause handling again.
        ioctl(FD_STDOUT, IOCTL_OTERM_PAUSE, 1);

        // Begin reading input from terminal instead.
        c = getchar();
    }

    return c;
}

static void close_script(void)
{
    if (script_on)
    {
        page_in_rom();
        esx_f_close(script_file);
        script_on = 0;
    }
}
#endif

static void exit_handler(void)
{
#if USE_SCRIPT
    close_script();
#endif

    ms_freemem();

    // Trig a soft reset. The Next hardware registers and I/O ports will be reset by NextZXOS after a soft reset.
    ZXN_NEXTREG(REG_RESET, RR_SOFT_RESET);
}

/*
 * Word wrap the given string (i.e. replace space characters with newline
 * characters) so that it fits the width of the FZX output terminal on an
 * even word boundary. Any existing newline characters are respected.
 * The word wrapped string is returned.
 */
static type8 *str_word_wrap(type8 *str, type16 str_len)
{
    type8 *line;
    type8 *line_end;

    line = strtok(str, "\n");

    while (line != NULL)
    {
        line_end = fzx_string_partition_ww(out_term_font, line, out_term_line_width);
        if ((line_end - str) >= str_len)
        {
            break;
        }
        if ((*line_end == ' ') || (*line_end == '\0'))
        {
            *line_end = '\n';
        }
        line = strtok(line_end + 1, "\n");
    }

    return str;
}

static void save_history(void)
{
    if (in_buffer_pos > 1)
    {
        // Copy entered input to history buffer except for ending newline.
        memcpy(history_buffer, in_buffer, in_buffer_pos - 1);
        history_buffer_size = in_buffer_pos - 1;
    }
}

static void load_history(void)
{
    b_array_t edit_buffer;

    // Copy history buffer to input terminal's edit buffer.
    fflush(stdin);
    ioctl(FD_STDIN, IOCTL_ITERM_GET_EDITBUF, &edit_buffer);
    memcpy(edit_buffer.data, history_buffer, history_buffer_size);
    edit_buffer.size = history_buffer_size;
    ioctl(FD_STDIN, IOCTL_ITERM_SET_EDITBUF, &edit_buffer);
}

/*
 * Get filename, either the given if non-null or by reading from stdin.
 */
static type8 *get_filename(type8 *name) __z88dk_fastcall
{
    type8 *realname;

    if (name != NULL)
    {
        realname = name;
    }
    else
    {
        // Game version 4.
        do
        {
            fputs("Filename: ", stdout);
            filename[0] = '\0';
            fgets(filename, sizeof(filename), stdin);
            fflush(stdin);
        }
        while (*(realname = strrstrip(strstrip(filename))) == '\0');
    }

    return realname;
}

/*
 * Read or write from/to the given file to/from the specified buffer.
 */
static type8 access_file(type8 *name, type8 *ptr, type16 size, type8 read)
{
    type8 fh;
    type8 page;
    type16 remaining_size;
    type16 addr;
    type8 *buffer;
    type8 ret = 0;

    /*
     * When using ESXDOS, the bottom 16 KB (MMU slot 0 and 1) must contain the
     * Spectrum ROM. The given argument ptr points in the currently swapped in
     * page in MMU slot 0. Since we must temporarily swap in the Spectrum ROM in
     * this area, the page pointed to by ptr is instead temporarily swapped in
     * to MMU slot 2 (the ULA screen, page 10). Depending on the size argument,
     * any following pages are also swapped in to MMU slot 2 in turn.
     */

    page_in_rom();

    errno = 0;
    fh = esx_f_open(name, read ? ESX_MODE_OPEN_EXIST | ESX_MODE_R : ESX_MODE_OPEN_CREAT_TRUNC | ESX_MODE_W);
    if (errno)
    {
        ret = 1;
        goto end;
    }

    page = current_page;
    remaining_size = size;
    addr = (type16) ptr;

    while (remaining_size > 0)
    {
        ZXN_WRITE_MMU2(page++);

        size = 0x2000 - (addr % 0x2000);
        if (size > remaining_size)
        {
            size = remaining_size;
        }
        buffer = (type8 *) (0x4000 + (addr % 0x2000));

        if ((read ? esx_f_read(fh, buffer, size) : esx_f_write(fh, buffer, size)) != size)
        {
            ret = 1;
            goto close;
        }

        addr += size;
        remaining_size -= size;
    }

close:
    esx_f_close(fh);
end:
    ZXN_WRITE_MMU2(10);
    page_out_rom();
    return ret;
}

#if USE_GFX
void show_location_image(type8 image_number) __z88dk_fastcall
{
    // Load and display the given location image. In order to minimize flicker
    // when switching to a new image, the new image and its palette is first
    // loaded into the layer 2 shadow screen and the layer 2 palette (primary or
    // secondary) not currently used. Then the layer 2 main/shadow screen and
    // the primary/secondary palettes are flipped to show the new image.

    sprintf(filename, "gfx/%2.2u.nxi", (type16) image_number);

    errno = 0;
    page_in_rom();

    layer2_load_screen(SHADOW_SCREEN, layer2_get_unused_access_palette(), filename, MAX_IMAGE_HEIGHT, buf_256);
    wait_video_line(192);
    layer2_flip_main_shadow_screen();
    layer2_flip_display_palettes();

    page_out_rom();
    if (errno)
    {
        printf("\nError loading image %s\n", filename);
    }
}

#if USE_MOUSE
static void mouse_handler(type8 mouse_x, type8 mouse_y, type8 mouse_buttons, type8s wheel_delta)
{
    static type8 last_mouse_drag_y = 0;

    if (wheel_delta != 0)
    {
        // MOUSE WHEEL EVENT

        type8s image_height_change;
        type8 image_height_rest;

        // Translate one mouse wheel movement as 8 pixels, i.e. as one line of text.
        image_height_change = wheel_delta << 3;

        // If the image bottom is not aligned on a text line boundary, make it so for this mouse wheel
        // event by scrolling the image up or down the amount of pixels to make it aligned and setting
        // the old gfx_window_height_adjustment so that the new gfx_window_height_adjustment will indicate
        // that the scroll limit of the output terminal should be adjusted by one line (8 pixels).
        image_height_rest = gfx_window_height % 8;
        if (image_height_rest != 0)
        {
            if (image_height_change < 0)
            {
                image_height_change = -image_height_rest;
                gfx_window_height_adjustment = -8 + image_height_rest;
            }
            else
            {
                image_height_change = 8 - image_height_rest;
                gfx_window_height_adjustment = image_height_rest;
            }
        }

        // Scroll image by the given amount.
        image_mouse_scroll(image_height_change);
    }
    else if (((mouse_buttons & MOUSE_BUTTON_LEFT) != 0) && ((last_mouse_drag_y != 0) ||
             (mouse_y < ((gfx_window_height != 0) ? gfx_window_height : 8))))
    {
        // MOUSE DRAG EVENT

        // Left mouse button pressed and mouse pointer inside image or a mouse drag
        // operation is ongoing. Allow some margin below the image when fully minimized.

        if (last_mouse_drag_y != 0)
        {
            type8s image_height_change = (type8s) (mouse_y - last_mouse_drag_y);

            // Make it easier to make the image fully minimized when dragging upwards and hitting the top border.
            if ((image_height_change < 0) && (gfx_window_height < 8))
            {
                image_height_change = -gfx_window_height;
            }

            // Scroll image by the given amount.
            image_mouse_scroll(image_height_change);
        }

        // Update last mouse drag y-coordinate.
        last_mouse_drag_y = mouse_y;
    }
    else
    {
        // Mark any ongoing mouse drag operation as ended.
        last_mouse_drag_y = 0;
    }
}
#endif
#endif

static void add_command(type8 *command)
{
    sprintf(in_buffer + command_pos, "%s\n", command);
    command_pos += strlen(command) + 1;
    num_commands++;
}

static void remove_command(void)
{
    if (num_commands)
    {
        num_commands--;
        if (num_commands == 0)
        {
            command_pos = 0;
        }
    }
}

#if USE_PROG_FORMAT

static void set_game_mode_sprites(type8 start_sprite_slot, bool visible)
{
    intrinsic_di();
    sprite_select_slot(start_sprite_slot);
    for (type8 i = 0; i != 5; ++i)
    {
        sprite_set_attributes(start_sprite_slot + i, 120 + 16 * i, 226, visible);
    }
    intrinsic_ei();
}

void switch_game_mode(void)
{
    type8 mode = toggle_game_mode();
    type8 start_sprite_slot = mode ? REMASTER_SPRITE_START_SLOT : CLASSIC_SPRITE_START_SLOT;
    set_game_mode_sprites(start_sprite_slot, true);
    z80_delay_ms(8000);
    set_game_mode_sprites(start_sprite_slot, false);
}

#endif

void ms_statuschar(type8 c) __z88dk_fastcall
{
    // A sequence of calls to print a status line with the current room name
    // (variable length) to the left and the score/turn (always 9 characters,
    // prepended with spaces if necessary) to the right.

    if (c == '\t')
    {
        // End room name string and mark the tab separating room name and score/turn.
        buf_256[statuschar_pos++] = '\0';
        statuschar_tab = statuschar_pos;
    }
    else if (c == '\n')
    {
        // End score/turn string, we now have the full status line.
        buf_256[statuschar_pos] = '\0';

        // Print status line, i.e. room name and score/turn.
        print_status(buf_256, buf_256 + statuschar_tab);

        // Status line handling is now complete.
        statuschar_pos = 0;
    }
    else
    {
        // Buffer the output.
        buf_256[statuschar_pos++] = c;
    }
}

void ms_flush(void)
{
    if (out_buffer_pos == 0)
    {
        return;
    }

    out_buffer[out_buffer_pos] = '\0';
    str_word_wrap(out_buffer, out_buffer_pos);
    fputs(out_buffer, stdout);
    out_buffer_pos = 0;
}

void ms_putchar(type8 c) __z88dk_fastcall
{
    // The interpreter can emit backspace characters for making minor corrections.
    if (c == '\b')
    {
        if (out_buffer_pos > 0)
        {
            out_buffer_pos--;
        }

        return;
    }

    // Buffer the output.
    out_buffer[out_buffer_pos++] = c;

    // Add a space after the prompt.
    if ((c == '>') && (out_buffer_pos == 1))
    {
        out_buffer[out_buffer_pos++] = ' ';
    }

    // Flush the output if prompt, newline or output buffer full.
    if ((c == '>') || (c == '\n') || (out_buffer_pos == OUT_BUFFER_SIZE - 1))
    {
        ms_flush();
    }
}

type8 ms_getchar(type8 trans) __z88dk_fastcall
{
    int c;

    // If start of input, read and buffer a line of input.
    if ((in_buffer_pos == 0) && (num_commands == 0))
    {
        type8 i = 0;

        while (1)
        {
#if USE_SCRIPT
            if (script_on)
            {
                // Read input from script file.
                page_in_rom();
                c = read_script(trans && (i == 0));
                page_out_rom();
            }
            else
            {
                // Read input from terminal.
                c = getchar();
            }
#else
            c = getchar();
#endif

            if ((c == EOF) && edit_pressed)
            {
                clearerr(stdin);
                ioctl(FD_STDIN, IOCTL_RESET);
                edit_pressed = false;
                load_history();
                i = 0;
                continue;
            }
            if ((c == '\n') || (c == EOF) || (i == 255))
            {
                break;
            }
            in_buffer[i++] = c;
            if (c == '\0')
            {
                break;
            }
        }

        in_buffer[i] = '\n';
    }

    // Return one character at a time of the buffered input.
    c = in_buffer[in_buffer_pos++];

    // End of input if newline or string termination.
    if ((c == '\n') || (c == '\0'))
    {
        if (num_commands)
        {
            remove_command();
        }
        if (num_commands == 0)
        {
            save_history();
            in_buffer_pos = 0;
        }
    }
    else if (num_commands)
    {
        // Echo the command.
        fputc(c, stdout);
    }

    return (type8) c;
}

void ms_showpic(type8 c, type8 mode)
{
#if USE_GFX
    if (mode)
    {
        // Load and display the image.
        show_location_image(c);

        // Turn on graphics if off.
        if (!gfx_on)
        {
            gfx_on = 1;
            gfx_window_height = MAX_IMAGE_HEIGHT;
            gfx_window_height_adjustment = 0;
            layer2_set_clip_window(0, 0, 256, MAX_IMAGE_HEIGHT);
            layer2_config(true);
            ioctl(FD_STDOUT, IOCTL_OTERM_SCROLL_LIMIT, (TEXT_WINDOW_HEIGHT - MAX_IMAGE_HEIGHT_IN_CHARS));
        }
    }
    else
    {
        // Turn off graphics if on.
        if (gfx_on)
        {
            gfx_on = 0;
            gfx_window_height = 0;
            gfx_window_height_adjustment = 0;
            layer2_config(false);
            ioctl(FD_STDOUT, IOCTL_OTERM_SCROLL_LIMIT, TEXT_WINDOW_HEIGHT);
        }
    }
#endif
}

type8 ms_save_file(type8 *name, type8 *ptr, type16 size)
{
    return access_file(get_filename(name), ptr, size, WRITE_FILE);
}

type8 ms_load_file(type8 *name, type8 *ptr, type16 size)
{
    type8 status = access_file(get_filename(name), ptr, size, READ_FILE);
    if (!status)
    {
        // Emit a "look" to update location description after a "load".
        add_command("look");
    }
    return status;
}

void ms_fatal(type8 *txt) __z88dk_fastcall
{
    printf("\nFatal error: %s\n", txt);
    ms_status();
    wait_key();
    exit(1);
}

int main(void)
{
    type8 running = 1;

    init_hardware();
#if USE_GFX
    show_intro();
#endif
    create_screen();

#if USE_PROG_FORMAT && !USE_SCRIPT
    add_command("verbose");
#endif

#if USE_IMAGE_SLIDESHOW
    run_image_slideshow();
#endif

#if USE_SCRIPT
    open_script();
#endif

    atexit(exit_handler);

#if USE_PROG_FORMAT
    // Display wait image.
    ms_showpic(WAIT_IMAGE, 2);
#endif

    if (!ms_init(GAME_FILE))
    {
        fputs("Could not start game.\n", stdout);
        return 1;
    }

#if USE_PROG_FORMAT
    ms_config();
#endif

    // Make sure the start location image is displayed immediately.
    ms_showpic(GAME_START_IMAGE, 2);

#ifdef GAME_VERSION_STR
    printf("%s\n", GAME_VERSION_STR);
#endif

    while (running)
    {
        running = ms_rungame();
    }

    return 0;
}
