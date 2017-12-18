
/* Note : this was taken from m68k-opcodes.h which comes with GDB and
 * so is (C) Richard M. Stallman */

#include "inttype.h"
#include "opcodes.h"

/* We store four bytes of opcode for all opcodes because that
   is the most any of them need.  The actual length of an instruction
   is always at least 2 bytes, and is as much longer as necessary to
   hold the operands it has.

   The match component is a mask saying which bits must match
   particular opcode in order for an instruction to be an instance
   of that opcode.

   The args component is a string containing two characters
   for each operand of the instruction.  The first specifies
   the kind of operand; the second, the place it is stored.  */

/* Kinds of operands:
   D  data register only.  Stored as 3 bits.
   A  address register only.  Stored as 3 bits.
   F  floating point coprocessor register only.   Stored as 3 bits.
   R  either kind of register.  Stored as 4 bits.
   O  an offset (or width): immediate data 0-31 or data register.
      Stored as 6 bits in special format for BF.. insns.
   +  autoincrement only.  Stored as 3 bits (number of the address register).
   -  autodecrement only.  Stored as 3 bits (number of the address register).
   Q  quick immediate data.  Stored as 3 bits.
      This matches an immediate operand only when value is in range 1 . 8.
   M  moveq immediate data.  Stored as 8 bits.
      This matches an immediate operand only when value is in range -128.127
   T  trap vector immediate data.  Stored as 4 bits.

   k  K-factor for fmove.p instruction.   Stored as a 7-bit constant or
      a three bit register offset, depending on the field type.

   #  immediate data.  Stored in special places (b, w or l)
      which say how many bits to store.
   ^  immediate data for floating point instructions.   Special places
      are offset by 2 bytes from '#'..
   B  pc-relative address, converted to an offset
      that is treated as immediate data.
   d  displacement and register.  Stores the register as 3 bits
      and stores the displacement in the entire second word.

   C  the CCR.  No need to store it; this is just for filtering validity.
   S  the SR.  No need to store, just as with CCR.
   U  the USP.  No need to store, just as with CCR.

   c  control register for movec

   I  Coprocessor ID.   Not printed if 1.   The Coprocessor ID is always
      extracted from the 'd' field of word one, which means that an extended
      coprocessor opcode can be skipped using the 'i' place, if needed.

   s  System Control register for the floating point coprocessor.

 These specifiy various classes of addressing modes.
 They are all stored as 6 bits using an address mode and a register number;
 they differ in which addressing modes they match.

   *  all					(modes 0-6,7.*)
   ~  alterable memory				(modes 2-6,7.0,7.1)(not 0,1,7.~)
   %  alterable					(modes 0-6,7.0,7.1)(not 7.~)
   ;  data					(modes 0,2-6,7.*)(not 1)
   @  data, but not immediate			(modes 0,2-6,7.???)(not 1,7.?)  This may really be ;, the 68020 book says it is
   !  control					(modes 2,5,6,7.*-)(not 0,1,3,4,7.4)
   &  alterable control				(modes 2,5,6,7.0,7.1)(not 0,1,7.???)
   $  alterable data				(modes 0,2-6,7.0,7.1)(not 1,7.~)
   ?  alterable control, or data register	(modes 0,2,5,6,7.0,7.1)(not 1,3,4,7.~)
   /  control, or data register			(modes 0,2,5,6,7.0,7.1,7.2,7.3)(not 1,3,4,7.4)
*/

/* Places to put an operand, for non-general operands:
   s  source, low bits of first word.
   d  dest, shifted 9 in first word
   1  second word, shifted 12
   2  second word, shifted 6
   3  second word, shifted 0
   4  third word, shifted 12
   5  third word, shifted 6
   6  third word, shifted 0
   7  second word, shifted 7
   8  second word, shifted 10
   D  store in both place 1 and place 3; for divul and divsl.
   b  second word, low byte
   w  second word (entire)
   l  second and third word (entire)
   g  branch offset for bra and similar instructions.
      The place to store depends on the magnitude of offset.
   t  store in both place 7 and place 8; for floating point operations
   c  branch offset for cpBcc operations.
      The place to store is word two if bit six of word one is zero,
      and words two and three if bit six of word one is one.
   i  Increment by two, to skip over coprocessor extended operands.   Only
      works with the 'I' format.
   k  Dynamic K-factor field.   Bits 6-4 of word 2, used as a register number.
      Also used for dynamic fmovem instruction.
   C  floating point coprocessor constant - 7 bits.  Also used for static
      K-factors..

 Places to put operand, for general operands:
   d  destination, shifted 6 bits in first word
   b  source, at low bit of first word, and immediate uses one byte
   w  source, at low bit of first word, and immediate uses two bytes
   l  source, at low bit of first word, and immediate uses four bytes
   s  source, at low bit of first word.
      Used sometimes in contexts where immediate is not allowed anyway.
   f  single precision float, low bit of 1st word, immediate uses 4 bytes
   F  double precision float, low bit of 1st word, immediate uses 8 bytes
   x  extended precision float, low bit of 1st word, immediate uses 12 bytes
   p  packed float, low bit of 1st word, immediate uses 12 bytes
*/

#define one(x) ((x) << 16)
#define two(x, y) (((x) << 16) + y)

/* The assembler requires that this array be sorted as follows:
   all instances of the same mnemonic must be consecutive.
   All instances of the same mnemonic with the same number of operands
   must be consecutive.
 */
struct m68k_opcode m68k_opcodes[] =
{
#ifdef EMU
  {O_EMU, "ret", one (0xa200), one (0xffff), ""},	/* same as rts */
  {O_EMU, "test_b", one (0123000), one (0177700), "$b"}, /* same as tst.b */
  {O_EMU, "test_w", one (0123100), one (0177700), "$w"}, /* same as tst.w */
  {O_EMU, "test_l", one (0123200), one (0177700), "$l"}, /* same as tst.l */
  {O_EMU, "test.b", one (0123000), one (0177700), "$b"}, /* same as tst.b */
  {O_EMU, "test.w", one (0123100), one (0177700), "$w"}, /* same as tst.w */
  {O_EMU, "test.l", one (0123200), one (0177700), "$l"}, /* same as tst.l */
  {O_EMU, "call", one (0122000), one (0xff00), "Bg"},	 /* bsr */
  {O_EMU, "call_s", one (0122000), one (0xff00), "Bs"}, /* bsr.s */
  {O_EMU, "call.s", one (0122000), one (0xff00), "Bs"}, /* bsr.s */
  {O_EMU, "call_l", one (0122000), one (0xff00), "Bw"}, /* bsr.l */
  {O_EMU, "call.l", one (0122000), one (0xff00), "Bw"}, /* bsr.l */
  {O_EMU|O_MOVEM2, "push_w", one (0124247), one (0177777), "#w"}, /* movem.w ...,-(A7) */
  {O_EMU|O_MOVEM2, "push.w", one (0124247), one (0177777), "#w"}, /* movem.w ...,-(A7) */
  {O_EMU|O_MOVEM, "pull_w", one (0125237), one (0177777), "#w"},  /* movem.w (A7)+,... */
  {O_EMU|O_MOVEM, "pull.w", one (0125237), one (0177777), "#w"},  /* movem.w (A7)+,... */
  {O_EMU|O_MOVEM, "pull", one (0125237), one (0177777), "#w"},  /* movem.w (A7)+,... */
  {O_EMU|O_MOVEM2, "push_l", one (0124347), one (0177777), "#w"}, /* movem.l ...,-(A7) */
  {O_EMU|O_MOVEM2, "push.l", one (0124347), one (0177777), "#w"}, /* movem.l ...,-(A7) */
  {O_EMU|O_MOVEM, "pull_l", one (0125337), one (0177777), "#w"},  /* movem.l (A7)+,... */
  {O_EMU|O_MOVEM, "pull.l", one (0125337), one (0177777), "#w"},  /* movem.l (A7)+,... */

#endif // EMU
#ifdef MR

  {0, "ret", one (0047165), one (0177777), ""},		/* same as rts */
  {0, "test_b", one (0045000), one (0177700), "$b"},	/* same as tst.b */
  {0, "test_w", one (0045100), one (0177700), "$w"},	/* same as tst.w */
  {0, "test_l", one (0045200), one (0177700), "$l"},	/* same as tst.l */
  {0, "test.b", one (0045000), one (0177700), "$b"},	/* same as tst.b */
  {0, "test.w", one (0045100), one (0177700), "$w"},	/* same as tst.w */
  {0, "test.l", one (0045200), one (0177700), "$l"},	/* same as tst.l */
  {0, "call", one (0060400), one (0xff00), "Bg"},	 /* bsr */
  {0, "call_s", one (0060400), one (0xff00), "Bs"}, /* bsr.s */
  {0, "call.s", one (0060400), one (0xff00), "Bs"}, /* bsr.s */
  {0, "call_l", one (0060400), one (0xff00), "Bw"}, /* bsr.l */
  {0, "call.l", one (0060400), one (0xff00), "Bw"}, /* bsr.l */
  {O_MOVEM2, "push_w", one (0044247), one (0177777), "#w"}, /* movem.w ...,-(A7) */
  {O_MOVEM2, "push.w", one (0044247), one (0177777), "#w"}, /* movem.w ...,-(A7) */
  {O_MOVEM, "pull_w", one (0046237), one (0177777), "#w"},  /* movem.w (A7)+,... */
  {O_MOVEM, "pull.w", one (0046237), one (0177777), "#w"},  /* movem.w (A7)+,... */
  {O_MOVEM, "pull", one (0046237), one (0177777), "#w"},  /* movem.w (A7)+,... */
  {O_MOVEM2, "push_l", one (0044347), one (0177777), "#w"}, /* movem.l ...,-(A7) */
  {O_MOVEM2, "push.l", one (0044347), one (0177777), "#w"}, /* movem.l ...,-(A7) */
  {O_MOVEM, "pull_l", one (0046337), one (0177777), "#w"},  /* movem.l (A7)+,... */
  {O_MOVEM, "pull.l", one (0046337), one (0177777), "#w"},  /* movem.l (A7)+,... */

#endif // MR

  {O_NEMU, "abcd", one (0140400), one (0170770), "DsDd"},
  {O_NEMU, "abcd", one (0140410), one (0170770), "-s-d"},

		/* Add instructions */
  {0, "adda.l", one (0150700), one (0170700), "*lAd"},
  {0, "adda.w", one (0150300), one (0170700), "*wAd"},
  {0, "addi.b", one (0003000), one (0177700), "#b$b"},
  {0, "addi.l", one (0003200), one (0177700), "#l$l"},
  {0, "addi.w", one (0003100), one (0177700), "#w$w"},
  {0, "addq.b", one (0050000), one (0170700), "Qd$b"},
  {0, "addq.l", one (0050200), one (0170700), "Qd%l"},
  {0, "addq.w", one (0050100), one (0170700), "Qd%w"},
  {O_WARN, "addq", one (0050100), one (0170700), "Qd%w"},

  {0, "add.b", one (0003000), one (0177700), "#b$b"}, /* addi written as add */
  {0, "add.b", one (0150000), one (0170700), ";bDd"}, /* addb <ea>, Dd */
  {0, "add.b", one (0150400), one (0170700), "Dd~b"}, /* addb Dd, <ea> */

  {0, "add.w", one (0003100), one (0177700), "#w$w"}, /* addi written as add */
  {0, "add.w", one (0150300), one (0170700), "*wAd"}, /* adda written as add */
  {0, "add.w", one (0150100), one (0170700), "*wDd"}, /* addw <ea>, Dd */
  {0, "add.w", one (0150500), one (0170700), "Dd~w"}, /* addw Dd, <ea> */

  {O_WARN, "add", one (0003100), one (0177700), "#w$w"}, /* addiw written as add */
  {O_WARN, "add", one (0150300), one (0170700), "*wAd"}, /* addaw written as add */
  {O_WARN, "add", one (0150100), one (0170700), "*wDd"}, /* addw <ea>, Dd */
  {O_WARN, "add", one (0150500), one (0170700), "Dd~w"}, /* addw Dd, <ea> */

  {0, "add.l", one (0003200), one (0177700), "#l$l"}, /* addi written as add */
  {0, "add.l", one (0150700), one (0170700), "*lAd"}, /* adda written as add */
  {0, "add.l", one (0150200), one (0170700), "*lDd"}, /* addl <ea>, Dd */
  {0, "add.l", one (0150600), one (0170700), "Dd~l"}, /* addl Dd, <ea> */

  {O_NEMU, "addx.b", one (0150400), one (0170770), "DsDd"},
  {O_NEMU, "addx.b", one (0150410), one (0170770), "-s-d"},
  {O_NEMU, "addx.l", one (0150600), one (0170770), "DsDd"},
  {O_NEMU, "addx.l", one (0150610), one (0170770), "-s-d"},
  {O_NEMU, "addx.w", one (0150500), one (0170770), "DsDd"},
  {O_NEMU, "addx.w", one (0150510), one (0170770), "-s-d"},

  {0, "andi", one (0001074), one (0177777), "#bCb"}, /* andi to ccr */
  {0, "andi", one (0001174), one (0177777), "#wSw"}, /* andi to sr */
  {0, "andi.b", one (0001000), one (0177700), "#b$b"},
  {0, "andi.w", one (0001100), one (0177700), "#w$w"},
  {O_WARN, "andi", one (0001100), one (0177700), "#w$w"},
  {0, "andi.l", one (0001200), one (0177700), "#l$l"},

  {0, "and", one (0001074), one (0177777), "#bCb"}, /* andi to ccr */
  {0, "and", one (0001174), one (0177777), "#wSw"}, /* andi to sr */
  {O_WARN, "and", one (0001100), one (0177700), "#w$w"}, /* andi written as or */
  {O_WARN, "and", one (0140100), one (0170700), ";wDd"}, /* memory to register */
  {O_WARN, "and", one (0140500), one (0170700), "Dd~w"}, /* register to memory */
  {0, "and.b", one (0001000), one (0177700), "#b$b"}, /* andi written as or */
  {0, "and.b", one (0140000), one (0170700), ";bDd"}, /* memory to register */
  {0, "and.b", one (0140400), one (0170700), "Dd~b"}, /* register to memory */
  {0, "and.w", one (0001100), one (0177700), "#w$w"}, /* andi written as or */
  {0, "and.w", one (0140100), one (0170700), ";wDd"}, /* memory to register */
  {0, "and.w", one (0140500), one (0170700), "Dd~w"}, /* register to memory */
  {O_WARN, "and.w", one (0001074), one (0177777), "#bCb"}, /* andi to ccr */
  {O_WARN, "and.w", one (0001174), one (0177777), "#wSw"}, /* andi to sr */
  {0, "and.l", one (0001200), one (0177700), "#l$l"}, /* andi written as or */
  {0, "and.l", one (0140200), one (0170700), ";lDd"}, /* memory to register */
  {0, "and.l", one (0140600), one (0170700), "Dd~l"}, /* register to memory */

  {0, "asl.b", one (0160400), one (0170770), "QdDs"},
  {0, "asl.b", one (0160440), one (0170770), "DdDs"},
  {0, "asl.l", one (0160600), one (0170770), "QdDs"},
  {0, "asl.l", one (0160640), one (0170770), "DdDs"},
  {0, "asl.w", one (0160500), one (0170770), "QdDs"},
  {0, "asl.w", one (0160540), one (0170770), "DdDs"},
  {0, "asl.w", one (0160700), one (0177700), "~s"},	/* Shift memory */
  {O_WARN, "asl", one (0160500), one (0170770), "QdDs"},
  {O_WARN, "asl", one (0160540), one (0170770), "DdDs"},
  {O_WARN, "asl", one (0160700), one (0177700), "~s"},	/* Shift memory */
  {0, "asr.b", one (0160000), one (0170770), "QdDs"},
  {0, "asr.b", one (0160040), one (0170770), "DdDs"},
  {0, "asr.l", one (0160200), one (0170770), "QdDs"},
  {0, "asr.l", one (0160240), one (0170770), "DdDs"},
  {0, "asr.w", one (0160100), one (0170770), "QdDs"},
  {0, "asr.w", one (0160140), one (0170770), "DdDs"},
  {0, "asr.w", one (0160300), one (0177700), "~s"},	/* Shift memory */
  {O_WARN, "asr", one (0160100), one (0170770), "QdDs"},
  {O_WARN, "asr", one (0160140), one (0170770), "DdDs"},
  {O_WARN, "asr", one (0160300), one (0177700), "~s"},	/* Shift memory */

  {0, "bhi", one (0061000), one (0177400), "Bg"},
  {0, "bhs", one (0062000), one (0177400), "Bg"},
  {0, "bls", one (0061400), one (0177400), "Bg"},
  {0, "bcc", one (0062000), one (0177400), "Bg"},
  {0, "bcs", one (0062400), one (0177400), "Bg"},
  {0, "blo", one (0062400), one (0177400), "Bg"},
  {0, "bne", one (0063000), one (0177400), "Bg"},
  {0, "beq", one (0063400), one (0177400), "Bg"},
  {0, "bvc", one (0064000), one (0177400), "Bg"},
  {0, "bvs", one (0064400), one (0177400), "Bg"},
  {0, "bpl", one (0065000), one (0177400), "Bg"},
  {0, "bmi", one (0065400), one (0177400), "Bg"},
  {0, "bge", one (0066000), one (0177400), "Bg"},
  {0, "blt", one (0066400), one (0177400), "Bg"},
  {0, "bgt", one (0067000), one (0177400), "Bg"},
  {0, "ble", one (0067400), one (0177400), "Bg"},

  /* short branches */
  {0, "bhi.s", one (0061000), one (0177400), "Bs"},
  {0, "bhs.s", one (0062000), one (0177400), "Bs"},
  {0, "bls.s", one (0061400), one (0177400), "Bs"},
  {0, "bcc.s", one (0062000), one (0177400), "Bs"},
  {0, "bcs.s", one (0062400), one (0177400), "Bs"},
  {0, "blo.s", one (0062400), one (0177400), "Bs"},
  {0, "bne.s", one (0063000), one (0177400), "Bs"},
  {0, "beq.s", one (0063400), one (0177400), "Bs"},
  {0, "bvc.s", one (0064000), one (0177400), "Bs"},
  {0, "bvs.s", one (0064400), one (0177400), "Bs"},
  {0, "bpl.s", one (0065000), one (0177400), "Bs"},
  {0, "bmi.s", one (0065400), one (0177400), "Bs"},
  {0, "bge.s", one (0066000), one (0177400), "Bs"},
  {0, "blt.s", one (0066400), one (0177400), "Bs"},
  {0, "bgt.s", one (0067000), one (0177400), "Bs"},
  {0, "ble.s", one (0067400), one (0177400), "Bs"},

  /* long (word displacement) branches */
  {0, "bhi.l", one (0061000), one (0177400), "Bw"},
  {0, "bhs.l", one (0062000), one (0177400), "Bw"},
  {0, "bls.l", one (0061400), one (0177400), "Bw"},
  {0, "bcc.l", one (0062000), one (0177400), "Bw"},
  {0, "bcs.l", one (0062400), one (0177400), "Bw"},
  {0, "blo.l", one (0062400), one (0177400), "Bw"},
  {0, "bne.l", one (0063000), one (0177400), "Bw"},
  {0, "beq.l", one (0063400), one (0177400), "Bw"},
  {0, "bvc.l", one (0064000), one (0177400), "Bw"},
  {0, "bvs.l", one (0064400), one (0177400), "Bw"},
  {0, "bpl.l", one (0065000), one (0177400), "Bw"},
  {0, "bmi.l", one (0065400), one (0177400), "Bw"},
  {0, "bge.l", one (0066000), one (0177400), "Bw"},
  {0, "blt.l", one (0066400), one (0177400), "Bw"},
  {0, "bgt.l", one (0067000), one (0177400), "Bw"},
  {0, "ble.l", one (0067400), one (0177400), "Bw"},

  {0, "bchg", one (0000500), one (0170700), "Dd$s"},
  {0, "bchg", one (0004100), one (0177700), "#b$s"},
  {0, "bclr", one (0000600), one (0170700), "Dd$s"},
  {0, "bclr", one (0004200), one (0177700), "#b$s"},
  {O_NEMU|O_M68020, "bfchg", one (0165300), one (0177700), "?sO2O3"},
  {O_NEMU|O_M68020, "bfclr", one (0166300), one (0177700), "?sO2O3"},
  {O_NEMU|O_M68020, "bfexts", one (0165700), one (0177700), "/sO2O3D1"},
  {O_NEMU|O_M68020, "bfextu", one (0164700), one (0177700), "/sO2O3D1"},
  {O_NEMU|O_M68020, "bfffo", one (0166700), one (0177700), "/sO2O3D1"},
  {O_NEMU|O_M68020, "bfins", one (0167700), one (0177700), "D1?sO2O3"},
  {O_NEMU|O_M68020, "bfset", one (0167300), one (0177700), "?sO2O3"},
  {O_NEMU|O_M68020, "bftst", one (0164300), one (0177700), "/sO2O3"},
  {0, "bset", one (0000700), one (0170700), "Dd$s"},
  {0, "bset", one (0004300), one (0177700), "#b$s"},
  {0, "btst", one (0000400), one (0170700), "Dd@s"},
  {0, "btst", one (0004000), one (0177700), "#b@s"},

  {O_NEMU|O_M68010, "bkpt", one (0044110), one (0177770), "Qs"},
  {0, "bra", one (0060000), one (0177400), "Bg"},
  {O_WARN, "bsr", one (0060400), one (0177400), "Bg"},
  {0, "bra.s", one (0060000), one (0177400), "Bs"},
  {O_WARN, "bsr.s", one (0060400), one (0177400), "Bs"},
  {0, "bra.l", one (0060000), one (0177400), "Bw"},
  {O_WARN, "bsr.l", one (0060400), one (0177400), "Bw"},
/* Ken wants this instruction to be a Macro really
  {O_NEMU|O_M68020, "callm", one (0003300), one (0177700), "#b!s"},
*/
  {O_NEMU|O_M68020, "cas2.l", one (0007374), one (0177777), "D3D6D2D5R1R4"},
  {O_NEMU|O_M68020, "cas2.w", one (0006374), one (0177777), "D3D6D2D5R1R4"},
  {O_NEMU|O_M68020, "cas.b", one (0005300), one (0177700), "D3D2~s"},
  {O_NEMU|O_M68020, "cas.l", one (0007300), one (0177700), "D3D2~s"},
  {O_NEMU|O_M68020, "cas.w", one (0006300), one (0177700), "D3D2~s"},

/*  {0, "chk", one (0040600), one (0170700), ";wDd"}, FOO this looks wrong */
  {O_NEMU|O_M68020, "chk2.b", two (0000300, 0004000), two (0177700, 07777), "!sR1"},
  {O_NEMU|O_M68020, "chk2.l", two (0002300, 0004000), two (0177700, 07777), "!sR1"},
  {O_NEMU|O_M68020, "chk2.w", two (0001300, 0004000), two (0177700, 07777), "!sR1"},
  {O_NEMU|O_M68020, "chk.l", one (0040400), one (0170700), ";lDd"},
  {O_NEMU|O_NEMU, "chk.w", one (0040600), one (0170700), ";wDd"},
  {0, "clr.b", one (0041000), one (0177700), "$s"},
  {0, "clr.l", one (0041200), one (0177700), "$s"},
  {0, "clr.w", one (0041100), one (0177700), "$s"},

  {O_NEMU|O_M68020, "cmp2.b", two (0000300, 0), two (0177700, 07777), "!sR1"},
  {O_NEMU|O_M68020, "cmp2.l", two (0002300, 0), two (0177700, 07777), "!sR1"},
  {O_NEMU|O_M68020, "cmp2.w", two (0001300, 0), two (0177700, 07777), "!sR1"},
  {0, "cmpa.l", one (0130700), one (0170700), "*lAd"},
  {0, "cmpa.w", one (0130300), one (0170700), "*wAd"},
  {0, "cmpi.b", one (0006000), one (0177700), "#b;b"},
  {0, "cmpi.l", one (0006200), one (0177700), "#l;l"},
  {0, "cmpi.w", one (0006100), one (0177700), "#w;w"},
  {0, "cmp.b", one (0130000), one (0170700), ";bDd"},
  {0, "cmp.b", one (0006000), one (0177700), "#b;b"}, /* cmpi written as cmp */
  {O_WARN, "cmp", one (0006100), one (0177700), "#w;w"},
  {O_WARN, "cmp", one (0130100), one (0170700), "*wDd"},
  {O_WARN, "cmp", one (0130300), one (0170700), "*wAd"}, /* cmpa written as cmp */
  {0, "cmp.w", one (0130100), one (0170700), "*wDd"},
  {0, "cmp.w", one (0006100), one (0177700), "#w;w"},
  {0, "cmp.w", one (0130300), one (0170700), "*wAd"}, /* cmpa written as cmp */
  {0, "cmp.l", one (0130200), one (0170700), "*lDd"},
  {0, "cmp.l", one (0006200), one (0177700), "#l;l"},
  {0, "cmp.l", one (0130700), one (0170700), "*lAd"},
  {O_NEMU, "cmpm.b", one (0130410), one (0170770), "+s+d"},
  {O_NEMU, "cmpm.l", one (0130610), one (0170770), "+s+d"},
  {O_NEMU, "cmpm.w", one (0130510), one (0170770), "+s+d"},

  {0, "dbcc", one (0052310), one (0177770), "DsBw"},
  {0, "dbcs", one (0052710), one (0177770), "DsBw"},
  {0, "dblo", one (0052710), one (0177770), "DsBw"},
  {0, "dbeq", one (0053710), one (0177770), "DsBw"},
  {0, "dbf", one (0050710), one (0177770), "DsBw"},
  {0, "dbge", one (0056310), one (0177770), "DsBw"},
  {0, "dbgt", one (0057310), one (0177770), "DsBw"},
  {0, "dbhi", one (0051310), one (0177770), "DsBw"},
  {0, "dble", one (0057710), one (0177770), "DsBw"},
  {0, "dbls", one (0051710), one (0177770), "DsBw"},
  {0, "dblt", one (0056710), one (0177770), "DsBw"},
  {0, "dbmi", one (0055710), one (0177770), "DsBw"},
  {0, "dbne", one (0053310), one (0177770), "DsBw"},
  {0, "dbpl", one (0055310), one (0177770), "DsBw"},
  {0, "dbra", one (0050710), one (0177770), "DsBw"},
  {0, "dbt", one (0050310), one (0177770), "DsBw"},
  {0, "dbvc", one (0054310), one (0177770), "DsBw"},
  {0, "dbvs", one (0054710), one (0177770), "DsBw"},

  {0, "divs.l", two (0046100, 0004000), two (0177700, 0107770), ";lD3D1"},
  {0, "divs.l", two (0046100, 0004000), two (0177700, 0107770), ";lDD"},
  {0, "divsl.l", two (0046100, 0006000), two (0177700, 0107770), ";lD3D1"},
  {0, "divs.w", one (0100700), one (0170700), ";wDd"},
  {0, "divs", one (0100700), one (0170700), ";wDd"},
  {0, "divu.l", two (0046100, 0000000), two (0177700, 0107770), ";lD3D1"},
  {0, "divu.l", two (0046100, 0000000), two (0177700, 0107770), ";lDD"},
  {0, "divul.l", two (0046100, 0002000), two (0177700, 0107770), ";lD3D1"},
  {0, "divu.w", one (0100300), one (0170700), ";wDd"},
  {0, "divu", one (0100300), one (0170700), ";wDd"},
  {0, "eor", one (0005074), one (0177777), "#bCs"}, /* eori to ccr */
  {0, "eor", one (0005174), one (0177777), "#wSs"}, /* eori to sr */
  {0, "eori", one (0005074), one (0177777), "#bCs"}, /* eori to ccr */
  {0, "eori", one (0005174), one (0177777), "#wSs"}, /* eori to sr */
  {0, "eor.b", one (0005000), one (0177700), "#b$s"}, /* eori written as or */
  {0, "eor.b", one (0130400), one (0170700), "Dd$s"}, /* register to memory */
  {0, "eori.b", one (0005000), one (0177700), "#b$s"},
  {0, "eori.l", one (0005200), one (0177700), "#l$s"},
  {0, "eori.w", one (0005100), one (0177700), "#w$s"},
  {0, "eor.l", one (0005200), one (0177700), "#l$s"},
  {0, "eor.l", one (0130600), one (0170700), "Dd$s"},
  {0, "eor.w", one (0005100), one (0177700), "#w$s"},
  {0, "eor.w", one (0130500), one (0170700), "Dd$s"},

  {0, "exg", one (0140500), one (0170770), "DdDs"},
  {0, "exg", one (0140510), one (0170770), "AdAs"},
  {0, "exg", one (0140610), one (0170770), "DdAs"},
  {0, "exg", one (0140610), one (0170770), "AsDd"},

  {0, "ext.w", one (0044200), one (0177770), "Ds"},
  {0, "ext.l", one (0044300), one (0177770), "Ds"},
  {O_NEMU|O_M68020, "extb.l", one (0044700), one (0177770), "Ds"},

#ifdef FP_INSTRUCTIONS
  {O_FP, "fabs.b", two (O_FPxF000, 0x5818), two (0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fabsd", two (0xF000, 0x5418), two (0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fabs.l", two( 0xF000, 0x4018), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fabsp", two (0xF000, 0x4C18), two (0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fabss", two (0xF000, 0x4418), two (0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fabs.w", two (0xF000, 0x5018), two (0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fabsx", two (0xF000, 0x0018), two (0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fabsx", two (0xF000, 0x0018), two (0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fabsx", two (0xF000, 0x4818), two (0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "facos.b", two (0xF000, 0x581C), two (0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "facosd", two (0xF000, 0x541C), two (0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "facos.l", two (0xF000, 0x401C), two (0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "facosp", two (0xF000, 0x4C1C), two (0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "facoss", two (0xF000, 0x441C), two (0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "facos.w", two (0xF000, 0x501C), two (0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "facosx", two (0xF000, 0x001C), two (0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "facosx", two (0xF000, 0x001C), two (0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "facosx", two (0xF000, 0x481C), two (0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fadd.b", two (0xF000, 0x5822), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "faddd", two (0xF000, 0x5422), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fadd.l", two (0xF000, 0x4022), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "faddp", two (0xF000, 0x4C22), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fadds", two (0xF000, 0x4422), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fadd.w", two (0xF000, 0x5022), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "faddx", two (0xF000, 0x0022), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "faddx", two (0xF000, 0x0022), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "faddx", two (0xF000, 0x4822), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fasin.b", two (0xF000, 0x580C), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fasind", two (0xF000, 0x540C), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fasin.l", two (0xF000, 0x400C), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fasinp", two (0xF000, 0x4C0C), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fasins", two (0xF000, 0x440C), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fasin.w", two (0xF000, 0x500C), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fasinx", two (0xF000, 0x000C), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fasinx", two (0xF000, 0x000C), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fasinx", two (0xF000, 0x480C), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fatan.b", two (0xF000, 0x580A), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fatand", two (0xF000, 0x540A), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fatan.l", two (0xF000, 0x400A), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fatanp", two (0xF000, 0x4C0A), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fatans", two (0xF000, 0x440A), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fatan.w", two (0xF000, 0x500A), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fatanx", two (0xF000, 0x000A), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fatanx", two (0xF000, 0x000A), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fatanx", two (0xF000, 0x480A), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fatanh.b", two (0xF000, 0x580D), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fatanhd", two (0xF000, 0x540D), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fatanh.l", two (0xF000, 0x400D), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fatanhp", two (0xF000, 0x4C0D), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fatanhs", two (0xF000, 0x440D), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fatanh.w", two (0xF000, 0x500D), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fatanhx", two (0xF000, 0x000D), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fatanhx", two (0xF000, 0x000D), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fatanhx", two (0xF000, 0x480D), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fbeq", one (0xF081), one (0xF1FF), "IdBc"},
  {O_FP, "fbf", one (0xF080), one (0xF1FF), "IdBc"},
  {O_FP, "fbge", one (0xF093), one (0xF1FF), "IdBc"},
  {O_FP, "fbgl", one (0xF096), one (0xF1FF), "IdBc"},
  {O_FP, "fbgle", one (0xF097), one (0xF1FF), "IdBc"},
  {O_FP, "fbgt", one (0xF092), one (0xF1FF), "IdBc"},
  {O_FP, "fble", one (0xF095), one (0xF1FF), "IdBc"},
  {O_FP, "fblt", one (0xF094), one (0xF1FF), "IdBc"},
  {O_FP, "fbne", one (0xF08E), one (0xF1FF), "IdBc"},
  {O_FP, "fbnge", one (0xF09C), one (0xF1FF), "IdBc"},
  {O_FP, "fbngl", one (0xF099), one (0xF1FF), "IdBc"},
  {O_FP, "fbngle", one (0xF098), one (0xF1FF), "IdBc"},
  {O_FP, "fbngt", one (0xF09D), one (0xF1FF), "IdBc"},
  {O_FP, "fbnle", one (0xF09A), one (0xF1FF), "IdBc"},
  {O_FP, "fbnlt", one (0xF09B), one (0xF1FF), "IdBc"},
  {O_FP, "fboge", one (0xF083), one (0xF1FF), "IdBc"},
  {O_FP, "fbogl", one (0xF086), one (0xF1FF), "IdBc"},
  {O_FP, "fbogt", one (0xF082), one (0xF1FF), "IdBc"},
  {O_FP, "fbole", one (0xF085), one (0xF1FF), "IdBc"},
  {O_FP, "fbolt", one (0xF084), one (0xF1FF), "IdBc"},
  {O_FP, "fbor", one (0xF087), one (0xF1FF), "IdBc"},
  {O_FP, "fbseq", one (0xF091), one (0xF1FF), "IdBc"},
  {O_FP, "fbsf", one (0xF090), one (0xF1FF), "IdBc"},
  {O_FP, "fbsne", one (0xF09E), one (0xF1FF), "IdBc"},
  {O_FP, "fbst", one (0xF09F), one (0xF1FF), "IdBc"},
  {O_FP, "fbt", one (0xF08F), one (0xF1FF), "IdBc"},
  {O_FP, "fbueq", one (0xF089), one (0xF1FF), "IdBc"},
  {O_FP, "fbuge", one (0xF08B), one (0xF1FF), "IdBc"},
  {O_FP, "fbugt", one (0xF08A), one (0xF1FF), "IdBc"},
  {O_FP, "fbule", one (0xF08D), one (0xF1FF), "IdBc"},
  {O_FP, "fbult", one (0xF08C), one (0xF1FF), "IdBc"},
  {O_FP, "fbun", one (0xF088), one (0xF1FF), "IdBc"},

  {O_FP, "fcmp.b", two (0xF000, 0x5838), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fcmpd", two (0xF000, 0x5438), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fcmp.l", two (0xF000, 0x4038), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fcmpp", two (0xF000, 0x4C38), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fcmps", two (0xF000, 0x4438), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fcmp.w", two (0xF000, 0x5038), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fcmpx", two (0xF000, 0x0038), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fcmpx", two (0xF000, 0x0038), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fcmpx", two (0xF000, 0x4838), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fcos.b", two (0xF000, 0x581D), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fcosd", two (0xF000, 0x541D), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fcos.l", two (0xF000, 0x401D), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fcosp", two (0xF000, 0x4C1D), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fcoss", two (0xF000, 0x441D), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fcos.w", two (0xF000, 0x501D), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fcosx", two (0xF000, 0x001D), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fcosx", two (0xF000, 0x001D), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fcosx", two (0xF000, 0x481D), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fcosh.b", two (0xF000, 0x5819), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fcoshd", two (0xF000, 0x5419), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fcosh.l", two (0xF000, 0x4019), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fcoshp", two (0xF000, 0x4C19), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fcoshs", two (0xF000, 0x4419), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fcosh.w", two (0xF000, 0x5019), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fcoshx", two (0xF000, 0x0019), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fcoshx", two (0xF000, 0x0019), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fcoshx", two (0xF000, 0x4819), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fdbeq", two (0xF048, 0x0001), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbf", two (0xF048, 0x0000), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbge", two (0xF048, 0x0013), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbgl", two (0xF048, 0x0016), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbgle", two (0xF048, 0x0017), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbgt", two (0xF048, 0x0012), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdble", two (0xF048, 0x0015), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdblt", two (0xF048, 0x0014), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbne", two (0xF048, 0x000E), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbnge", two (0xF048, 0x001C), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbngl", two (0xF048, 0x0019), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbngle", two (0xF048, 0x0018), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbngt", two (0xF048, 0x001D), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbnle", two (0xF048, 0x001A), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbnlt", two (0xF048, 0x001B), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdboge", two (0xF048, 0x0003), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbogl", two (0xF048, 0x0006), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbogt", two (0xF048, 0x0002), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbole", two (0xF048, 0x0005), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbolt", two (0xF048, 0x0004), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbor", two (0xF048, 0x0007), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbseq", two (0xF048, 0x0011), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbsf", two (0xF048, 0x0010), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbsne", two (0xF048, 0x001E), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbst", two (0xF048, 0x001F), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbt", two (0xF048, 0x000F), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbueq", two (0xF048, 0x0009), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbuge", two (0xF048, 0x000B), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbugt", two (0xF048, 0x000A), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbule", two (0xF048, 0x000D), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbult", two (0xF048, 0x000C), two (0xF1F8, 0xFFFF), "IiDsBw"},
  {O_FP, "fdbun", two (0xF048, 0x0008), two (0xF1F8, 0xFFFF), "IiDsBw"},

  {O_FP, "fdiv.b", two (0xF000, 0x5820), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fdivd", two (0xF000, 0x5420), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fdiv.l", two (0xF000, 0x4020), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fdivp", two (0xF000, 0x4C20), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fdivs", two (0xF000, 0x4420), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fdiv.w", two (0xF000, 0x5020), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fdivx", two (0xF000, 0x0020), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fdivx", two (0xF000, 0x0020), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fdivx", two (0xF000, 0x4820), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fetox.b", two (0xF000, 0x5810), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fetoxd", two (0xF000, 0x5410), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fetox.l", two (0xF000, 0x4010), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fetoxp", two (0xF000, 0x4C10), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fetoxs", two (0xF000, 0x4410), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fetox.w", two (0xF000, 0x5010), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fetoxx", two (0xF000, 0x0010), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fetoxx", two (0xF000, 0x0010), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fetoxx", two (0xF000, 0x4810), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fetoxm1.b", two (0xF000, 0x5808), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fetoxm1d", two (0xF000, 0x5408), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fetoxm1.l", two (0xF000, 0x4008), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fetoxm1p", two (0xF000, 0x4C08), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fetoxm1s", two (0xF000, 0x4408), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fetoxm1.w", two (0xF000, 0x5008), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fetoxm1x", two (0xF000, 0x0008), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fetoxm1x", two (0xF000, 0x0008), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fetoxm1x", two (0xF000, 0x4808), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fgetexp.b", two (0xF000, 0x581E), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fgetexpd", two (0xF000, 0x541E), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fgetexp.l", two (0xF000, 0x401E), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fgetexpp", two (0xF000, 0x4C1E), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fgetexps", two (0xF000, 0x441E), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fgetexp.w", two (0xF000, 0x501E), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fgetexpx", two (0xF000, 0x001E), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fgetexpx", two (0xF000, 0x001E), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fgetexpx", two (0xF000, 0x481E), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fgetman.b", two (0xF000, 0x581F), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fgetmand", two (0xF000, 0x541F), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fgetman.l", two (0xF000, 0x401F), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fgetmanp", two (0xF000, 0x4C1F), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fgetmans", two (0xF000, 0x441F), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fgetman.w", two (0xF000, 0x501F), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fgetmanx", two (0xF000, 0x001F), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fgetmanx", two (0xF000, 0x001F), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fgetmanx", two (0xF000, 0x481F), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fint.b", two (0xF000, 0x5801), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fintd", two (0xF000, 0x5401), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fint.l", two (0xF000, 0x4001), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fintp", two (0xF000, 0x4C01), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fints", two (0xF000, 0x4401), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fint.w", two (0xF000, 0x5001), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fintx", two (0xF000, 0x0001), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fintx", two (0xF000, 0x0001), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fintx", two (0xF000, 0x4801), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fintrz.b", two (0xF000, 0x5803), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fintrzd", two (0xF000, 0x5403), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fintrz.l", two (0xF000, 0x4003), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fintrzp", two (0xF000, 0x4C03), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fintrzs", two (0xF000, 0x4403), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fintrz.w", two (0xF000, 0x5003), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fintrzx", two (0xF000, 0x0003), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fintrzx", two (0xF000, 0x0003), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fintrzx", two (0xF000, 0x4803), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "flog10.b", two (0xF000, 0x5815), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "flog10d", two (0xF000, 0x5415), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "flog10.l", two (0xF000, 0x4015), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "flog10p", two (0xF000, 0x4C15), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "flog10s", two (0xF000, 0x4415), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "flog10.w", two (0xF000, 0x5015), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "flog10x", two (0xF000, 0x0015), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "flog10x", two (0xF000, 0x0015), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "flog10x", two (0xF000, 0x4815), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "flog2.b", two (0xF000, 0x5816), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "flog2d", two (0xF000, 0x5416), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "flog2.l", two (0xF000, 0x4016), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "flog2p", two (0xF000, 0x4C16), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "flog2s", two (0xF000, 0x4416), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "flog2.w", two (0xF000, 0x5016), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "flog2x", two (0xF000, 0x0016), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "flog2x", two (0xF000, 0x0016), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "flog2x", two (0xF000, 0x4816), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "flogn.b", two (0xF000, 0x5814), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "flognd", two (0xF000, 0x5414), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "flogn.l", two (0xF000, 0x4014), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "flognp", two (0xF000, 0x4C14), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "flogns", two (0xF000, 0x4414), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "flogn.w", two (0xF000, 0x5014), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "flognx", two (0xF000, 0x0014), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "flognx", two (0xF000, 0x0014), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "flognx", two (0xF000, 0x4814), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "flognp1.b", two (0xF000, 0x5806), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "flognp1d", two (0xF000, 0x5406), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "flognp1.l", two (0xF000, 0x4006), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "flognp1p", two (0xF000, 0x4C06), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "flognp1s", two (0xF000, 0x4406), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "flognp1.w", two (0xF000, 0x5006), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "flognp1x", two (0xF000, 0x0006), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "flognp1x", two (0xF000, 0x0006), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "flognp1x", two (0xF000, 0x4806), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fmod.b", two (0xF000, 0x5821), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fmodd", two (0xF000, 0x5421), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fmod.l", two (0xF000, 0x4021), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fmodp", two (0xF000, 0x4C21), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fmods", two (0xF000, 0x4421), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fmod.w", two (0xF000, 0x5021), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fmodx", two (0xF000, 0x0021), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fmodx", two (0xF000, 0x0021), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fmodx", two (0xF000, 0x4821), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fmove.b", two (0xF000, 0x5800), two (0xF1C0, 0xFC7F), "Ii;bF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmove.b", two (0xF000, 0x7800), two (0xF1C0, 0xFC7F), "IiF7@b"},		/* fmove from fp<n> to <ea> */
  {O_FP, "fmoved", two (0xF000, 0x5400), two (0xF1C0, 0xFC7F), "Ii;FF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmoved", two (0xF000, 0x7400), two (0xF1C0, 0xFC7F), "IiF7@F"},		/* fmove from fp<n> to <ea> */
  {O_FP, "fmove.l", two (0xF000, 0x4000), two (0xF1C0, 0xFC7F), "Ii;lF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmove.l", two (0xF000, 0x6000), two (0xF1C0, 0xFC7F), "IiF7@l"},		/* fmove from fp<n> to <ea> */
  {O_FP, "fmovep", two (0xF000, 0x4C00), two (0xF1C0, 0xFC7F), "Ii;pF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmovep", two (0xF000, 0x6C00), two (0xF1C0, 0xFC00), "IiF7@pkC"},		/* fmove.p with k-factors: */
  {O_FP, "fmovep", two (0xF000, 0x7C00), two (0xF1C0, 0xFC0F), "IiF7@pkk"},		/* fmove.p with k-factors: */
  {O_FP, "fmoves", two (0xF000, 0x4400), two (0xF1C0, 0xFC7F), "Ii;fF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmoves", two (0xF000, 0x6400), two (0xF1C0, 0xFC7F), "IiF7@f"},		/* fmove from fp<n> to <ea> */
  {O_FP, "fmove.w", two (0xF000, 0x5000), two (0xF1C0, 0xFC7F), "Ii;wF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmove.w", two (0xF000, 0x7000), two (0xF1C0, 0xFC7F), "IiF7@w"},		/* fmove from fp<n> to <ea> */
  {O_FP, "fmovex", two (0xF000, 0x0000), two (0xF1C0, 0xE07F), "IiF8F7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmovex", two (0xF000, 0x0000), two (0xF1C0, 0xE07F), "IiFt"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmovex", two (0xF000, 0x4800), two (0xF1C0, 0xFC7F), "Ii;xF7"},		/* fmove from <ea> to fp<n> */
  {O_FP, "fmovex", two (0xF000, 0x6800), two (0xF1C0, 0xFC7F), "IiF7@x"},		/* fmove from fp<n> to <ea> */

		/* fmove.l from/to system control registers: */

/* fmove.l and fmovem.l are the same instruction.   fmovem.l makes sense in
   more cases, so I've dumped fmove.l pro tem, but this is the wrong
   way to solve the problem in the long run.   Hmmm. */
/*  {O_FP, "fmove.l", two (0xF000, 0xA000), two (0xF1C0, 0xE3FF), "Iis8@s"}, */
/*  {O_FP, "fmove.l", two (0xF000, 0x8000), two (0xF1C0, 0xE3FF), "Ii@ss8"}, */

  {O_FP, "fmovecrx", two (0xF000, 0x5C00), two (0xF1FF, 0xFC00), "Ii#CF7"},		/* fmovecr.x #ccc, FPn */

  {O_FP, "fmovemx", two (0xF020, 0xE000), two (0xF1F8, 0xFF00), "Id#b-s"},		/* fmovem.x to autodecrement, static and dynamic */
  {O_FP, "fmovemx", two (0xF020, 0xE800), two (0xF1F8, 0xFF8F), "IiDk-s"},		/* fmovem.x to autodecrement, static and dynamic */

  {O_FP, "fmovemx", two (0xF000, 0xF000), two (0xF1C0, 0xFF00), "Id#b&s"},		/* fmovem.x to control, static and dynamic: */
  {O_FP, "fmovemx", two (0xF000, 0xF800), two (0xF1C0, 0xFF8F), "IiDk&s"},		/* fmovem.x to control, static and dynamic: */

  {O_FP, "fmovemx", two (0xF018, 0xD000), two (0xF1F8, 0xFF00), "Id#b+s"},		/* fmovem.x from autoincrement, static and dynamic: */
  {O_FP, "fmovemx", two (0xF018, 0xD800), two (0xF1F8, 0xFF8F), "IiDk+s"},		/* fmovem.x from autoincrement, static and dynamic: */
  
  {O_FP, "fmovemx", two (0xF000, 0xD000), two (0xF1C0, 0xFF00), "Id#b&s"},		/* fmovem.x from control, static and dynamic: */
  {O_FP, "fmovemx", two (0xF000, 0xD800), two (0xF1C0, 0xFF8F), "IiDk&s"},		/* fmovem.x from control, static and dynamic: */

/* fmoveml and fmovel are the same instruction.   This may cause some
   confusion in the assembler. */

  {O_FP, "fmovem.l", two (0xF000, 0xA000), two (0xF1C0, 0xE3FF), "Ii#8%s"},		/* fmovem.l to/from system control register(s): */
  {O_FP, "fmovem.l", two (0xF000, 0x8000), two (0xF1C0, 0xE3FF), "Ii%s#8"},		/* fmovem.l to/from system control register(s): */

  {O_FP, "fmul.b", two (0xF000, 0x5823), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fmuld", two (0xF000, 0x5423), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fmul.l", two (0xF000, 0x4023), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fmulp", two (0xF000, 0x4C23), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fmuls", two (0xF000, 0x4423), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fmul.w", two (0xF000, 0x5023), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fmulx", two (0xF000, 0x0023), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fmulx", two (0xF000, 0x0023), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fmulx", two (0xF000, 0x4823), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fneg.b", two (0xF000, 0x581A), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fnegd", two (0xF000, 0x541A), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fneg.l", two (0xF000, 0x401A), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fnegp", two (0xF000, 0x4C1A), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fnegs", two (0xF000, 0x441A), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fneg.w", two (0xF000, 0x501A), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fnegx", two (0xF000, 0x001A), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fnegx", two (0xF000, 0x001A), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fnegx", two (0xF000, 0x481A), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "frem.b", two (0xF000, 0x5825), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fremd", two (0xF000, 0x5425), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "frem.l", two (0xF000, 0x4025), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fremp", two (0xF000, 0x4C25), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "frems", two (0xF000, 0x4425), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "frem.w", two (0xF000, 0x5025), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fremx", two (0xF000, 0x0025), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fremx", two (0xF000, 0x0025), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fremx", two (0xF000, 0x4825), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

/* Need special mode for autoincrement/decrement? */
  {O_FP, "frestore", one (0xF140), one (0xF1C0), "Id&s"},
  {O_FP, "fsave", one (0xF100), one( 0xF1C0), "Id&s"},

  {O_FP, "fsincos.b", two (0xF000, 0x5830), two( 0xF1C0, 0xFC78), "Ii;bF7FC"},
  {O_FP, "fsincosd", two (0xF000, 0x5430), two( 0xF1C0, 0xFC78), "Ii;FF7FC"},
  {O_FP, "fsincos.l", two (0xF000, 0x4030), two( 0xF1C0, 0xFC78), "Ii;lF7FC"},
  {O_FP, "fsincosp", two (0xF000, 0x4C30), two( 0xF1C0, 0xFC78), "Ii;pF7FC"},
  {O_FP, "fsincoss", two (0xF000, 0x4430), two( 0xF1C0, 0xFC78), "Ii;fF7FC"},
  {O_FP, "fsincos.w", two (0xF000, 0x5030), two( 0xF1C0, 0xFC78), "Ii;wF7FC"},
  {O_FP, "fsincosx", two (0xF000, 0x0030), two( 0xF1C0, 0xE078), "IiF8F7FC"},
  {O_FP, "fsincosx", two (0xF000, 0x4830), two( 0xF1C0, 0xFC78), "Ii;xF7FC"},

  {O_FP, "fscale.b", two (0xF000, 0x5826), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fscaled", two (0xF000, 0x5426), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fscale.l", two (0xF000, 0x4026), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fscalep", two (0xF000, 0x4C26), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fscales", two (0xF000, 0x4426), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fscale.w", two (0xF000, 0x5026), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fscalex", two (0xF000, 0x0026), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fscalex", two (0xF000, 0x0026), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fscalex", two (0xF000, 0x4826), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fseq", two (0xF040, 0x0001), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsf", two (0xF040, 0x0000), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsge", two (0xF040, 0x0013), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsgl", two (0xF040, 0x0016), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsgle", two (0xF040, 0x0017), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsgt", two (0xF040, 0x0012), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsle", two (0xF040, 0x0015), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fslt", two (0xF040, 0x0014), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsne", two (0xF040, 0x000E), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsnge", two (0xF040, 0x001C), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsngl", two (0xF040, 0x0019), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsngle", two (0xF040, 0x0018), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsngt", two (0xF040, 0x001D), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsnle", two (0xF040, 0x001A), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsnlt", two (0xF040, 0x001B), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsoge", two (0xF040, 0x0003), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsogl", two (0xF040, 0x0006), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsogt", two (0xF040, 0x0002), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsole", two (0xF040, 0x0005), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsolt", two (0xF040, 0x0004), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsor", two (0xF040, 0x0007), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsseq", two (0xF040, 0x0011), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fssf", two (0xF040, 0x0010), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fssne", two (0xF040, 0x001E), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsst", two (0xF040, 0x001F), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fst", two (0xF040, 0x000F), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsueq", two (0xF040, 0x0009), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsuge", two (0xF040, 0x000B), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsugt", two (0xF040, 0x000A), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsule", two (0xF040, 0x000D), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsult", two (0xF040, 0x000C), two (0xF1C0, 0xFFFF), "Ii@s"},
  {O_FP, "fsun", two (0xF040, 0x0008), two (0xF1C0, 0xFFFF), "Ii@s"},

  {O_FP, "fsgldiv.b", two (0xF000, 0x5824), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fsgldivd", two (0xF000, 0x5424), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fsgldiv.l", two (0xF000, 0x4024), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fsgldivp", two (0xF000, 0x4C24), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fsgldivs", two (0xF000, 0x4424), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fsgldiv.w", two (0xF000, 0x5024), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fsgldivx", two (0xF000, 0x0024), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fsgldivx", two (0xF000, 0x0024), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fsgldivx", two (0xF000, 0x4824), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fsglmul.b", two (0xF000, 0x5827), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fsglmuld", two (0xF000, 0x5427), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fsglmul.l", two (0xF000, 0x4027), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fsglmulp", two (0xF000, 0x4C27), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fsglmuls", two (0xF000, 0x4427), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fsglmul.w", two (0xF000, 0x5027), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fsglmulx", two (0xF000, 0x0027), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fsglmulx", two (0xF000, 0x0027), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fsglmulx", two (0xF000, 0x4827), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fsin.b", two (0xF000, 0x580E), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fsind", two (0xF000, 0x540E), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fsin.l", two (0xF000, 0x400E), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fsinp", two (0xF000, 0x4C0E), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fsins", two (0xF000, 0x440E), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fsin.w", two (0xF000, 0x500E), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fsinx", two (0xF000, 0x000E), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fsinx", two (0xF000, 0x000E), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fsinx", two (0xF000, 0x480E), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fsinh.b", two (0xF000, 0x5802), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fsinhd", two (0xF000, 0x5402), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fsinh.l", two (0xF000, 0x4002), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fsinhp", two (0xF000, 0x4C02), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fsinhs", two (0xF000, 0x4402), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fsinh.w", two (0xF000, 0x5002), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fsinhx", two (0xF000, 0x0002), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fsinhx", two (0xF000, 0x0002), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fsinhx", two (0xF000, 0x4802), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fsqrt.b", two (0xF000, 0x5804), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fsqrtd", two (0xF000, 0x5404), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fsqrt.l", two (0xF000, 0x4004), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fsqrtp", two (0xF000, 0x4C04), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fsqrts", two (0xF000, 0x4404), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fsqrt.w", two (0xF000, 0x5004), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fsqrtx", two (0xF000, 0x0004), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fsqrtx", two (0xF000, 0x0004), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fsqrtx", two (0xF000, 0x4804), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "fsub.b", two (0xF000, 0x5828), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "fsubd", two (0xF000, 0x5428), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "fsub.l", two (0xF000, 0x4028), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "fsubp", two (0xF000, 0x4C28), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "fsubs", two (0xF000, 0x4428), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "fsub.w", two (0xF000, 0x5028), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "fsubx", two (0xF000, 0x0028), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "fsubx", two (0xF000, 0x0028), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "fsubx", two (0xF000, 0x4828), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "ftan.b", two (0xF000, 0x580F), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "ftand", two (0xF000, 0x540F), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "ftan.l", two (0xF000, 0x400F), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "ftanp", two (0xF000, 0x4C0F), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "ftans", two (0xF000, 0x440F), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "ftan.w", two (0xF000, 0x500F), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "ftanx", two (0xF000, 0x000F), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "ftanx", two (0xF000, 0x000F), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "ftanx", two (0xF000, 0x480F), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "ftanh.b", two (0xF000, 0x5809), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "ftanhd", two (0xF000, 0x5409), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "ftanh.l", two (0xF000, 0x4009), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "ftanhp", two (0xF000, 0x4C09), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "ftanhs", two (0xF000, 0x4409), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "ftanh.w", two (0xF000, 0x5009), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "ftanhx", two (0xF000, 0x0009), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "ftanhx", two (0xF000, 0x0009), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "ftanhx", two (0xF000, 0x4809), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "ftentox.b", two (0xF000, 0x5812), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "ftentoxd", two (0xF000, 0x5412), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "ftentox.l", two (0xF000, 0x4012), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "ftentoxp", two (0xF000, 0x4C12), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "ftentoxs", two (0xF000, 0x4412), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "ftentox.w", two (0xF000, 0x5012), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "ftentoxx", two (0xF000, 0x0012), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "ftentoxx", two (0xF000, 0x0012), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "ftentoxx", two (0xF000, 0x4812), two( 0xF1C0, 0xFC7F), "Ii;xF7"},

  {O_FP, "ftrapeq", two (0xF07C, 0x0001), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapf", two (0xF07C, 0x0000), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapge", two (0xF07C, 0x0013), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapgl", two (0xF07C, 0x0016), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapgle", two (0xF07C, 0x0017), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapgt", two (0xF07C, 0x0012), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftraple", two (0xF07C, 0x0015), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftraplt", two (0xF07C, 0x0014), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapne", two (0xF07C, 0x000E), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapnge", two (0xF07C, 0x001C), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapngl", two (0xF07C, 0x0019), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapngle", two (0xF07C, 0x0018), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapngt", two (0xF07C, 0x001D), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapnle", two (0xF07C, 0x001A), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapnlt", two (0xF07C, 0x001B), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapoge", two (0xF07C, 0x0003), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapogl", two (0xF07C, 0x0006), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapogt", two (0xF07C, 0x0002), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapole", two (0xF07C, 0x0005), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapolt", two (0xF07C, 0x0004), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapor", two (0xF07C, 0x0007), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapseq", two (0xF07C, 0x0011), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapsf", two (0xF07C, 0x0010), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapsne", two (0xF07C, 0x001E), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapst", two (0xF07C, 0x001F), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapt", two (0xF07C, 0x000F), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapueq", two (0xF07C, 0x0009), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapuge", two (0xF07C, 0x000B), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapugt", two (0xF07C, 0x000A), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapule", two (0xF07C, 0x000D), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapult", two (0xF07C, 0x000C), two (0xF1FF, 0xFFFF), "Ii"},
  {O_FP, "ftrapun", two (0xF07C, 0x0008), two (0xF1FF, 0xFFFF), "Ii"},

  {O_FP, "ftrapeq.w", two (0xF07A, 0x0001), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapf.w", two (0xF07A, 0x0000), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapge.w", two (0xF07A, 0x0013), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapgl.w", two (0xF07A, 0x0016), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapgle.w", two (0xF07A, 0x0017), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapgt.w", two (0xF07A, 0x0012), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftraple.w", two (0xF07A, 0x0015), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftraplt.w", two (0xF07A, 0x0014), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapne.w", two (0xF07A, 0x000E), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapnge.w", two (0xF07A, 0x001C), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapngl.w", two (0xF07A, 0x0019), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapngle.w", two (0xF07A, 0x0018), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapngt.w", two (0xF07A, 0x001D), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapnle.w", two (0xF07A, 0x001A), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapnlt.w", two (0xF07A, 0x001B), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapoge.w", two (0xF07A, 0x0003), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapogl.w", two (0xF07A, 0x0006), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapogt.w", two (0xF07A, 0x0002), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapole.w", two (0xF07A, 0x0005), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapolt.w", two (0xF07A, 0x0004), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapor.w", two (0xF07A, 0x0007), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapseq.w", two (0xF07A, 0x0011), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapsf.w", two (0xF07A, 0x0010), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapsne.w", two (0xF07A, 0x001E), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapst.w", two (0xF07A, 0x001F), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapt.w", two (0xF07A, 0x000F), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapueq.w", two (0xF07A, 0x0009), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapuge.w", two (0xF07A, 0x000B), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapugt.w", two (0xF07A, 0x000A), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapule.w", two (0xF07A, 0x000D), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapult.w", two (0xF07A, 0x000C), two (0xF1FF, 0xFFFF), "Ii^w"},
  {O_FP, "ftrapun.w", two (0xF07A, 0x0008), two (0xF1FF, 0xFFFF), "Ii^w"},

  {O_FP, "ftrapeq.l", two (0xF07B, 0x0001), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapf.l", two (0xF07B, 0x0000), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapge.l", two (0xF07B, 0x0013), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapgl.l", two (0xF07B, 0x0016), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapgle.l", two (0xF07B, 0x0017), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapgt.l", two (0xF07B, 0x0012), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftraple.l", two (0xF07B, 0x0015), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftraplt.l", two (0xF07B, 0x0014), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapne.l", two (0xF07B, 0x000E), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapnge.l", two (0xF07B, 0x001C), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapngl.l", two (0xF07B, 0x0019), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapngle.l", two (0xF07B, 0x0018), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapngt.l", two (0xF07B, 0x001D), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapnle.l", two (0xF07B, 0x001A), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapnlt.l", two (0xF07B, 0x001B), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapoge.l", two (0xF07B, 0x0003), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapogl.l", two (0xF07B, 0x0006), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapogt.l", two (0xF07B, 0x0002), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapole.l", two (0xF07B, 0x0005), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapolt.l", two (0xF07B, 0x0004), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapor.l", two (0xF07B, 0x0007), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapseq.l", two (0xF07B, 0x0011), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapsf.l", two (0xF07B, 0x0010), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapsne.l", two (0xF07B, 0x001E), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapst.l", two (0xF07B, 0x001F), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapt.l", two (0xF07B, 0x000F), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapueq.l", two (0xF07B, 0x0009), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapuge.l", two (0xF07B, 0x000B), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapugt.l", two (0xF07B, 0x000A), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapule.l", two (0xF07B, 0x000D), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapult.l", two (0xF07B, 0x000C), two (0xF1FF, 0xFFFF), "Ii^l"},
  {O_FP, "ftrapun.l", two (0xF07B, 0x0008), two (0xF1FF, 0xFFFF), "Ii^l"},

  {O_FP, "ftst.b", two (0xF000, 0x583A), two( 0xF1C0, 0xFC7F), "Ii;b"},
  {O_FP, "ftstd", two (0xF000, 0x543A), two( 0xF1C0, 0xFC7F), "Ii;F"},
  {O_FP, "ftst.l", two (0xF000, 0x403A), two( 0xF1C0, 0xFC7F), "Ii;l"},
  {O_FP, "ftstp", two (0xF000, 0x4C3A), two( 0xF1C0, 0xFC7F), "Ii;p"},
  {O_FP, "ftsts", two (0xF000, 0x443A), two( 0xF1C0, 0xFC7F), "Ii;f"},
  {O_FP, "ftst.w", two (0xF000, 0x503A), two( 0xF1C0, 0xFC7F), "Ii;w"},
  {O_FP, "ftstx", two (0xF000, 0x003A), two( 0xF1C0, 0xE07F), "IiF8"},
  {O_FP, "ftstx", two (0xF000, 0x483A), two( 0xF1C0, 0xFC7F), "Ii;x"},

  {O_FP, "ftwotox.b", two (0xF000, 0x5811), two( 0xF1C0, 0xFC7F), "Ii;bF7"},
  {O_FP, "ftwotoxd", two (0xF000, 0x5411), two( 0xF1C0, 0xFC7F), "Ii;FF7"},
  {O_FP, "ftwotox.l", two (0xF000, 0x4011), two( 0xF1C0, 0xFC7F), "Ii;lF7"},
  {O_FP, "ftwotoxp", two (0xF000, 0x4C11), two( 0xF1C0, 0xFC7F), "Ii;pF7"},
  {O_FP, "ftwotoxs", two (0xF000, 0x4411), two( 0xF1C0, 0xFC7F), "Ii;fF7"},
  {O_FP, "ftwotox.w", two (0xF000, 0x5011), two( 0xF1C0, 0xFC7F), "Ii;wF7"},
  {O_FP, "ftwotoxx", two (0xF000, 0x0011), two( 0xF1C0, 0xE07F), "IiF8F7"},
  {O_FP, "ftwotoxx", two (0xF000, 0x0011), two( 0xF1C0, 0xE07F), "IiFt"},
  {O_FP, "ftwotoxx", two (0xF000, 0x4811), two( 0xF1C0, 0xFC7F), "Ii;xF7"},
#endif //FP_INSTRUCTIONS

  {0, "illegal", one (0045374), one (0177777), ""},
  {0, "jmp", one (0047300), one (0177700), "!s"},
  {0, "jsr", one (0047200), one (0177700), "!s"},
  {0, "lea", one (0040700), one (0170700), "!sAd"},
  {O_WARN, "lea.l", one (0040700), one (0170700), "!sAd"},
  {O_NEMU, "link.w", one (0047120), one (0177770), "As#w"},
  {O_NEMU|O_M68020, "link.l", one (0044010), one (0177770), "As#l"},
  {O_NEMU, "link", one (0047120), one (0177770), "As#w"},
  {O_NEMU|O_M68020, "link", one (0044010), one (0177770), "As#l"},

  {0, "lsl.b", one (0160410), one (0170770), "QdDs"}, /* lsrb #Q, Ds */
  {0, "lsl.b", one (0160450), one (0170770), "DdDs"}, /* lsrb Dd, Ds */
  {0, "lsl.w", one (0160510), one (0170770), "QdDs"}, /* lsrb #Q, Ds */
  {0, "lsl.w", one (0160550), one (0170770), "DdDs"}, /* lsrb Dd, Ds */
  {0, "lsl.w", one (0161700), one (0177700), "~s"},	/* Shift memory */
  {0, "lsl.l", one (0160610), one (0170770), "QdDs"}, /* lsrb #Q, Ds */
  {0, "lsl.l", one (0160650), one (0170770), "DdDs"}, /* lsrb Dd, Ds */

  {0, "lsr.b", one (0160010), one (0170770), "QdDs"} /* lsrb #Q, Ds */,
  {0, "lsr.b", one (0160050), one (0170770), "DdDs"}, /* lsrb Dd, Ds */
  {0, "lsr.l", one (0160210), one (0170770), "QdDs"}, /* lsrb #Q, Ds */
  {0, "lsr.l", one (0160250), one (0170770), "DdDs"}, /* lsrb #Q, Ds */
  {0, "lsr.w", one (0160110), one (0170770), "QdDs"}, /* lsrb #Q, Ds */
  {0, "lsr.w", one (0160150), one (0170770), "DdDs"}, /* lsrb #Q, Ds */
  {0, "lsr.w", one (0161300), one (0177700), "~s"},	/* Shift memory */

  {0, "movea.l", one (0020100), one (0170700), "*lAd"},
  {0, "movea.w", one (0030100), one (0170700), "*wAd"},
  {0, "move.b", one (0010000), one (0170000), ";b$d"}, /* move */
  {0, "move.l", one (0020000), one (0170000), "*l$d"},
  {0, "move.l", one (0020100), one (0170700), "*lAd"},
  {O_NEMU, "move.l", one (0047140), one (0177770), "AsUd"}, /* move to USP */
  {O_NEMU, "move.l", one (0047150), one (0177770), "UdAs"}, /* move from USP */

  {O_WARN|O_MOVEM, "movem.l", one (0044300), one (0177700), "#w&s"}, /* movem reg to mem. */
  {O_WARN|O_MOVEM2, "movem.l", one (0044340), one (0177770), "#w-s"}, /* movem reg to autodecrement. */
  {O_WARN|O_MOVEM, "movem.l", one (0046300), one (0177700), "!s#w"}, /* movem mem to reg. */
  {O_WARN|O_MOVEM, "movem.l", one (0046330), one (0177770), "+s#w"}, /* movem autoinc to reg. */

  {O_WARN|O_MOVEM, "movem.w", one (0044200), one (0177700), "#w&s"}, /* movem reg to mem. */
  {O_WARN|O_MOVEM2, "movem.w", one (0044240), one (0177770), "#w-s"}, /* movem reg to autodecrement. */
  {O_WARN|O_MOVEM, "movem.w", one (0046200), one (0177700), "!s#w"}, /* movem mem to reg. */
  {O_WARN|O_MOVEM, "movem.w", one (0046230), one (0177770), "+s#w"}, /* movem autoinc to reg. */

  {O_NEMU, "movep.l", one (0000510), one (0170770), "dsDd"}, /* memory to register */
  {O_NEMU, "movep.l", one (0000710), one (0170770), "Ddds"}, /* register to memory */
  {O_NEMU, "movep.w", one (0000410), one (0170770), "dsDd"}, /* memory to register */
  {O_NEMU, "movep.w", one (0000610), one (0170770), "Ddds"}, /* register to memory */
  {0, "moveq", one (0070000), one (0170400), "MsDd"},
  {0, "move.w", one (0030000), one (0170000), "*w$d"},
  {0, "move.w", one (0030100), one (0170700), "*wAd"}, /* movea, written as move */
  {O_WARN, "move.w", one (0042300), one (0177700), ";wCd"}, /* move to ccr */
  {O_WARN, "move.w", one (0043300), one (0177700), ";wSd"}, /* move to sr */
  {O_NEMU, "move", one (0040300), one (0177700), "Ss$s"}, /* Move from sr */
  {O_NEMU|O_M68010, "move", one (0041300), one (0177700), "Cs$s"}, /* Move from ccr */
  {0, "move", one (0042300), one (0177700), ";wCd"}, /* move to ccr */
  {0, "move", one (0043300), one (0177700), ";wSd"}, /* move to sr */
  {O_WARN, "move", one (0030000), one (0170000), "*w$d"},
  {O_WARN, "move", one (0030100), one (0170700), "*wAd"}, /* movea, written as move */
  {O_M68010, "movec", one (0047172), one (0177777), "c3R1"}, /* movec ctrl,Rn */
  {O_M68010, "movec", one (0047173), one (0177777), "R1c3"}, /* movec Rn,ctrl */
  {O_M68010, "movec", two (0047172, 0x800), two (0177777, 0xfff), "UdR1"}, /* movec USP,Rn */
  {O_M68010, "movec", two (0047173, 0x800), two (0177777, 0xfff), "R1Ud"}, /* movec Rn,USP */
/* movec not done */

  {O_NEMU|O_M68010, "moves.b", two (0007000, 0), two (0177700, 07777), "~sR1"},  /* moves from memory */
  {O_NEMU|O_M68010, "moves.b", two (0007000, 04000), two (0177700, 07777), "R1~s"},  /* moves to memory */
  {O_NEMU|O_M68010, "moves.l", two (0007200, 0), two (0177700, 07777), "~sR1"},  /* moves from memory */
  {O_NEMU|O_M68010, "moves.l", two (0007200, 04000), two (0177700, 07777), "R1~s"},  /* moves to memory */
  {O_NEMU|O_M68010, "moves.w", two (0007100, 0), two (0177700, 07777), "~sR1"},  /* moves from memory */
  {O_NEMU|O_M68010, "moves.w", two (0007100, 04000), two (0177700, 07777), "R1~s"},  /* moves to memory */

  {O_NEMU|O_M68020, "muls.l", two (0046000, 004000), two (0177700, 0107770), ";lD1"},
  {O_NEMU|O_M68020, "muls.l", two (0046000, 006000), two (0177700, 0107770), ";lD3D1"},
  {O_NEMU, "muls.w", one (0140700), one (0170700), ";wDd"},
  {O_NEMU, "muls", one (0140700), one (0170700), ";wDd"},
  {O_NEMU|O_M68020, "mulu.l", two (0046000, 000000), two (0177700, 0107770), ";lD1"},
  {O_NEMU|O_M68020, "mulu.l", two (0046000, 002000), two (0177700, 0107770), ";lD3D1"},
  {O_NEMU, "mulu.w", one (0140300), one (0170700), ";wDd"},
  {O_NEMU, "mulu", one (0140300), one (0170700), ";wDd"},
  {O_NEMU, "nbcd", one (0044000), one (0177700), "$s"},
  {0, "neg.b", one (0042000), one (0177700), "$s"},
  {0, "neg.l", one (0042200), one (0177700), "$s"},
  {0, "neg.w", one (0042100), one (0177700), "$s"},
  {O_NEMU, "negx.b", one (0040000), one (0177700), "$s"},
  {O_NEMU, "negx.l", one (0040200), one (0177700), "$s"},
  {O_NEMU, "negx.w", one (0040100), one (0177700), "$s"},
  {0, "nop", one (0047161), one (0177777), ""},
  {0, "not.b", one (0043000), one (0177700), "$s"},
  {0, "not.l", one (0043200), one (0177700), "$s"},
  {0, "not.w", one (0043100), one (0177700), "$s"},

  {0, "or", one (0000074), one (0177777), "#bCs"}, /* ori to ccr */
  {0, "or", one (0000174), one (0177777), "#wSs"}, /* ori to sr */
  {0, "ori", one (0000074), one (0177777), "#bCs"}, /* ori to ccr */
  {0, "ori", one (0000174), one (0177777), "#wSs"}, /* ori to sr */
  {0, "or.b", one (0000000), one (0177700), "#b$s"}, /* ori written as or */
  {0, "or.b", one (0100000), one (0170700), ";bDd"}, /* memory to register */
  {0, "or.b", one (0100400), one (0170700), "Dd~s"}, /* register to memory */
  {0, "ori.b", one (0000000), one (0177700), "#b$s"},
  {0, "ori.l", one (0000200), one (0177700), "#l$s"},
  {0, "ori.w", one (0000100), one (0177700), "#w$s"},
  {0, "or.l", one (0000200), one (0177700), "#l$s"},
  {0, "or.l", one (0100200), one (0170700), ";lDd"}, /* memory to register */
  {0, "or.l", one (0100600), one (0170700), "Dd~s"}, /* register to memory */
  {0, "or.w", one (0000100), one (0177700), "#w$s"},
  {0, "or.w", one (0100100), one (0170700), ";wDd"}, /* memory to register */
  {0, "or.w", one (0100500), one (0170700), "Dd~s"}, /* register to memory */
  {O_WARN, "or.w", one (0000074), one (0177777), "#bCs"}, /* ori to ccr */
  {O_WARN, "or.w", one (0000174), one (0177777), "#wSs"}, /* ori to sr */

  {O_NEMU|O_M68020, "pack", one (0100500), one (0170770), "DsDd#w"}, /* pack Ds, Dd, #w */
  {O_NEMU|O_M68020, "pack", one (0100510), one (0170770), "-s-d#w"}, /* pack -(As), -(Ad), #w */
  {0, "pea", one (0044100), one (0177700), "!s"},
  {O_NEMU, "reset", one (0047160), one (0177777), ""},

  {0, "rol.b", one (0160430), one (0170770), "QdDs"}, /* rorb #Q, Ds */
  {0, "rol.b", one (0160470), one (0170770), "DdDs"}, /* rorb Dd, Ds */
  {0, "rol.l", one (0160630), one (0170770), "QdDs"}, /* rorb #Q, Ds */
  {0, "rol.l", one (0160670), one (0170770), "DdDs"}, /* rorb Dd, Ds */
  {0, "rol.w", one (0160530), one (0170770), "QdDs"}, /* rorb #Q, Ds */
  {0, "rol.w", one (0160570), one (0170770), "DdDs"}, /* rorb Dd, Ds */
  {0, "rol.w", one (0163700), one (0177700), "~s"},	/* Rotate memory */
  {0, "ror.b", one (0160030), one (0170770), "QdDs"}, /* rorb #Q, Ds */
  {0, "ror.b", one (0160070), one (0170770), "DdDs"}, /* rorb Dd, Ds */
  {0, "ror.l", one (0160230), one (0170770), "QdDs"}, /* rorb #Q, Ds */
  {0, "ror.l", one (0160270), one (0170770), "DdDs"}, /* rorb Dd, Ds */
  {0, "ror.w", one (0160130), one (0170770), "QdDs"}, /* rorb #Q, Ds */
  {0, "ror.w", one (0160170), one (0170770), "DdDs"}, /* rorb Dd, Ds */
  {0, "ror.w", one (0163300), one (0177700), "~s"},	/* Rotate memory */

  {O_NEMU, "roxl.b", one (0160420), one (0170770), "QdDs"}, /* roxrb #Q, Ds */
  {O_NEMU, "roxl.b", one (0160460), one (0170770), "DdDs"}, /* roxrb Dd, Ds */
  {O_NEMU, "roxl.l", one (0160620), one (0170770), "QdDs"}, /* roxrb #Q, Ds */
  {O_NEMU, "roxl.l", one (0160660), one (0170770), "DdDs"}, /* roxrb Dd, Ds */
  {O_NEMU, "roxl.w", one (0160520), one (0170770), "QdDs"}, /* roxrb #Q, Ds */
  {O_NEMU, "roxl.w", one (0160560), one (0170770), "DdDs"}, /* roxrb Dd, Ds */
  {O_NEMU, "roxl.w", one (0162700), one (0177700), "~s"}, /* Rotate memory */
  {O_NEMU, "roxr.b", one (0160020), one (0170770), "QdDs"}, /* roxrb #Q, Ds */
  {O_NEMU, "roxr.b", one (0160060), one (0170770), "DdDs"}, /* roxrb Dd, Ds */
  {O_NEMU, "roxr.l", one (0160220), one (0170770), "QdDs"}, /* roxrb #Q, Ds */
  {O_NEMU, "roxr.l", one (0160260), one (0170770), "DdDs"}, /* roxrb Dd, Ds */
  {O_NEMU, "roxr.w", one (0160120), one (0170770), "QdDs"}, /* roxrb #Q, Ds */
  {O_NEMU, "roxr.w", one (0160160), one (0170770), "DdDs"}, /* roxrb Dd, Ds */
  {O_NEMU, "roxr.w", one (0162300), one (0177700), "~s"}, /* Rotate memory */

  {O_NEMU|O_M68010, "rtd", one (0047164), one (0177777), "#w"},
  {O_NEMU, "rte", one (0047163), one (0177777), ""},
  {O_NEMU|O_M68020, "rtm", one (0003300), one (0177760), "Rs"},
  {O_NEMU, "rtr", one (0047167), one (0177777), ""},
  {O_WARN, "rts", one (0047165), one (0177777), ""},

  {0, "scc", one (0052300), one (0177700), "$s"},
  {0, "scs", one (0052700), one (0177700), "$s"},
  {0, "slo", one (0052700), one (0177700), "$s"},
  {0, "seq", one (0053700), one (0177700), "$s"},
  {0, "sf", one (0050700), one (0177700), "$s"},
  {0, "sge", one (0056300), one (0177700), "$s"},
  {0, "sgt", one (0057300), one (0177700), "$s"},
  {0, "shi", one (0051300), one (0177700), "$s"},
  {0, "sle", one (0057700), one (0177700), "$s"},
  {0, "sls", one (0051700), one (0177700), "$s"},
  {0, "slt", one (0056700), one (0177700), "$s"},
  {0, "smi", one (0055700), one (0177700), "$s"},
  {0, "sne", one (0053300), one (0177700), "$s"},
  {0, "spl", one (0055300), one (0177700), "$s"},
  {0, "st", one (0050300), one (0177700), "$s"},
  {0, "svc", one (0054300), one (0177700), "$s"},
  {0, "svs", one (0054700), one (0177700), "$s"},

  {O_WARN, "scc.b", one (0052300), one (0177700), "$s"},
  {O_WARN, "scs.b", one (0052700), one (0177700), "$s"},
  {O_WARN, "slo.b", one (0052700), one (0177700), "$s"},
  {O_WARN, "seq.b", one (0053700), one (0177700), "$s"},
  {O_WARN, "sf.b", one (0050700), one (0177700), "$s"},
  {O_WARN, "sge.b", one (0056300), one (0177700), "$s"},
  {O_WARN, "sgt.b", one (0057300), one (0177700), "$s"},
  {O_WARN, "shi.b", one (0051300), one (0177700), "$s"},
  {O_WARN, "sle.b", one (0057700), one (0177700), "$s"},
  {O_WARN, "sls.b", one (0051700), one (0177700), "$s"},
  {O_WARN, "slt.b", one (0056700), one (0177700), "$s"},
  {O_WARN, "smi.b", one (0055700), one (0177700), "$s"},
  {O_WARN, "sne.b", one (0053300), one (0177700), "$s"},
  {O_WARN, "spl.b", one (0055300), one (0177700), "$s"},
  {O_WARN, "st.b", one (0050300), one (0177700), "$s"},
  {O_WARN, "svc.b", one (0054300), one (0177700), "$s"},
  {O_WARN, "svs.b", one (0054700), one (0177700), "$s"},

  {O_NEMU, "sbcd", one (0100400), one (0170770), "DsDd"}, /* sbcd Ds, Dd */
  {O_NEMU, "sbcd", one (0100410), one (0170770), "-s-d"}, /* sbcd -(As), -(Ad) */
  {O_NEMU, "stop", one (0047162), one (0177777), "#w"},

  {0, "suba.l", one (0110700), one (0170700), "*lAd"},
  {0, "suba.w", one (0110300), one (0170700), "*wAd"},
  {0, "sub.b", one (0002000), one (0177700), "#b$s"}, /* subi written as sub */
  {0, "sub.b", one (0110000), one (0170700), ";bDd"}, /* subb ??, Dd */
  {0, "sub.b", one (0110400), one (0170700), "Dd~s"}, /* subb Dd, ?? */
  {0, "subi.b", one (0002000), one (0177700), "#b$s"},
  {0, "subi.l", one (0002200), one (0177700), "#l$s"},
  {0, "subi.w", one (0002100), one (0177700), "#w$s"},
  {0, "sub.l", one (0002200), one (0177700), "#l$s"},
  {0, "sub.l", one (0110200), one (0170700), "*lDd"},
  {0, "sub.l", one (0110600), one (0170700), "Dd~s"},
  {0, "sub.l", one (0110700), one (0170700), "*lAd"},
  {0, "subq.b", one (0050400), one (0170700), "Qd%s"},
  {0, "subq.l", one (0050600), one (0170700), "Qd%s"},
  {0, "subq.w", one (0050500), one (0170700), "Qd%s"},
  {O_WARN, "subq", one (0050500), one (0170700), "Qd%s"},
  {0, "sub.w", one (0002100), one (0177700), "#w$s"},
  {0, "sub.w", one (0110100), one (0170700), "*wDd"},
  {0, "sub.w", one (0110300), one (0170700), "*wAd"}, /* suba written as sub */
  {0, "sub.w", one (0110500), one (0170700), "Dd~s"},

  {O_NEMU, "subx.b", one (0110400), one (0170770), "DsDd"}, /* subxb Ds, Dd */
  {O_NEMU, "subx.b", one (0110410), one (0170770), "-s-d"}, /* subxb -(As), -(Ad) */
  {O_NEMU, "subx.l", one (0110600), one (0170770), "DsDd"},
  {O_NEMU, "subx.l", one (0110610), one (0170770), "-s-d"},
  {O_NEMU, "subx.w", one (0110500), one (0170770), "DsDd"},
  {O_NEMU, "subx.w", one (0110510), one (0170770), "-s-d"},

  {0, "swap", one (0044100), one (0177770), "Ds"},

  {O_NEMU, "tas", one (0045300), one (0177700), "$s"},
  {O_NEMU, "trap", one (0047100), one (0177760), "Ts"},

  {O_NEMU|O_M68020, "trapcc", one (0052374), one (0177777), ""},
  {O_NEMU|O_M68020, "trapcs", one (0052774), one (0177777), ""},
  {O_NEMU|O_M68020, "trapeq", one (0053774), one (0177777), ""},
  {O_NEMU|O_M68020, "trapf", one (0050774), one (0177777), ""},
  {O_NEMU|O_M68020, "trapge", one (0056374), one (0177777), ""},
  {O_NEMU|O_M68020, "trapgt", one (0057374), one (0177777), ""},
  {O_NEMU|O_M68020, "traphi", one (0051374), one (0177777), ""},
  {O_NEMU|O_M68020, "traple", one (0057774), one (0177777), ""},
  {O_NEMU|O_M68020, "trapls", one (0051774), one (0177777), ""},
  {O_NEMU|O_M68020, "traplt", one (0056774), one (0177777), ""},
  {O_NEMU|O_M68020, "trapmi", one (0055774), one (0177777), ""},
  {O_NEMU|O_M68020, "trapne", one (0053374), one (0177777), ""},
  {O_NEMU|O_M68020, "trappl", one (0055374), one (0177777), ""},
  {O_NEMU|O_M68020, "trapt", one (0050374), one (0177777), ""},
  {O_NEMU|O_M68020, "trapvc", one (0054374), one (0177777), ""},
  {O_NEMU|O_M68020, "trapvs", one (0054774), one (0177777), ""},

  {O_NEMU|O_M68020, "trapcc.w", one (0052372), one (0177777), ""},
  {O_NEMU|O_M68020, "trapcs.w", one (0052772), one (0177777), ""},
  {O_NEMU|O_M68020, "trapeq.w", one (0053772), one (0177777), ""},
  {O_NEMU|O_M68020, "trapf.w", one (0050772), one (0177777), ""},
  {O_NEMU|O_M68020, "trapge.w", one (0056372), one (0177777), ""},
  {O_NEMU|O_M68020, "trapgt.w", one (0057372), one (0177777), ""},
  {O_NEMU|O_M68020, "traphi.w", one (0051372), one (0177777), ""},
  {O_NEMU|O_M68020, "traple.w", one (0057772), one (0177777), ""},
  {O_NEMU|O_M68020, "trapls.w", one (0051772), one (0177777), ""},
  {O_NEMU|O_M68020, "traplt.w", one (0056772), one (0177777), ""},
  {O_NEMU|O_M68020, "trapmi.w", one (0055772), one (0177777), ""},
  {O_NEMU|O_M68020, "trapne.w", one (0053372), one (0177777), ""},
  {O_NEMU|O_M68020, "trappl.w", one (0055372), one (0177777), ""},
  {O_NEMU|O_M68020, "trapt.w", one (0050372), one (0177777), ""},
  {O_NEMU|O_M68020, "trapvc.w", one (0054372), one (0177777), ""},
  {O_NEMU|O_M68020, "trapvs.w", one (0054772), one (0177777), ""},

  {O_NEMU|O_M68020, "trapcc.l", one (0052373), one (0177777), ""},
  {O_NEMU|O_M68020, "trapcs.l", one (0052773), one (0177777), ""},
  {O_NEMU|O_M68020, "trapeq.l", one (0053773), one (0177777), ""},
  {O_NEMU|O_M68020, "trapf.l", one (0050773), one (0177777), ""},
  {O_NEMU|O_M68020, "trapge.l", one (0056373), one (0177777), ""},
  {O_NEMU|O_M68020, "trapgt.l", one (0057373), one (0177777), ""},
  {O_NEMU|O_M68020, "traphi.l", one (0051373), one (0177777), ""},
  {O_NEMU|O_M68020, "traple.l", one (0057773), one (0177777), ""},
  {O_NEMU|O_M68020, "trapls.l", one (0051773), one (0177777), ""},
  {O_NEMU|O_M68020, "traplt.l", one (0056773), one (0177777), ""},
  {O_NEMU|O_M68020, "trapmi.l", one (0055773), one (0177777), ""},
  {O_NEMU|O_M68020, "trapne.l", one (0053373), one (0177777), ""},
  {O_NEMU|O_M68020, "trappl.l", one (0055373), one (0177777), ""},
  {O_NEMU|O_M68020, "trapt.l", one (0050373), one (0177777), ""},
  {O_NEMU|O_M68020, "trapvc.l", one (0054373), one (0177777), ""},
  {O_NEMU|O_M68020, "trapvs.l", one (0054773), one (0177777), ""},

  {O_NEMU, "trapv", one (0047166), one (0177777), ""},

  {O_WARN, "tst.b", one (0045000), one (0177700), ";b"},
  {O_WARN, "tst.w", one (0045100), one (0177700), "*w"},
  {O_WARN, "tst.l", one (0045200), one (0177700), "*l"},

  {O_NEMU, "unlk", one (0047130), one (0177770), "As"},
  {O_NEMU|O_M68020, "unpk", one (0100600), one (0170770), "DsDd#w"}, /* unpk Ds, Dd, #w */
  {O_NEMU|O_M68020, "unpk", one (0100610), one (0170770), "-s-d#w"}, /* unpk -(As), -(Ad), #w */


#ifdef SUN_MNEMONICS
		/* JF this is for SUN, which has different mnemonics */
  {0, "jbsr", one (0060400), one (0177400), "Bg"},
  {0, "jra", one (0060000), one (0177400), "Bg"},
  
  {0, "jhi", one (0061000), one (0177400), "Bg"},
  {0, "jls", one (0061400), one (0177400), "Bg"},
  {0, "jcc", one (0062000), one (0177400), "Bg"},
  {0, "jcs", one (0062400), one (0177400), "Bg"},
  {0, "jne", one (0063000), one (0177400), "Bg"},
  {0, "jeq", one (0063400), one (0177400), "Bg"},
  {0, "jvc", one (0064000), one (0177400), "Bg"},
  {0, "jvs", one (0064400), one (0177400), "Bg"},
  {0, "jpl", one (0065000), one (0177400), "Bg"},
  {0, "jmi", one (0065400), one (0177400), "Bg"},
  {0, "jge", one (0066000), one (0177400), "Bg"},
  {0, "jlt", one (0066400), one (0177400), "Bg"},
  {0, "jgt", one (0067000), one (0177400), "Bg"},
  {0, "jle", one (0067400), one (0177400), "Bg"},

  {0, "mova.l", one (0020100), one (0170700), "*lAd"},
  {0, "mova.w", one (0030100), one (0170700), "*wAd"},
  {0, "mov.b", one (0010000), one (0170000), ";b$d"}, /* mov */
  {0, "mov.l", one (0020000), one (0170000), "*l$d"},
  {0, "mov.l", one (0020100), one (0170700), "*lAd"},
  {0, "mov.l", one (0047140), one (0177770), "AsUd"}, /* mov to USP */
  {0, "mov.l", one (0047150), one (0177770), "UdAs"}, /* mov from USP */
  {0, "mov.l", one (0070000), one (0170400), "MsDd"}, /* movq written as mov */
  {O_MOVEM, "movm.l", one (0044300), one (0177700), "#w&s"}, /* movm reg to mem. */
  {O_MOVEM2, "movm.l", one (0044340), one (0177770), "#w-s"}, /* movm reg to autodecrement. */
  {O_MOVEM, "movm.l", one (0046300), one (0177700), "!s#w"}, /* movm mem to reg. */
  {O_MOVEM, "movm.l", one (0046330), one (0177770), "+s#w"}, /* movm autoinc to reg. */
  {O_MOVEM, "movm.w", one (0044200), one (0177700), "#w&s"}, /* movm reg to mem. */
  {O_MOVEM2, "movm.w", one (0044240), one (0177770), "#w-s"}, /* movm reg to autodecrement. */
  {O_MOVEM, "movm.w", one (0046200), one (0177700), "!s#w"}, /* movm mem to reg. */
  {O_MOVEM, "movm.w", one (0046230), one (0177770), "+s#w"}, /* movm autoinc to reg. */
  {0, "movp.l", one (0000510), one (0170770), "dsDd"}, /* memory to register */
  {0, "movp.l", one (0000710), one (0170770), "Ddds"}, /* register to memory */
  {0, "movp.w", one (0000410), one (0170770), "dsDd"}, /* memory to register */
  {0, "movp.w", one (0000610), one (0170770), "Ddds"}, /* register to memory */
  {0, "movq", one (0070000), one (0170400), "MsDd"},
  {0, "mov.w", one (0030000), one (0170000), "*w$d"},
  {0, "mov.w", one (0030100), one (0170700), "*wAd"}, /* mova, written as mov */
  {0, "mov", one (0040300), one (0177700), "Ss$s"}, /* Move from sr */
  {0, "mov", one (0041300), one (0177700), "Cs$s"}, /* Move from ccr */
  {0, "mov", one (0042300), one (0177700), ";wCd"}, /* mov to ccr */
  {0, "mov", one (0043300), one (0177700), ";wSd"}, /* mov to sr */
/* movc not done */

  {O_NEMU|O_M68010, "movs.b", two (0007000, 0), two (0177700, 07777), "~sR1"},  /* movs from memory */
  {O_NEMU|O_M68010, "movs.b", two (0007000, 04000), two (0177700, 07777), "R1~s"},  /* movs to memory */
  {O_NEMU|O_M68010, "movs.l", two (0007200, 0), two (0177700, 07777), "~sR1"},  /* movs from memory */
  {O_NEMU|O_M68010, "movs.l", two (0007200, 04000), two (0177700, 07777), "R1~s"},  /* movs to memory */
  {O_NEMU|O_M68010, "movs.w", two (0007100, 0), two (0177700, 07777), "~sR1"},  /* movs from memory */
  {O_NEMU|O_M68010, "movs.w", two (0007100, 04000), two (0177700, 07777), "R1~s"},  /* movs to memory */
#endif // SUN_MNEMONICS
};

inttype numopcodes=sizeof(m68k_opcodes)/sizeof(m68k_opcodes[0]);

/*
struct m68k_opcode *endop = m68k_opcodes+sizeof(m68k_opcodes)/sizeof(m68k_opcodes[0]);;
*/
