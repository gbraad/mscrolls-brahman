/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2016-2018 Voidware Ltd.  All Rights Reserved.
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for historical reference
 * and educational purposes.
 */

#include "commands.h"
#include "y.tab.h"

struct command commands[] = {
	"examine", "x", EXAMINE, "examine register contents",
	"examinev", "xv", EXAMINEV, "verbosely examine register contents",
	"step", "s", STEP, "single step one instruction",
	"next", "n", NEXT, "single step, but don't follow calls",
	"call", "cl", CALL, "call routine",
	"follow", "f", NEXT, "see next",
	"dump", "d", DUMP, "dump memory",
	"go", "g", GO, "execute with breakpoints",
	"cont", "c", GO, "same as go",
	"fastgo", "fg", FASTGO, "execute without breakpoints (faster)",
	"print", "p", PRINT, "print value of expression",
	"printx", "px", PRINTX, "print value of expression in hex",
	"printd", "pd", PRINTD, "print value of expression in decimal",
	"reload", "rl", RELOAD, "reload 68k program",
	"modifyb", "mb", MODIFYB, "modify memory byte",
	"modifyw", "mw", MODIFYW, "modify memory word",
	"modifyl", "ml", MODIFYL, "modify memory long",
	"list", "l", LIST, "disassemble",
	"listp", "lp", LISTV, "disassemble, printing PC instead of ascii",
	"base", "base", BASE, "change current number base",
	"break", "b", BREAK, "set/list breakpoints",
	"delete", "dl", DELETE, "delete breakpoint",
	"breakm", "bm", BREAKM, "set/list memory breakpoints",
	"deletem", "dlm", DELETEM, "delete memory breakpoint",
	"quit", "q", QUIT, "quit",
	"log", "lg", LOG, "start logging",
	"longlog", "llg", LONGLOG, "start long logging",
	"endlog", "elg", ENDLOG, "end logging",
	"exec", "ex", EXEC, "exec a file of adventure commands",
	"more", "mo", MORE, "toggle moreing",
	"warnings", "warn", WARNINGS, "toggle warnings",
	"help", "?", HELP, "print this message",
	0,0,0,0
};
