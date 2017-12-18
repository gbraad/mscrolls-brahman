/**
 *
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 */

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include "inttype.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <string.h>
#include "vfiles.h"
#include "alloc.h"
#include "sections.h"
#include "symbols.h"
#include "tokens.h"
#include "expr.h"
#include "ea.h"
#include "directives.h"
#include "opcodes.h"
#include "getline.h"
#include "parse.h"
#include "assemble.h"
#include "errors.h"
#include "macros.h"
#include "reloc.h"
#include "assemins.h"
#include "output.h"
#include "include.h"

#ifdef DEBUG
extern inttype debug;
#endif

jmp_buf mainloop;
struct symbol *lastsym;
inttype cond_depth=0;			/* current conditional depth */
inttype stop_depth=0;			/* conditional depth when assembling stopped */
inttype assembling=1;			/* set when assembling (used for conditionals) */
inttype movem_flag=0;			/* set when assembling movem instructions */

extern inttype n_errors;
extern inttype nexttoken;
extern char *yytext;

extern struct ea args[];
extern inttype n_args;
extern inttype pass;
extern inttype include_depth;		/* current include file nest depth */
extern inttype in_macro;			/* current macro nest depth */
extern char *program_unit_name;
extern char current_filename[];
extern inttype current_lineno;
extern struct symbol star;
extern inttype memorytemp;
extern FILE *ftemp;
extern VFILE *vtemp;

extern int lex();

inttype local_index=0;

static void setsym(struct symbol* sym)
{
    unsigned long value = current_section->pc + current_section->load_address;
    if (sym) {
	if (!isdigit(sym->name[0])) local_index++;
	if (sym->type == S_UNDF) {
	    sym->section_index = current_index;
	    sym->svalue = value;
	    sym->type = S_RELOC;
	} else if (sym->type == S_XREF) {
	    sym->section_index = current_index;
	    sym->svalue = value;
	    sym->type = S_RELOC;
	    if (pass==1)
		warning("\"%s\" has been XREFed in this file", sym->name);
	} else if (pass == 2) {
	    if (sym->svalue != value)
		if (!n_errors)	/* avoid error cascades in 2nd pass */
		    error(
"symbol %s given different value in 2nd pass: %d != %d",
			  sym->name, sym->svalue, current_section->pc);
	} else if (pass == 1) error("\"%s\" redefined", sym->name);
    }
}


void assemble()
{
    inttype t,i;
    char *p;
    struct section *sp;
    inttype opindex, sect_type;
    char sect_name[30];
    struct expr e;

    setjmp(mainloop);	/* error longjmps to here */

    for (;;) {
        star.svalue = current_section->pc;
        star.section_index = current_index;

        getline();		/* read a line from input stream/macro */

        if (*label_field) {
            lastsym = find_symbol(label_field);
            if (assembling && !lastsym)	/* undefined? */
                lastsym = add_symbol(label_field);
        } else
            lastsym=0;

        if (!*opcode_field) {
            if (assembling) {
                setsym(lastsym);
                dump_line();
            }
            continue;
        }

        if (!assembling) {
            puttemp('\n'); /* make sure no. lines in tempfile correct */
            if (opcode_symbol)
            {
                if (opcode_symbol->type == S_TOKEN)
                {
                    switch ((int)opcode_symbol->svalue)
                    {
                    case D_IFEQ :
                    case D_IFNE :
                    case D_IFGT :
                    case D_IFGE :
                    case D_IFLT :
                    case D_IFLE :
                    case D_IFC :
                    case D_IFNC :
                    case D_IFD :
                    case D_IFND :
                        /* remember depth of conditionals */
                        cond_depth++;
                        break;
                    case D_ELSE :		/* turn on assembling */
                        assembling = 1;
                        break;
                    case D_ENDC :
                        cond_depth--;
                        /* have we reached the depth at which assembling stopped? */
                        if (cond_depth == stop_depth)
                            assembling=1;
                        break;
                    }
                }
                else if (opcode_symbol->type == S_MACRO)
                    expand_macro(opcode_symbol);
            }
            continue;
        }

        switch (opcode_symbol->type) {
        case S_MACRO :
            puttemp('\n');
            setsym(lastsym);
            if (*label_field && pass==1)	/* put the label into temp file */
            {
                if (memorytemp)
                    Vprintf(vtemp, "%s\n", label_field);
                else
                    fprintf(ftemp, "%s\n", label_field);
            }
            expand_macro(opcode_symbol);
            continue;
        case S_OPCODE :
            {	struct m68k_opcode *op;
#ifdef A_OUT
                if (pass==1)
                    add_line_number(current_lineno,
                                    current_section->pc);
#endif
                dump_line();				/* dump to temp file in 1st pass */
                setsym(lastsym);
                opindex = opcode_symbol->svalue;	/* index in opcode table */
                op = &m68k_opcodes[opindex];
                if (op->flags&(O_MOVEM|O_MOVEM2))
                    movem_flag=1;
                else
                    movem_flag=0;
                /* parse the arguments */
                parse_args();
                assemble_instruction(opindex);
                if (current_section->type != CODE)
                    warning("instructions in data/bss section");
                continue;
            }
        case S_TOKEN :
            break;
        default :
            error("bad opcode");
            continue;
        }

        /* handle directives */
        switch ((int)opcode_symbol->svalue) {
        case D_SECTION :
            dump_line();
            if (lex() != T_STRING)
                error("section name expected");
            strcpy(sect_name, yylval.string);
            if (lex() != ',')
                error("comma expected");
            t = lex();
            switch ((int)t) {
            case T_CODE :
                sect_type = CODE;
                break;
            case T_DATA :
                sect_type = DATA;
                break;
            case T_BSS :
                sect_type = BSS;
                break;
            default :
                error("bad section type");
                sect_type = CODE;
            }
            for (i=0, sp=sections; i<n_sections; i++, sp++)
                if (sp->type == sect_type &&
                    !strcmp(sp->name, sect_name)) break;
            if (i==n_sections) {	/* new section ? */
#ifdef DEBUG
                if (debug)
                    printf("creating section %s,%d\n",
                           sect_name, sect_type);
#endif
                sp->name = allocstr(sect_name);
                strcpy(sp->name, sect_name);
                sp->type = sect_type;
                sp->pc = 0;
                sp->contains_ext=0;
                sp->load_address=0L;
                current_section = sp;
                current_index = i;
                n_sections++;
            } else {
#ifdef DEBUG
                if (debug)
                    printf("changing to section %s,%d\n",
                           sect_name, sect_type);
#endif
                current_section = sp;
                current_index = i;
            }
            setsym(lastsym);
            break;

        case D_RORG :
            dump_line();
            e = *parse_expr();
            if (e.sym && (e.sym->type==S_UNDF || e.sym->type==S_XREF))
                error("RORG address must be defined before use");
            current_section->pc = e.value;
            setsym(lastsym);
            break;
		
        case D_OFFSET :
            puttemp('\n');
            error("OFFSET not implemented");
            break;

        case D_END :
            dump_line();
            if (include_depth)
                error("END in include file");
            return;		/* end of pass */

        case D_SET :
            dump_line();
            if (!lastsym)
                error("SET must have a label");
            if (lastsym->type != S_UNDF && !(lastsym->flags & F_SET))
                error("\"%s\" redefined", lastsym->name);
            lastsym->flags |= F_SET;
            e = *parse_expr();
            if (e.sym)
                if (e.sym->type == S_UNDF)
                    error("illegal forward reference");
                else {
                    lastsym->type = e.sym->type;
                    lastsym->svalue = e.value;
                    lastsym->othersym = e.sym;
                }
            else {
                lastsym->type = S_ABS;	/* absolute value */
                lastsym->svalue = e.value;
            }
            break;

        case D_EQU :
            puttemp('\n');
            if (!lastsym)
                error("EQU must have a label");
            if (lastsym->type != S_UNDF)
                error("\"%s\" redefined", lastsym->name);
            e = *parse_expr();
            if (e.sym)
                if (e.sym->type == S_UNDF)
                    error("illegal forward reference");
                else {
                    lastsym->type = e.sym->type;
                    lastsym->svalue = e.value;
                    lastsym->othersym = e.sym;
                }
            else {
                lastsym->type = S_ABS;	/* absolute value */
                lastsym->svalue = e.value;
            }
            break;

        case D_EQUR :
            dump_line();
            if (!lastsym)
                error("EQUR must have a label");
            t = lex();
            if (t == T_DREG) {
                lastsym->type = S_DREG;
                lastsym->svalue = yylval.number;
            } else if (t == T_AREG) {
                lastsym->type = S_AREG;
                lastsym->svalue = yylval.number;
            } else
                error("EQUR must have a register argument");
            break;
		
        case D_REG :
            if (!lastsym)
                error("REG requires a label");
            if (lastsym->type != S_UNDF)
                error("\"%s\" redefined", lastsym->name);
            nexttoken = lex();		/* set up for register_mask() */
            lastsym->type = S_REG;
            lastsym->svalue = register_mask();
            break;

        case D_DC_B :
            dump_line();
            setsym(lastsym);
            nexttoken = ',';	/* bodge */
            while (nexttoken == ',') {
                match(',');
                if (nexttoken != T_ASCII && nexttoken != T_STRING) {	/* check for dc.b 'string' */
                    e = *expr();
                    if (e.sym) switch (e.sym->type) {
                        case S_ABS :
                            output_byte(e.value);
                            break;
                        case S_RELOC :
                            output_byte(e.value - current_section->pc);
                            break;
                        case S_XREF :
                            if (pass==2) {
                                add_ref8(e.sym, current_section->pc);
                            }
                            output_byte(e.value);
                            break;
                        case S_UNDF :
                            output_byte((inttype)0);
                            break;
                        } else
                        output_byte(e.value);
                } else {
                    for (p=yylval.string; *p; p++)
                        output_byte((inttype)*p);
                    nexttoken = lex();
                }
            }
            break;

        case D_DC_W :
            dump_line();
            align(1);
            setsym(lastsym);
            nexttoken = ',';	/* bodge */
            while (nexttoken == ',') {
                match(',');
                e = *expr();
                if (e.sym) switch (e.sym->type) {
                    case S_ABS :
                        output_word(e.value);
                        break;
                    case S_RELOC :
                        if (pass==2) {
                            warning("16 bit relocation");
                            add_reloc16(e.sym, current_section->pc);
                        }
                        output_word(e.value);
                        break;
                    case S_XREF :
                        if (pass==2) {
                            add_ref16(e.sym, current_section->pc);
                        }
                        output_word(e.value);
                        break;
                    case S_UNDF :
                        output_word((inttype)0);
                        break;
                    } else
                    output_word(e.value);
            }
            break;

        case D_DC_L :
            dump_line();
            align(1);
            setsym(lastsym);
            nexttoken = ',';	/* bodge */
            while (nexttoken == ',') {
                match(',');
                e = *expr();
                if (e.sym) switch (e.sym->type) {
                    case S_ABS :
                        output_long(e.value);
                        break;
                    case S_RELOC :
                        if (pass==2)
                            add_reloc32(e.sym, current_section->pc);
                        output_long(e.value);
                        break;
                    case S_XREF :
                        if (pass==2)
                            add_ref32(e.sym, current_section->pc);
                        output_long(e.value);
                        break;
                    case S_UNDF :
                        output_long((inttype)0);
                        break;
                    } else
                    output_long(e.value);
            }
            break;

        case D_DCB_B :
            dump_line();
            setsym(lastsym);
            e = *parse_expr();
            if (e.sym)
                error("first argument to DCB must be absolute");
            i = e.value;
            if (nexttoken != ',')
                error("comma expected");
            nexttoken = lex();
            if (nexttoken == T_ASCII)
                for (; i; i--)
                    for (p=yylval.string; *p; p++)
                        output_byte((inttype)*p);
            else {
                e = *expr();
                for (; i; i--)
                    if (e.sym) switch (e.sym->type) {
                        case S_ABS :
                            output_byte(e.value);
                            break;
                        case S_RELOC :
                            output_byte(e.value - current_section->pc);
                            break;
                        case S_XREF :
                            if (pass==2) {
                                add_ref8(e.sym, current_section->pc);
                            }
                            output_byte(e.value);
                            break;
                        case S_UNDF :
                            output_byte((inttype)0);
                        } else
                        output_byte(e.value);
            }
            break;

        case D_DCB_W :
            dump_line();
            setsym(lastsym);
            align(0);
            e = *parse_expr();
            if (e.sym)
                error("first argument to DCB must be absolute");
            i = e.value;
            if (nexttoken != ',')
                error("comma expected");
            e = *parse_expr();
            for (; i; i--)
                if (e.sym) switch (e.sym->type) {
                    case S_ABS :
                        output_word(e.value);
                        break;
                    case S_RELOC :
                        if (pass==2) {
                            warning("16 bit relocation");
                            add_reloc16(e.sym, current_section->pc);
                        }
                        output_word(e.value);
                        break;
                    case S_XREF :
                        if (pass==2) {
                            add_ref16(e.sym, current_section->pc);
                        }
                        output_word(e.value);
                        break;
                    case S_UNDF :
                        output_word((inttype)0);
                    } else
                    output_word(e.value);
            break;

        case D_DCB_L :
            dump_line();
            setsym(lastsym);
            align(0);
            e = *parse_expr();
            if (e.sym)
                error("first argument to DCB must be absolute");
            i = e.value;
            if (nexttoken != ',')
                error("comma expected");
            e = *parse_expr();
            for (; i; i--)
                if (e.sym) switch (e.sym->type) {
                    case S_ABS :
                        output_long(e.value);
                        break;
                    case S_RELOC :
                        if (pass==2)
                            add_reloc32(e.sym, current_section->pc);
                        output_long(e.value - current_section->pc);
                        break;
                    case S_XREF :
                        if (pass==2)
                            add_ref32(e.sym, current_section->pc);
                        output_long(e.value);
                        break;
                    case S_UNDF :
                        output_long((inttype)0);
                    } else
                    output_long(e.value);
            break;

        case D_DS_B :
            dump_line();
            setsym(lastsym);
            e = *parse_expr();
            if (e.sym)
                error("argument to DS must be absolute");
            current_section->pc += e.value;
            break;

        case D_DS_W :
            dump_line();
            align(0);
            setsym(lastsym);
            e = *parse_expr();
            if (e.sym)
                error("argument to DS must be absolute");
            current_section->pc += 2*e.value;
            break;

        case D_DS_L :
            dump_line();
            align(0);
            setsym(lastsym);
            e = *parse_expr();
            if (e.sym)
                error("argument to DS must be absolute");
            current_section->pc += 4*e.value;
            break;

        case D_FAIL :
            dump_line();
            error("FAIL!");
            break;
		
        case D_CNOP :
            dump_line();
            {	inttype offset, bdry, ad;
                e = *parse_expr();
                if (e.sym)
                    error("arguments to CNOP must be absolute");
                if (nexttoken != ',')
                    error("comma expected");
                offset = e.value;
                e = *parse_expr();
                if (e.sym)
                    error("arguments to CNOP must be absolute");
                bdry = e.value;
                ad = current_section->pc;
                ad = ad - (ad % bdry) + offset;
                if (ad < current_section->pc) ad += bdry;
                current_section->pc = ad;
            }
            break;

        case D_IFEQ :
            puttemp('\n');
            e = *parse_expr();
            if (e.value != 0) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;
            break;

        case D_IFNE :
            puttemp('\n');
            e = *parse_expr();
            if (e.value == 0) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFGT :
            puttemp('\n');
            e = *parse_expr();
            if (e.value <= 0) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFGE :
            puttemp('\n');
            e = *parse_expr();
            if (e.value < 0) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFLT :
            puttemp('\n');
            e = *parse_expr();
            if (e.value >= 0) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFLE :
            puttemp('\n');
            e = *parse_expr();
            if (e.value > 0) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFC :
            puttemp('\n');
            t = lex();
            if (t != T_ASCII)
                error("arguments to IFC must be strings");
            p = allocstr(yylval.string);	/* space to store 1st arg */
            strcpy(p, yylval.string);
            if (lex() != ',')
                error("comma expected");
            t = lex();
            if (t != T_ASCII)
                error("arguments to IFC must be strings");
            if (strcmp(p, yylval.string)) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            free(p);
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFNC :
            puttemp('\n');
            t = lex();
            if (t != T_ASCII)
                error("arguments to IFNC must be strings");
            p = allocstr(yylval.string);	/* space to store 1st arg */
            strcpy(p, yylval.string);
            if (lex() != ',')
                error("comma expected");
            t = lex();
            if (t != T_ASCII)
                error("arguments to IFNC must be strings");
            if (!strcmp(p, yylval.string)) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            free(p);
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFD :
            puttemp('\n');
            t = lex();
            if (t != T_LABEL)
                error("label expected");
            if (!yylval.symbol || yylval.symbol->type==S_UNDF) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_IFND :
            puttemp('\n');
            t = lex();
            if (t != T_LABEL)
                error("label expected");
            if (yylval.symbol && yylval.symbol->type!=S_UNDF) {
                stop_depth=cond_depth;	/* remember when assembling stopped */
                assembling=0;
            }
            cond_depth++;	/* remember how deep in conditional */
            break;

        case D_ELSE :
            puttemp('\n');
            if (!cond_depth)
                error("ELSE without matching IFxx");
            assembling = 0;	/* turn off assembling */
            break;

        case D_ENDC :
            puttemp('\n');
            if (!cond_depth)
                error("ENDC without matching IFxx");
            cond_depth--;
            break;

        case D_MACRO :
            puttemp('\n');
            if (!lastsym)
                error("MACRO requires a label");
            if (lastsym->type != S_UNDF)
                error("\"%s\" redefined", lastsym->name);
            lastsym->type = S_MACRO;
            lastsym->charp = define_macro();
            break;

        case D_ENDM :
            puttemp('\n');
            error("ENDM without matching MACRO");
            break;

        case D_MEXIT :
            puttemp('\n');
            if (!in_macro)
                error("MEXIT must be inside a macro");
            exit_macro();
            break;

        case D_XDEF :
            puttemp('\n');
            current_section->contains_ext=1;
            t = ',';
            while (t == ',') {
                if ((t = lex()) != T_LABEL)
                    error("label expected");
                if (!yylval.symbol) yylval.symbol = add_symbol(yytext);
                yylval.symbol->flags |= F_EXTERNAL;
                if (yylval.symbol->type == S_XREF) {
                    warning(
                            "symbol \"%s\" has been XREFed in this file", yylval.symbol->name
                            );
                    yylval.symbol->type = S_UNDF;
                }
                t = lex();
            }
            if (t)
                error("\"%s\" unexpected", yytext);
            break;

        case D_XREF :
            puttemp('\n');
            current_section->contains_ext=1;
            t = ',';
            while (t == ',') {
                if ((t = lex()) != T_LABEL)
                    error("label expected");
                if (!yylval.symbol) yylval.symbol = add_symbol(yytext);
                if (yylval.symbol->type != S_UNDF &&
                    yylval.symbol->type != S_XREF && pass==1)
                    warning("\"%s\" has been defined in this file",
                            yylval.symbol->name);
                else {
                    yylval.symbol->type = S_XREF;
                    yylval.symbol->svalue = 0;
                }
                t = lex();
            }
            if (t)
                error("\"%s\" unexpected", yytext);
            break;

        case D_INCLUDE :
            puttemp('\n');
            if (lastsym)
                error("label on INCLUDE line");
            t = lex();
            if (t != T_STRING && t != T_ASCII)
                error("filename expected");
            include(yylval.string);
            break;

        case D_IDNT :
            puttemp('\n');
            t = lex();
            if (t != T_STRING)
                error("IDNT requires a name in quotes");
            program_unit_name = allocstr(yylval.string);
            strcpy(program_unit_name, yylval.string);
            break;

        case D_MASK2 :
            puttemp('\n');
            warning("what the hell are you using MASK2 for ?!");
            break;

        case D_CURRENT_FILE :
            if (lex() != T_STRING)
                error("file name expected");
            strcpy(current_filename, yylval.string);
            if (lex() != ',')
                error("comma expected");
            if (lex() != T_NUMBER)
                error("line number expected");
            current_lineno = yylval.number;
            break;

        case D_PRINT :
            error_line();
            break;

        case D_UNIMP :
            puttemp('\n');
            warning("directive unimplemented");
            break;

        case D_NOTHING :
            puttemp('\n');
            break;

        default :
            error("%s unexpected", yytext);
        }
    }
}

