/*******************************************************************************
 * Stefan Bylund 2019
 *
 * Module for printing the status in the status bar.
 *
 * The status bar consists of 16 hardware sprites (16 * 16 pixels) above the
 * screen in the top border. The status text is first drawn in a 4 KB off-screen
 * layer 2 memory area (256 * 16 pixels) in a separate MMU page using layer2_fzx.h
 * and then transferred to the sprite BRAM for the 16 sprites.
 *
 * MMU page 28 is used for holding the 4 KB off-screen layer 2 memory area and
 * is temporarily paged in to MMU slot 2 when being accessed.
 * In Timex hi-res mode, this data is followed by a standard resolution font
 * (806 bytes) that is used when printing in the status bar. The font is located
 * at offset 0x1000 in MMU page 28 and is temporarily paged in to MMU slot 0
 * when being used.
 ******************************************************************************/

#include <arch/zxn.h>
#include <intrinsic.h>
#include <z80.h>
#include <font/fzx.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "zconfig.h"
#include "status_bar.h"
#include "layer2_fzx.h"
#include "sprite.h"
#include "ide_friendly.h"

#define SPRITE_RAM_PAGE 28

#define SPRITE_RAM_ADDRESS ((uint8_t *) 0x4000)

#define SPRITE_SLOT 12

#define SPRITE_TRANS_COLOR 0xE3

#define STATUS_SCORE_COLUMN 208

#if USE_TIMEX_HIRES
extern uint8_t status_font[];
#endif

static void clear_sprite_ram(void);

static void transfer_sprite_ram(void);

static struct fzx_state fzx_state;

void init_status_bar(struct fzx_font *font)
{
    struct layer2_fzx_state l2_fzx_state;

#if USE_TIMEX_HIRES
    font = (struct fzx_font *) status_font;
#endif

    fzx_state.jp = 195;
    fzx_state.fzx_draw = _layer2_fzx_draw;
    fzx_state.font = font;
    fzx_state.x = 0;
    fzx_state.y = 8;
    fzx_state.paper.x = 0;
    fzx_state.paper.width = 256;
    fzx_state.paper.y = 0;
    fzx_state.paper.height = 16;
    fzx_state.left_margin = 0;
    fzx_state.space_expand = 0;
    fzx_state.fgnd_attr = 0;
    fzx_state.fgnd_mask = 255;

    l2_fzx_state.base_page = SPRITE_RAM_PAGE;
    l2_fzx_state.color = STATUS_FONT_COLOR;
    layer2_fzx_set_state(&l2_fzx_state);

    clear_sprite_ram();
    transfer_sprite_ram();

    sprite_select_slot(SPRITE_SLOT);
    for (uint8_t i = 0; i != 16; ++i)
    {
        sprite_set_attributes(SPRITE_SLOT + i, 32 + 16 * i, 16, true);
    }
}

void print_status(uint8_t *room_name, uint8_t *score_turn)
{
#if USE_TIMEX_HIRES
    uint8_t old_mmu0_page;
#endif

    clear_sprite_ram();

    // Before printing, page in the standard resolution font if Timex hi-res mode.
#if USE_TIMEX_HIRES
    old_mmu0_page = ZXN_READ_MMU0();
    ZXN_WRITE_MMU0(SPRITE_RAM_PAGE);
#endif

    fzx_state.x = 0;
    fzx_puts(&fzx_state, room_name);

    fzx_state.x = STATUS_SCORE_COLUMN;
    fzx_puts(&fzx_state, score_turn);

#if USE_TIMEX_HIRES
    ZXN_WRITE_MMU0(old_mmu0_page);
#endif

    transfer_sprite_ram();
}

static void clear_sprite_ram(void)
{
    ZXN_WRITE_MMU2(SPRITE_RAM_PAGE);
    memset(SPRITE_RAM_ADDRESS, SPRITE_TRANS_COLOR, 256 * 16);
    ZXN_WRITE_MMU2(10);
}

static void transfer_sprite_ram(void)
{
    uint8_t *src;

    ZXN_WRITE_MMU2(SPRITE_RAM_PAGE);

    intrinsic_di();

    sprite_select_slot(SPRITE_SLOT);

    for (uint8_t i = 0; i != 16; ++i)
    {
        src = SPRITE_RAM_ADDRESS + 16 * i;

        for (uint8_t j = 0; j != 16; ++j)
        {
            z80_otir((void *) src, __IO_SPRITE_PATTERN, 16);
            src += 256;
        }
    }

    intrinsic_ei();

    ZXN_WRITE_MMU2(10);
}
