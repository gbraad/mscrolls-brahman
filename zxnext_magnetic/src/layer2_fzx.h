/*******************************************************************************
 * Stefan Bylund 2019
 *
 * The _layer2_fzx_draw() function allows the ULA FZX proportional font driver
 * in Z88DK to be used for printing text on the layer 2 screen or a layer 2
 * off-screen memory area.
 *
 * This method effectively redirects the output from the ULA screen to the layer
 * 2 screen. This is a bit of a hack but is still very useful and can be used
 * with both the API in fzx.h and the terminal output functions in stdio.h
 * (except for clearing and scrolling of the terminal).
 *
 * Note: MMU slot 2 is temporarily used when accessing the layer 2 off-screen
 * memory area.
 *
 * How to use with fzx.h:
 *
 * 1. Set the layer 2 specific layer2_fzx_state structure:
 *    struct layer2_fzx_state l2_fzx_state;
 *    l2_fzx_state.base_page = 16; // layer 2 screen base page
 *    l2_fzx_state.color = 0xFF;   // Text color
 *    layer2_fzx_set_state(&l2_fzx_state);
 *
 * 2. Set the fzx_state structure to be used with layer 2:
 *    struct fzx_state fzx_state;
 *    fzx_state.jp = 195; // z80 jump instruction
 *    fzx_state.fzx_draw = _layer2_fzx_draw;
 *    fzx_state.font = &ff_ao_Prefect; // FZX font
 *    fzx_state.x = 0; // Current x printing position
 *    fzx_state.y = 0; // Current y printing position
 *    fzx_state.paper.x = 0;
 *    fzx_state.paper.width = 256;
 *    fzx_state.paper.y = 0;
 *    fzx_state.paper.height = 192;
 *    fzx_state.left_margin = 0;
 *    fzx_state.space_expand = 0;
 *    fzx_state.fgnd_attr = 0;   // Prevent ULA attribute writing
 *    fzx_state.fgnd_mask = 255; // Prevent ULA attribute writing
 *
 * 3. Start printing on the layer screen, e.g.:
 *    fzx_puts(&fzx_state, "Hello World");
 *
 * How to use with stdio.h:
 *
 * 1. Set the layer 2 specific layer2_fzx_state structure:
 *    struct layer2_fzx_state l2_fzx_state;
 *    l2_fzx_state.base_page = 16; // layer 2 screen base page
 *    l2_fzx_state.color = 0xFF;   // Text color
 *    layer2_fzx_set_state(&l2_fzx_state);
 *
 * 2. Update the fzx_state structure for stdout to be used with layer 2:
 *    struct fzx_state fzx_state;
 *    ioctl(1, IOCTL_OTERM_FZX_GET_FZX_STATE, &fzx_state);
 *    fzx_state.fzx_draw = _layer2_fzx_draw;
 *    fzx_state.fgnd_attr = 0;   // Prevent ULA attribute writing
 *    fzx_state.fgnd_mask = 255; // Prevent ULA attribute writing
 *    ioctl(1, IOCTL_OTERM_FZX_SET_FZX_STATE, &fzx_state);
 *
 * 3. Start printing on the layer 2 screen via stdout, e.g.:
 *    printf("The magic number is %d", magic_number);
 *
 ******************************************************************************/

#ifndef _LAYER2_FZX_H
#define _LAYER2_FZX_H

#include <stdint.h>
#include "ide_friendly.h"

struct layer2_fzx_state
{
    uint8_t base_page; // Layer 2 screen base page
    uint8_t color;     // Text color (layer 2 palette index)
};

extern void layer2_fzx_set_state(struct layer2_fzx_state *fzx_state) __z88dk_fastcall __preserves_regs(b,c,d,e);

extern void _layer2_fzx_draw(void);

#endif
