/*******************************************************************************
 * Stefan Bylund 2018
 *
 * C API for graphics utility functions in gfx_util.asm.
 ******************************************************************************/

#ifndef _GFX_UTIL_H
#define _GFX_UTIL_H

#include <stdint.h>
#include "ide_friendly.h"

extern void image_mouse_scroll(int8_t image_height_change) __z88dk_fastcall;

#endif
