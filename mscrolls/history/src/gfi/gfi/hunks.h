/*************************************************************************
 *
 *	@(#)hunks.h	1.1
 *	8/5/87
 *
 *************************************************************************/

#define hunk_unit 0x3E7
#define hunk_name 0x3E8
#define hunk_code 0x3E9
#define hunk_data 0x3EA
#define hunk_bss 0x3EB
#define hunk_reloc32 0x3EC
#define hunk_reloc16 0x3ED
#define hunk_reloc16An 0x200	/* needed for incremental linking */
#define hunk_reloc8 0x3EE
#define hunk_ext 0x3EF
#define ext_symb 0
#define ext_def 1
#define ext_abs 2
#define ext_res 3
#define ext_ref32 129
#define ext_common 130
#define ext_ref16 131
#define ext_ref8 132
#define ext_ref16An 133
#define hunk_symbol 0x3F0
#define hunk_debug 0x3F1
#define hunk_end 0x3F2
#define hunk_header 0x3F3
#define hunk_overlay 0x3F5
#define hunk_break 0x3F6
