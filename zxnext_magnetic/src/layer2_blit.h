/*******************************************************************************
 * Stefan Bylund 2020
 *
 * Layer 2 blit functions.
 *
 * Note: MMU slot 2 is temporarily used when accessing the layer 2 screen.
 ******************************************************************************/

#ifndef _LAYER2_BLIT_H
#define _LAYER2_BLIT_H

#include <stdint.h>
#include "layer2.h"

/*
 * Specifies the blit context for the layer2_blit() and layer2_blit_trans()
 * blit functions.
 *
 * Note: The src, y and height fields are modified by the callee.
 */
typedef struct layer2_blit_context
{
    uint8_t screen_base_page;
    uint8_t *src;
    uint16_t src_line_width;
    uint8_t x;
    uint8_t y;
    uint16_t width;
    uint8_t height;
    uint8_t transparency_color;
} layer2_blit_context_t;

/*
 * Blit a rectangle of the specified width (1-256) and height (1-192) from a
 * source bitmap to the specified layer 2 screen at the given X (0-255) and
 * Y (0-191) coordinates.
 *
 * The source bitmap pointer should point to the top-left pixel in the source
 * rectangle. The source line width is the width of the source bitmap which
 * may be wider than the width of the rectangle to blit from it.
 *
 * Note: It is the responsibility of the caller to make sure that the source
 * bitmap is paged in to the address space.
 */
void layer2_blit(layer2_blit_context_t *blit_context) __z88dk_fastcall;

/*
 * Blit a rectangle of the specified width (1-256) and height (1-192) from a
 * source bitmap to the specified layer 2 screen at the given X (0-255) and
 * Y (0-191) coordinates.
 *
 * The source bitmap pointer should point to the top-left pixel in the source
 * rectangle. The source line width is the width of the source bitmap which
 * may be wider than the width of the rectangle to blit from it.
 *
 * Transparent pixels (i.e. pixels matching the specified transparency color)
 * in the source rectangle are skipped.
 *
 * Note: It is the responsibility of the caller to make sure that the source
 * bitmap is paged in to the address space.
 */
void layer2_blit_trans(layer2_blit_context_t *blit_context) __z88dk_fastcall;

/*
 * Blit shadow layer 2 screen to main layer 2 screen or vice versa.
 * Note: MMU slot 0 will be temporarily used and then restored with the ROM.
 */
void layer2_blit_screen(layer2_screen_t dst, layer2_screen_t src) __z88dk_callee;

/*
 * Blit a full-width section from the specified layer 2 screen (main or shadow)
 * starting at the specified Y coordinate and with the specified height to the
 * given destination memory area.
 *
 * Note: It is the responsibility of the caller to make sure that the
 * destination memory area is paged in to the address space.
 */
void layer2_blit_screen_section(uint8_t *dst, uint8_t src_screen_base_page, uint8_t y, uint8_t height);

#endif
