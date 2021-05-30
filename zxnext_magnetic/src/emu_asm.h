/*******************************************************************************
 * Stefan Bylund 2021
 *
 * This module contains functions originally from emu.c that have
 * been compiled to Z80 assembly and then manually optimized.
 ******************************************************************************/

#ifndef _EMU_ASM_H
#define _EMU_ASM_H

#include "defs.h"
#include "ide_friendly.h"

/*
 * Page in the ROM to MMU slots 0 and 1.
 */
void page_in_rom(void) __preserves_regs(b,c,d,e,h,l);

/*
 * Page in the current and next game pages to MMU slots 0 and 1.
 */
void page_in_game(void) __preserves_regs(b,c,d,e,h,l);

/*
 * Convert the given virtual pointer to an effective code pointer.
 * The returned pointer will point into a code page in slot 0 with the next
 * code page in slot 1 and update the current_page global variable to the
 * page in slot 0.
 */
type8 *effective(type32 ptr) __z88dk_fastcall;

/*
 * Convert the given virtual pointer to an effective string pointer.
 * The returned pointer will point into a string page in slot 0 with the next
 * string page in slot 1 and update the current_page global variable to the
 * page in slot 0.
 */
type8 *effective_string(type32 ptr) __z88dk_fastcall;

/*
 * Convert the given virtual pointer to an effective string2 pointer.
 * The returned pointer will point into a string2 page in slot 0 with the next
 * string2 page in slot 1 and update the current_page global variable to the
 * page in slot 0.
 */
type8 *effective_string2(type32 ptr) __z88dk_fastcall;

/*
 * Convert the given virtual pointer to an effective string3 pointer.
 * The returned pointer will point into a string3 page in slot 0 with the next
 * string3 page in slot 1 and update the current_page global variable to the
 * page in slot 0.
 */
type8 *effective_string3(type32 ptr) __z88dk_fastcall;

/*
 * Convert the given virtual pointer to an effective dictionary pointer.
 * The returned pointer will point into a dictionary page in slot 0 with the next
 * dictionary page in slot 1 and update the current_page global variable to the
 * page in slot 0.
 */
type8 *effective_dict(type32 ptr) __z88dk_fastcall;

/*
 * Return the page number for the given linear address.
 */
type8 addr_to_page(type32 addr) __preserves_regs(b,c,d,e) __z88dk_fastcall;

/*
 * Copy the longword pointed to by arg2 to arg2_buf
 * and then set arg2 to point to arg2_buf instead.
 */
void save_arg2(void) __preserves_regs(a);

/*
 * Return an initial seed for ms_seed().
 */
type16 seed_r(void) __preserves_regs(b,c,d,e);

/*
 * Align register pointer for word/byte accesses.
 */
type8 *reg_align(type8 *ptr, type8 size) __preserves_regs(b,c) __z88dk_callee;

/*
 * Read the specified register as a word.
 * 0 - 7 denotes D0 - D7 and 8 - 15 denotes A0 - A7.
 */
type16 read_reg_w(type8 i) __preserves_regs(b,c,d,e) __z88dk_fastcall;

/*
 * Read the specified register as a longword.
 * 0 - 7 denotes D0 - D7 and 8 - 15 denotes A0 - A7.
 */
type32 read_reg_l(type8 i) __preserves_regs(b,c) __z88dk_fastcall;

/*
 * Read the specified data register as a byte.
 */
type8 read_dreg_b(type8 i) __preserves_regs(b,c,d,e) __z88dk_fastcall;

/*
 * Read the specified data register as a word.
 */
type16 read_dreg_w(type8 i) __preserves_regs(b,c,d,e) __z88dk_fastcall;

/*
 * Read the specified data register as a longword.
 */
type32 read_dreg_l(type8 i) __preserves_regs(b,c) __z88dk_fastcall;

/*
 * Read the specified address register as a word.
 */
type16 read_areg_w(type8 i) __preserves_regs(b,c,d,e) __z88dk_fastcall;

/*
 * Read the specified address register as a longword.
 */
type32 read_areg_l(type8 i) __preserves_regs(b,c) __z88dk_fastcall;

/*
 * Write a byte value to the specified data register.
 */
void write_dreg_b(type8 i, type8 val) __preserves_regs(b,c) __z88dk_callee;

/*
 * Write a word value to the specified data register.
 */
void write_dreg_w(type8 i, type16 val) __preserves_regs(b,c) __z88dk_callee;

/*
 * Write a longword value to the specified data register.
 */
void write_dreg_l(type8 i, type32 val) __preserves_regs(b,c) __z88dk_callee;

/*
 * Write a word value to the specified address register.
 */
void write_areg_w(type8 i, type16 val) __preserves_regs(b,c) __z88dk_callee;

/*
 * Write a longword value to the specified address register.
 */
void write_areg_l(type8 i, type32 val) __preserves_regs(b,c) __z88dk_callee;

/*
 * Increment the word in the specified data register.
 */
void inc_dreg_w(type8 i) __z88dk_fastcall;

/*
 * Decrement the word in the specified data register.
 */
void dec_dreg_w(type8 i) __z88dk_fastcall;

/*
 * Increment the word in the specified address register.
 */
void inc_areg_w(type8 i) __z88dk_fastcall;

/*
 * Add the specified value to the word in the specified address register.
 */
void inc_var_areg_w(type8 i, type16 val) __z88dk_callee;

/*
 * Extract addressing mode information.
 */
void set_info(type8 b) __preserves_regs(b,c,d,e,h) __z88dk_fastcall;

/*
 * Read a word and increase the PC.
 */
void read_word(void);

/*
 * Sets the first argument of an instruction based on the addressing mode.
 */
void set_arg1(void);

/*
 * Sets the second argument of an instruction (without size alignment).
 */
void set_arg2_nosize(type8 use_dx, type8 b) __preserves_regs(b,c) __z88dk_callee;

/*
 * Sets the second argument of an instruction (with size alignment).
 */
void set_arg2(type8 use_dx, type8 b) __preserves_regs(b,c) __z88dk_callee;

/*
 * Swap arg1 and arg2.
 */
void swap_args(void) __preserves_regs(a,b,c);

/*
 * Push a longword on the stack.
 */
void push(type32 c) __z88dk_fastcall;

/*
 * Pop a longword off the stack.
 */
type32 pop(void);

/*
 * Check addressing mode and get arguments.
 */
void get_arg(void);

/*
 * Set the Z and N flags.
 */
void set_flags(void) __preserves_regs(b,c);

/*
 * Evaluate the given byte and return 0x00 or
 * 0xFF depending on its value and the flags.
 */
type8 condition(type8 b) __preserves_regs(d,e) __z88dk_fastcall;

/*
 * Set PC to branch target address.
 */
void branch(type8 b) __z88dk_fastcall;

/*
 * Add instructions (add/addi/addq).
 */
void do_add(void);

/*
 * Add address instruction (adda).
 */
void do_adda(void);

/*
 * Sub instructions (sub/subq).
 */
void do_sub(void);

/*
 * Sub address instruction (suba).
 */
void do_suba(void);

/*
 * Exclusive or instruction (eor).
 */
void do_eor(void) __preserves_regs(c);

/*
 * And instruction (and).
 */
void do_and(void) __preserves_regs(c);

/*
 * Or instruction (or).
 */
void do_or(void) __preserves_regs(c);

/*
 * Compare instruction (cmp).
 */
void do_cmp(void);

/*
 * Move byte instruction (move.b).
 */
void do_move_b(void) __preserves_regs(b,c);

/*
 * Move word instruction (move.w).
 */
void do_move_w(void);

/*
 * Move longword instruction (move.l).
 */
void do_move_l(void);

/*
 * Bit test instruction (btst).
 */
type8 do_btst(type8 a) __preserves_regs(c) __z88dk_fastcall;

/*
 * Do a bit operation instruction (bchg/bclr/bset).
 */
void do_bop(type8 b, type8 a) __z88dk_callee;

/*
 * Check if a bit test instruction (btst).
 */
void check_btst(void);

/*
 * Check if a load effective address instruction (lea).
 */
void check_lea(void);

/*
 * Check if a move multiple registers to memory instruction (movem).
 */
void check_movem(void);

/*
 * Check if a move memory to multiple registers instruction (movem).
 */
void check_movem2(void);

#endif
