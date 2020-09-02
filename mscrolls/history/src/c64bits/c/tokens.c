char *tokens[] = {
	"END",		/* 0x80 */
	"FOR",		/* 0x81 */
	"NEXT",		/* 0x82 */
	"DATA",		/* 0x83 */
	"INPUT#",	/* 0x84 */
	"INPUT",	/* 0x85 */
	"DIM",		/* 0x86 */
	"READ",		/* 0x87 */
	"LET",		/* 0x88 */
	"GOTO",		/* 0x89 */
	"RUN",		/* 0x8A */
	"IF",		/* 0x8B */
	"RESTORE",	/* 0x8C */
	"GOSUB",	/* 0x8D */
	"RETURN",	/* 0x8E */
	"REM",		/* 0x8F */
	"STOP",		/* 0x90 */
	"ON",		/* 0x91 */
	"WAIT",		/* 0x92 */
	"LOAD",		/* 0x93 */
	"SAVE",		/* 0x94 */
	"VERIFY",	/* 0x95 */
	"DEF",		/* 0x96 */
	"POKE",		/* 0x97 */
	"PRINT#",	/* 0x98 */
	"PRINT",	/* 0x99 */
	"CONT",		/* 0x9A */
	"LIST",		/* 0x9B */
	"CLR",		/* 0x9C */
	"CMD",		/* 0x9D */
	"SYS",		/* 0x9E */
	"OPEN",		/* 0x9F */
	"CLOSE",	/* 0xA0 */
	"GET",		/* 0xA1 */
	"NEW",		/* 0xA2 */
	"TAB(",		/* 0xA3 */
	"TO",		/* 0xA4 */
	"FN",		/* 0xA5 */
	"SPC(",		/* 0xA6 */
	"THEN",		/* 0xA7 */
	"NOT",		/* 0xA8 */
	"STEP",		/* 0xA9 */
	"+",		/* 0xAA */
	"-",		/* 0xAB */
	"*",		/* 0xAC */
	"/",		/* 0xAD */
	"^",		/* 0xAE */
	"AND",		/* 0xAF */
	"OR",		/* 0xB0 */
	">",		/* 0xB1 */
	"=",		/* 0xB2 */
	"<",		/* 0xB3 */
	"SGN",		/* 0xB4 */
	"INT",		/* 0xB5 */
	"ABS",		/* 0xB6 */
	"USR",		/* 0xB7 */
	"FRE",		/* 0xB8 */
	"POS",		/* 0xB9 */
	"SQR",		/* 0xBA */
	"RND",		/* 0xBB */
	"LOG",		/* 0xBC */
	"EXP",		/* 0xBD */
	"COS",		/* 0xBE */
	"SIN",		/* 0xBF */
	"TAN",		/* 0xC0 */
	"ATN",		/* 0xC1 */
	"PEEK",		/* 0xC2 */
	"LEN",		/* 0xC3 */
	"STR$",		/* 0xC4 */
	"VAL",		/* 0xC5 */
	"ASC",		/* 0xC6 */
	"CHR$",		/* 0xC7 */
	"LEFT$",	/* 0xC8 */
	"RIGHT$",	/* 0xC9 */
	"MID$",		/* 0xCA */
	"GO",		/* 0xCB */
	"RGR",		/* 0xCC */
	"RCLR",		/* 0xCD */
	"WEIRD",	/* 0xCE */
	"JOY",		/* 0xCF */
	"RDOT",		/* 0xD0 */
	"DEC",		/* 0xD1 */
	"HEX$",		/* 0xD2 */
	"ERR$",		/* 0xD3 */
	"INSTR",	/* 0xD4 */
	"ELSE",		/* 0xD5 */
	"RESUME",	/* 0xD6 */
	"TRAP",		/* 0xD7 */
	"TRON",		/* 0xD8 */
	"TROFF",	/* 0xD9 */
	"SOUND",	/* 0xDA */
	"VOL",		/* 0xDB */
	"AUTO",		/* 0xDC */
	"PUDEF",	/* 0xDD */
	"GRAPHIC",	/* 0xDE */
	"PAINT",	/* 0xDF */
	"CHAR",		/* 0xE0 */
	"BOX",		/* 0xE1 */
	"CIRCLE",	/* 0xE2 */
	"GSHAPE",	/* 0xE3 */
	"SSHAPE",	/* 0xE4 */
	"DRAW",		/* 0xE5 */
	"LOCATE",	/* 0xE6 */
	"COLOR",	/* 0xE7 */
	"SCNCLR",	/* 0xE8 */
	"SCALE",	/* 0xE9 */
	"HELP",		/* 0xEA */
	"DO",		/* 0xEB */
	"LOOP",		/* 0xEC */
	"EXIT",		/* 0xED */
	"DIRECTORY",	/* 0xEE */
	"DSAVE",	/* 0xEF */
	"DLOAD",	/* 0xF0 */
	"HEADER",	/* 0xF1 */
	"SCRATCH",	/* 0xF2 */
	"COLLECT",	/* 0xF3 */
	"COPY",		/* 0xF4 */
	"RENAME",	/* 0xF5 */
	"BACKUP",	/* 0xF6 */
	"DELETE",	/* 0xF7 */
	"RENUMBER",	/* 0xF8 */
	"KEY",		/* 0xF9 */
	"MONITOR",	/* 0xFA */
	"USING",	/* 0xFB */
	"UNTIL",	/* 0xFC */
	"WHILE",	/* 0xFD */
	"WEIRD2",	/* 0xFE */
};

char *tokensCE[] = {
	"",			/* 0x00 */
	"",			/* 0x01 */
	"POT",		/* 0x02 */
	"BUMP",		/* 0x03 */
	"PEN",		/* 0x04 */
	"RSPPOS",	/* 0x05 */
	"RSPSPRITE",	/* 0x06 */
	"RSPCOLOR",	/* 0x07 */
	"XOR",		/* 0x08 */
	"RWINDOW",	/* 0x09 */
	"POINTER",	/* 0x0A */
};

char *tokensFE[] = {
	"",			/* 0x00 */
	"",			/* 0x01 */
	"BANK",		/* 0x02 */
	"FILTER",	/* 0x03 */
	"PLAY",		/* 0x04 */
	"TEMPO",	/* 0x05 */
	"MOVSPR",	/* 0x06 */
	"SPRITE",	/* 0x07 */
	"SPRCOLOR",	/* 0x08 */
	"RREG",		/* 0x09 */
	"ENVELOPE",	/* 0x0a */
	"SLEEP",	/* 0x0b */
	"CATALOG",	/* 0x0c */
	"DOPEN",	/* 0x0d */
	"APPEND",	/* 0x0e */
	"DCLOSE",	/* 0x0f */
	"BSAVE",	/* 0x10 */
	"BLOAD",	/* 0x11 */
	"RECORD",	/* 0x12 */
	"CONCAT",	/* 0x13 */
	"DVERIFY",	/* 0x14 */
	"DCLEAR",	/* 0x15 */
	"SPRSAV",	/* 0x16 */
	"COLLISION",/* 0x17 */
	"BEGIN",	/* 0x18 */
	"BEND",		/* 0x19 */
	"WINDOW",	/* 0x1a */
	"BOOT",		/* 0x1b */
	"WIDTH",	/* 0x1c */
	"SPRDEF",	/* 0x1d */
	"QUIT",		/* 0x1e */
	"STASH",	/* 0x1f */
	"",			/* 0x20 */
	"FETCH",	/* 0x21 */
	"",			/* 0x22 */
	"SWAP",		/* 0x23 */
	"OFF",		/* 0x24 */
	"FAST",		/* 0x25 */
	"SLOW",		/* 0x26 */
};