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
#include <string.h>
#include "symbols.h"
#include "expr.h"
#include "ea.h"
#include "tokens.h"
#include "getline.h"
#include "errors.h"
#include "parse.h"

extern char *yytext;
extern struct ea args[];
extern inttype n_args;
extern inttype movem_flag; /* set when parsing movem instructions */
extern struct symbol star;
extern inttype pass;

YYSTYPE yylval;
inttype nexttoken;
struct ea *ap;

// functions from lexan.l
extern int lex();
extern void unlex();
extern void lex_unput(int);

# define isrel(e)	(e.sym && (e.sym->type==S_RELOC || e.sym->type==S_XREF \
	|| (pass==2 && e.sym->type==S_UNDF)))

static void check(int token)
{
    if (token != nexttoken)
        error("%s unexpected", yytext);
}

void match(int token)
{
    check(token);
    nexttoken = lex();
}

static inttype reg()
{
    inttype t;
    if (nexttoken == T_DREG)
    {
        t = yylval.number;
        match(T_DREG);
        return t;
    } else {
        t = 8+yylval.number;
        match(T_AREG);
        return t;
    }
}

static inttype sizewl()
{
    inttype t;
    if (nexttoken != T_SIZE) return 0;
    t = yylval.number;
    match(T_SIZE);
    if (t == 0)
        error("bad size");
    return t-1;
}

void parse_args()
{
    n_args = 0;
    if (!*operand_ptr) return;	/* is there anything to parse? */
    nexttoken = ',';	/* bodge */
    ap = args;
    while (nexttoken == ',')
    {
        match(',');
        ea();
    }
}

void ea()
{
    switch ((int)nexttoken)
    {
    case T_DREG :
        if (movem_flag) {
            ap->mode = M_PC;
            ap->reg = M_IMMED;
            ap->ext.value = register_mask();
            ap->ext.sym = 0;
        } else {
            ap->mode = M_DREG;
            ap->reg = yylval.number;
            match(T_DREG);
        }
        break;

    case T_AREG :
        if (movem_flag) {
            ap->mode = M_PC;
            ap->reg = M_IMMED;
            ap->ext.value = register_mask();
            ap->ext.sym = 0;
        } else {
            ap->mode = M_AREG;
            ap->reg = yylval.number;
            match(T_AREG);
        }
        break;

	/* check for REG'ed register masks */
    case T_LABEL :
        if (yylval.symbol && yylval.symbol->type==S_REG) {
            ap->mode = M_PC;
            ap->reg = M_IMMED;
            ap->ext.value = yylval.symbol->svalue;
            ap->ext.sym = 0;
            match(T_LABEL);
            break;
        }
        /* must be an expression */
        goto expr_ea;

    case '(' :
        match('(');
        if (nexttoken == T_AREG) {
            ap->reg = yylval.number;
            match(T_AREG);
            match(')');
            if (nexttoken == '+') {
                match('+');
                ap->mode = M_POSTINC;
            } else
                ap->mode = M_AREG_IND;
        } else {
            unlex();		/* unlex the last token */
            nexttoken = '(';
            goto expr_ea;
        }
        break;

    case '-' :
        /* note have to catch things like -8(A0) here */
        match('-');
        if (nexttoken == '(') {
            match('(');
            if (nexttoken != T_AREG) {	/* must be an expression */
                unlex();
                lex_unput('(');
                nexttoken = '-';
                goto expr_ea;
            }
        } else {	/* next token isn't '(' so must be an expr */
            unlex();
            nexttoken = '-';
            goto expr_ea;
        }
        ap->mode = M_PREDEC;
        ap->reg = yylval.number;
        match(T_AREG);
        match(')');
        break;

    case '#' :
        ap->mode = M_PC;
        ap->reg = M_IMMED;
        match('#');
        ap->ext = *expr();
        break;

    case T_CCR :
        match(T_CCR);
        ap->mode = M_CCR;
        break;

    case T_SR :
        match(T_SR);
        ap->mode = M_SR;
        break;

    case T_USP :
        match(T_USP);
        ap->mode = M_USP;
        break;

    case T_CONTROL :
        ap->mode = M_CONTROL;
        ap->reg = yylval.number;
        match(T_CONTROL);
        break;

    default :
    expr_ea:				/* effective addresses starting with expr */
        ap->ext = *expr();
        if (nexttoken == '(') {
            match('(');
            if (nexttoken == T_AREG) {
                ap->reg = yylval.number;
                match(T_AREG);
                if (nexttoken == ',') {
                    inttype offset = ap->ext.value;
                    if (offset<-128 || offset>127)
                        error("byte range error");
                    match(',');
                    ap->ext.value = reg()<<12;
                    ap->ext.value += sizewl()<<11;
                    ap->ext.value += offset&0xff;
                    ap->mode = M_AREG_OFF_IND;
                } else
                    ap->mode = M_AREG_OFF;
                match(')');
            } else {
                match(T_PC);
                ap->mode = M_PC;
                if (nexttoken == ',') {
                    match(',');
                    if (ap->ext.sym && ap->ext.sym->type == S_RELOC) {
                        ap->ext.value -= (current_section->pc + 2);
                        ap->ext.sym = 0;	/* not an address anymore */
                        if (ap->ext.value < -128 || ap->ext.value > 127)
                            error("PC relative reference too far away");
                    }
                    ap->ext.value &= 0xff;		/* byte offset */
                    ap->ext.value += reg()<<12;
                    ap->ext.value += sizewl()<<11;
                    ap->reg = M_PCREL_IND;
                } else {
                    if (ap->ext.sym && ap->ext.sym->type == S_RELOC) {
                        ap->ext.value -= (current_section->pc + 2);
                        ap->ext.sym = 0;
                        if (ap->ext.value < -32768 ||
                            ap->ext.value > 32767)
                            error("PC relative reference too far away");
                    }
                    ap->reg = M_PCREL;
                }
                match(')');
            }
        } else {
            ap->mode = M_PC;
            ap->reg = M_ABS32;
        }
        break;
    }
    ap++; n_args++;

    /* check for Dh:Dl in mul/div instructions */
    if (ap[-1].mode == M_DREG && nexttoken==':') {
        match(':');
        ap->mode = M_DREG;
        ap->reg = yylval.number;
        match(T_DREG);
        ap++; n_args++;
    }

    /* check for bitfield after the effective address */
    if (nexttoken == '[') {
        match('[');
        ap->mode = M_BITFIELD;
        ap->ext.sym = 0;
        if (nexttoken == T_DREG) {
            ap->ext.value = yylval.number + 32;
            match(T_DREG);
        } else {
            ap->ext = *expr();	/* parse an expression */
            if (ap->ext.sym)
                error("Bitfield offset must be absolute");
            if (ap->ext.value < 0 || ap->ext.value > 31)
                error("Bitfield offset out of range");
        }
        ap++; n_args++;
        match(':');
        ap->mode = M_BITFIELD;
        if (nexttoken == T_DREG) {
            ap->ext.value = yylval.number + 32;
            match(T_DREG);
        } else {
            ap->ext = *expr();	/* parse an expression */
            if (ap->ext.sym)
                error("Bitfield width must be absolute");
            if (ap->ext.value < 0 || ap->ext.value > 31)
                error("Bitfield width out of range");
        }
        ap++; n_args++;
        match(']');
    }
}

struct expr* parse_expr()
{
    nexttoken = lex();
    return expr();
}

/* expr = term ( op term )* */
struct expr* expr()
{
    struct expr t, u;
    static struct expr result;
    t = *term();
    for (;;) {
        switch ((int)nexttoken) {
        case '+' :
            match('+');
            u = *term();
            if (isrel(t)) {
                if (isrel(u))
                    if (pass==2)	/* only error in second pass */
                        error(
                              "at least one operand of plus must be absolute");
                t.value += u.value;
                break;
            } else {
                u.value += t.value;
                t = u;
                break;
            }
        case '-' :
            match('-');
            u = *term();
            if (isrel(t))
                if (isrel(u)) {
                    t.value -= u.value;
                    t.sym = 0;		/* R - R is an absolute */
                    break;
                } else {
                    t.value -= u.value;
                    break;
                }
            else {
                if (isrel(u))
                    if (pass==2)
                        error("bad operands to minus");
                t.value -= u.value;
                break;
            }
        default :
            result = t;
            return &result;
        }
    }
}

/* term = factor ( op factor )* */
struct expr* term()
{
    struct expr t, u;
    static struct expr result;
    t = *factor();
    for (;;) {
        switch ((int)nexttoken) {
        case '*' :
            match('*');
            u = *factor();
            if (isrel(t) || isrel(u))
                if (pass==2)
                    error("multiply must have absolute operands");
            t.value *= u.value;
            break;
        case '/' :
            match('/');
            u = *factor();
            if (isrel(t) || isrel(u))
                if (pass==2)
                    error("divide must have absolute operands");
            t.value /= u.value;
            break;
        case T_LSHIFT :
            match(T_LSHIFT);
            u = *factor();
            if (isrel(t) || isrel(u))
                if (pass==2)
                    error("left shift must have absolute operands");
            t.value <<= u.value;
            break;
        case T_RSHIFT :
            match(T_RSHIFT);
            u = *factor();
            if (isrel(t) || isrel(u))
                if (pass==2)
                    error("right shift must have absolute operands");
            t.value >>= u.value;
            break;
        case '&' :
            match('&');
            u = *factor();
            if (isrel(t) || isrel(u))
                if (pass==2)
                    error("AND must have absolute operands");
            t.value &= u.value;
            break;
        case '!' :
            match('!');
            goto or;
        case '|' :		/* '!' for compatibility */
            match('|');
        or:			u = *factor();
            if (isrel(t) || isrel(u))
                if (pass==2)
                    error("OR must have absolute operands");
            t.value |= u.value;
            break;
        default :
            result = t;
            return &result;
        }
    }
}

/* factor = uop factor
   | '(' expr ')'
   | number
   | symbol
*/
struct expr* factor()
{
    inttype n;
    char *p;
    static struct expr t;

    switch ((int)nexttoken) {
    case '-' :
        match('-');
        t = *factor();
        if (isrel(t))
            error("cannot negate a relocateable value");
        t.value = -t.value;
        return &t;
    case '~' :
        match('~');
        t = *factor();
        if (isrel(t))
            error("cannot complement a relocateable value");
        t.value = ~t.value;
        return &t;
    case '(' :
        match('(');
        t = *expr();
        match(')');
        return &t;
    case T_LABEL :
        if (!yylval.symbol)	/* undefined ? */
            yylval.symbol = add_symbol(yytext);
        switch (yylval.symbol->type) {
        case S_RELOC :
        case S_XREF :
        case S_UNDF :
            t.value = yylval.symbol->svalue;
            t.sym = yylval.symbol;
            match(T_LABEL);
            break;
        case S_ABS :
            t.value = yylval.symbol->svalue;
            t.sym = 0;
            match(T_LABEL);
            break;
        case S_NOTHING :
            /* this should catch forward refs to EQUR'ed syms in
             * the second pass */
            error("forward reference to EQUR'ed symbol");
            /*NOTREACHED*/
        default :
            /* i hope this should never happen */
            fatal_error("weird symbol in expr type=%d",
                        yylval.symbol->type);
            /*NOTREACHED*/
        }
        if (t.sym&&t.sym->othersym) t.sym = t.sym->othersym;
        return &t;
    case T_NUMBER :
        t.value = yylval.number;
        match(T_NUMBER);
        t.sym = 0;
        return &t;
    case '*' :
        t.value = star.svalue;
        t.sym = &star;
        match('*');
        return &t;
    case T_ASCII :
        if (strlen(yylval.string) > 4)
            error("ascii literal too long");
        n = 0;
        for (p=yylval.string; *p; p++)
            n = (n<<8) + *p;
        match(T_ASCII);
        t.value = n;
        t.sym = 0;
        return &t;
    default :
        error("error in expression");
    }
    
    /*NOTREACHED*/
    return 0;
}

static inttype register_range()
{
    inttype r1,r2;
    inttype t;

    r1 = reg();
    if (nexttoken == '-') {
        match('-');
        r2 = reg();
    } else
        r2 = r1;
    if (r2 < r1) {
        t = r1;
        r1 = r2;
        r2 = t;
    }
    /* calculate mask */
    return ((1<<(r2+1))-1) & ~((1<<r1)-1);
}

/* (R1(-R2)?)(/(R1(-R2)?))* */
/* returns the mask with d0=lsb, a7=msb */
inttype register_mask()
{
    inttype mask=0;

    mask = register_range();
    while (nexttoken == '/') {
        match('/');
        mask |= register_range();
    }
    return mask;
}

