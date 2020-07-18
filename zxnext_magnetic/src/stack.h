/*******************************************************************************
 * Stefan Bylund 2018
 *
 * C API for peak_stack_usage() function in stack.asm.
 ******************************************************************************/

#ifndef _STACK_H
#define _STACK_H

#include <stdint.h>
#include "ide_friendly.h"

extern type16 peak_stack_usage(void) __preserves_regs(d,e);

#endif
