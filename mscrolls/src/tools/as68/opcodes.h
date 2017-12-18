
#pragma once

extern struct m68k_opcode
{
  char flags;
#define O_WARN		1		/* opcodes which ken reckons are really illegal */
#define O_M68010	2		/* 68010 opcode */
#define O_M68020	4		/* 68020 opcode */
#define O_FP		8		/* FP coprocessor opcode */
#define O_MOVEM		16		/* move multiple instruction not movem ..,-(An) */
#define O_MOVEM2	32		/* movem ..,-(An) */
#define O_NEMU		64		/* not emulated */
#define O_EMU		128		/* only emulated */
  char *name;
  unsigned long opcode;
  unsigned long  match;
  char *args;
} m68k_opcodes[];

extern inttype numopcodes;
