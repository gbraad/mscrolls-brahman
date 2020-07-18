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
 * Note: MMU slot 2 is temporarily used when accessing the layer 2 off-screen
 * memory area.
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

static void clear_sprite_ram(void);

static void transfer_sprite_ram(void);

static struct fzx_state fzx_state;

void init_status_bar(struct fzx_font *status_font)
{
    struct layer2_fzx_state l2_fzx_state;

    fzx_state.jp = 195;
    fzx_state.fzx_draw = _layer2_fzx_draw;
    fzx_state.font = status_font;
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

    /*
     * CSpect only supports 12 sprites per line (#CSpect does not have that limitation).
     * When testing with CSpect, we have to make 4 of the 16 sprites in the status bar
     * invisible. Sprites 8 to 11 in the status bar are almost never used and can be made
     * invisible.
     */
#if USE_CSPECT
    sprite_select_slot(SPRITE_SLOT + 8);
    for (uint8_t i = 8; i != 12; ++i)
    {
        sprite_set_attributes(SPRITE_SLOT + i, 32 + 16 * i, 16, false);
    }
#endif
}

void print_status(uint8_t *room_name, uint8_t *score_turn)
{
    clear_sprite_ram();

    fzx_state.x = 0;
    fzx_puts(&fzx_state, room_name);

    fzx_state.x = STATUS_SCORE_COLUMN;
    fzx_puts(&fzx_state, score_turn);

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
