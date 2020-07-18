/**
 *
 *    _    __        _      __
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#pragma once

#define hunk_unit 0x3E7
#define hunk_name 0x3E8
#define hunk_code 0x3E9
#define hunk_data 0x3EA
#define hunk_bss 0x3EB
#define hunk_reloc32 0x3EC
#define hunk_reloc16 0x3EC
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
#define hunk_symbol 0x3F0
#define hunk_debug 0x3F1
#define hunk_end 0x3F2
#define hunk_header 0x3F3
#define hunk_overlay 0x3F5
#define hunk_break 0x3F6
#define hunk_
