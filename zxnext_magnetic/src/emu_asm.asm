;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Stefan Bylund 2021
;;
;; This module contains functions originally from emu.c that have
;; been compiled to Z80 assembly and then manually optimized.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE "zconfig.inc"

SECTION code_user

PUBLIC _page_in_rom
PUBLIC _page_in_game
PUBLIC _effective
PUBLIC _effective_string
IF !USE_PROG_FORMAT
PUBLIC _effective_string2
PUBLIC _effective_string3
ENDIF
PUBLIC _effective_dict
PUBLIC _addr_to_page
PUBLIC _save_arg2
PUBLIC _seed_r
PUBLIC _reg_align
PUBLIC _read_reg_w
PUBLIC _read_reg_l
PUBLIC _read_dreg_b
PUBLIC _read_dreg_w
PUBLIC _read_dreg_l
PUBLIC _read_areg_w
PUBLIC _read_areg_l
PUBLIC _write_dreg_b
PUBLIC _write_dreg_w
PUBLIC _write_dreg_l
PUBLIC _write_areg_w
PUBLIC _write_areg_l
PUBLIC _inc_dreg_w
PUBLIC _dec_dreg_w
PUBLIC _inc_areg_w
PUBLIC _inc_var_areg_w
PUBLIC _set_info
PUBLIC _read_word
PUBLIC _set_arg1
PUBLIC _set_arg2_nosize
PUBLIC _set_arg2
PUBLIC _swap_args
PUBLIC _push
PUBLIC _pop
PUBLIC _get_arg
PUBLIC _set_flags
PUBLIC _condition
PUBLIC _branch
PUBLIC _do_add
PUBLIC _do_adda
PUBLIC _do_sub
PUBLIC _do_suba
PUBLIC _do_eor
PUBLIC _do_and
PUBLIC _do_or
PUBLIC _do_cmp
PUBLIC _do_move_b
PUBLIC _do_move_w
PUBLIC _do_move_l
PUBLIC _do_btst
PUBLIC _do_bop
PUBLIC _check_btst
PUBLIC _check_lea
PUBLIC _check_movem
PUBLIC _check_movem2

EXTERN _ms_fatal
IF LOGEMU
EXTERN _out
ENDIF

EXTERN _dreg
EXTERN _areg
EXTERN _pc
EXTERN _arg1i
EXTERN _zflag
EXTERN _nflag
EXTERN _cflag
EXTERN _vflag
EXTERN _byte1
EXTERN _byte2
EXTERN _regnr
EXTERN _admode
EXTERN _opsize
EXTERN _arg1
EXTERN _arg2
EXTERN _arg2_buf
EXTERN _is_reversible
EXTERN _tmparg
EXTERN _version
EXTERN _quick_flag
EXTERN _current_page
EXTERN _code_base_page
EXTERN _string_base_page
IF !USE_PROG_FORMAT
EXTERN _string2_base_page
EXTERN _string3_base_page
ENDIF
EXTERN _dict_base_page

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _PAGE_IN_ROM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_page_in_rom:

   ; void page_in_rom(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af

   ld a,255
; ZXN_WRITE_MMU0(255);
   mmu0 a
; ZXN_WRITE_MMU1(255);
   mmu1 a
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _PAGE_IN_GAME
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_page_in_game:

   ; void page_in_game(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af

   ld a,(_current_page)
; ZXN_WRITE_MMU0(current_page);
   mmu0 a
; ZXN_WRITE_MMU1(current_page + 1);
   inc a
   mmu1 a
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _EFFECTIVE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_effective:

   ; type8 *effective(type32 ptr) __z88dk_fastcall;
   ;
   ; enter : dehl = virtual pointer
   ; exit  : hl = effective pointer
   ; uses  : af, bc, de, hl

IF !USE_PROG_FORMAT && !USE_MAGNETIC_WINDOWS
; ptr &= 0xFFFF;
   ld de,0x0000
ENDIF

; type8 page = addr_to_page(ptr);
   ld c,l
   ld b,h                      ; save bc = hl
   ; dehl = address
   call _addr_to_page          ; l = page

; type16 addr = (type16) (ptr % 0x2000);
   ld a,b
   and a,0x1F
   ld b,a                      ; bc = addr = (type16) (ptr % 0x2000)

; type8 new_page = code_base_page + page;
   ld a,l
   ld hl,_code_base_page
   add a,(hl)
   ld e,a                      ; e = new_page = code_base_page + page

; if (current_page != new_page)
   ld a,(_current_page)
   cp a,e
   jr z,effective_end

; current_page = new_page;
   ld a,e
   ld (_current_page),a        ; current_page = a = new_page

; ZXN_WRITE_MMU0(current_page);
   mmu0 a

; ZXN_WRITE_MMU1(current_page + 1);
   inc a
   mmu1 a
; end-if

effective_end:
; return (type8 *) addr;
   ld l,c
   ld h,b
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _EFFECTIVE_STRING
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_effective_string:

   ; type8 *effective_string(type32 ptr) __z88dk_fastcall;
   ;
   ; enter : dehl = virtual pointer
   ; exit  : hl = effective pointer
   ; uses  : af, bc, de, hl

; type8 page = addr_to_page(ptr);
   ld c,l
   ld b,h                      ; save bc = hl
   ; dehl = address
   call _addr_to_page          ; l = page

; type16 addr = (type16) (ptr % 0x2000);
   ld a,b
   and a,0x1F
   ld b,a                      ; bc = addr = (type16) (ptr % 0x2000)

; type8 new_page = string_base_page + page;
   ld a,l
   ld hl,_string_base_page
   add a,(hl)
   ld e,a                      ; e = new_page = string_base_page + page

; if (current_page != new_page)
   ld a,(_current_page)
   cp a,e
   jr z,effective_string_end

; current_page = new_page;
   ld a,e
   ld (_current_page),a        ; current_page = a = new_page

; ZXN_WRITE_MMU0(current_page);
   mmu0 a

; ZXN_WRITE_MMU1(current_page + 1);
   inc a
   mmu1 a
; end-if

effective_string_end:
; return (type8 *) addr;
   ld l,c
   ld h,b
   ret

IF !USE_PROG_FORMAT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _EFFECTIVE_STRING2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_effective_string2:

   ; type8 *effective_string2(type32 ptr) __z88dk_fastcall;
   ;
   ; enter : dehl = virtual pointer
   ; exit  : hl = effective pointer
   ; uses  : af, bc, de, hl

; type8 page = addr_to_page(ptr);
   ld c,l
   ld b,h                      ; save bc = hl
   ; dehl = address
   call _addr_to_page          ; l = page

; type16 addr = (type16) (ptr % 0x2000);
   ld a,b
   and a,0x1F
   ld b,a                      ; bc = addr = (type16) (ptr % 0x2000)

; type8 new_page = string2_base_page + page;
   ld a,l
   ld hl,_string2_base_page
   add a,(hl)
   ld e,a                      ; e = new_page = string2_base_page + page

; if (current_page != new_page)
   ld a,(_current_page)
   cp a,e
   jr z,effective_string2_end

; current_page = new_page;
   ld a,e
   ld (_current_page),a        ; current_page = a = new_page

; ZXN_WRITE_MMU0(current_page);
   mmu0 a

; ZXN_WRITE_MMU1(current_page + 1);
   inc a
   mmu1 a
; end-if

effective_string2_end:
; return (type8 *) addr;
   ld l,c
   ld h,b
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _EFFECTIVE_STRING3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_effective_string3:

   ; type8 *effective_string3(type32 ptr) __z88dk_fastcall;
   ;
   ; enter : dehl = virtual pointer
   ; exit  : hl = effective pointer
   ; uses  : af, bc, de, hl

; type8 page = addr_to_page(ptr);
   ld c,l
   ld b,h                      ; save bc = hl
   ; dehl = address
   call _addr_to_page          ; l = page

; type16 addr = (type16) (ptr % 0x2000);
   ld a,b
   and a,0x1F
   ld b,a                      ; bc = addr = (type16) (ptr % 0x2000)

; type8 new_page = string3_base_page + page;
   ld a,l
   ld hl,_string3_base_page
   add a,(hl)
   ld e,a                      ; e = new_page = string3_base_page + page

; if (current_page != new_page)
   ld a,(_current_page)
   cp a,e
   jr z,effective_string3_end

; current_page = new_page;
   ld a,e
   ld (_current_page),a        ; current_page = a = new_page

; ZXN_WRITE_MMU0(current_page);
   mmu0 a

; ZXN_WRITE_MMU1(current_page + 1);
   inc a
   mmu1 a
; end-if

effective_string3_end:
; return (type8 *) addr;
   ld l,c
   ld h,b
   ret

ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _EFFECTIVE_DICT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_effective_dict:

   ; type8 *effective_dict(type32 ptr) __z88dk_fastcall;
   ;
   ; enter : dehl = virtual pointer
   ; exit  : hl = effective pointer
   ; uses  : af, bc, de, hl

; type8 page = addr_to_page(ptr);
   ld c,l
   ld b,h                      ; save bc = hl
   ; dehl = address
   call _addr_to_page          ; l = page

; type16 addr = (type16) (ptr % 0x2000);
   ld a,b
   and a,0x1F
   ld b,a                      ; bc = addr = (type16) (ptr % 0x2000)

; type8 new_page = dict_base_page + page;
   ld a,l
   ld hl,_dict_base_page
   add a,(hl)
   ld e,a                      ; e = new_page = dict_base_page + page

; if (current_page != new_page)
   ld a,(_current_page)
   cp a,e
   jr z,effective_dict_end

; current_page = new_page;
   ld a,e
   ld (_current_page),a        ; current_page = a = new_page

; ZXN_WRITE_MMU0(current_page);
   mmu0 a

; ZXN_WRITE_MMU1(current_page + 1);
   inc a
   mmu1 a
; end-if

effective_dict_end:
; return (type8 *) addr;
   ld l,c
   ld h,b
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _ADDR_TO_PAGE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_addr_to_page:

   ; type8 addr_to_page(type32 addr) __z88dk_fastcall;
   ;
   ; Note: The code is taken from zxn_page_from_addr() in z88dk but with the
   ; parameter checking removed.
   ;
   ; enter : dehl = address (only the 21 low bits are considered = 2 MB)
   ; exit  : l = page number 0-255 (256 slots = 2 MB)
   ; uses  : af, hl

   ld a,e                      ; ahl = address

   rl h
   rla
   rl h
   rla
   rl h
   rla

   ld l,a
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SAVE_ARG2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_save_arg2:

   ; void save_arg2(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : bc, de, hl

; memcpy(arg2_buf, arg2, sizeof(arg2_buf));
   ld de,_arg2_buf
   ld hl,(_arg2)
   ldi
   ldi
   ldi
   ldi

; arg2 = arg2_buf;
   ld hl,_arg2_buf
   ld (_arg2),hl

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SEED_R
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_seed_r:

   ; type16 seed_r(void);
   ;
   ; enter : none
   ; exit  : hl = seed
   ; uses  : af, hl

   ld a,r
   ld l,a
   cpl
   ld h,a

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _REG_ALIGN
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_reg_align:

   ; type8 *reg_align(type8 *ptr, type8 size) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : hl = size-aligned register pointer
   ; uses  : af, de, hl

   pop de                      ; return address
   pop hl                      ; hl = ptr
   dec sp
   pop af                      ; a = size
   push de                     ; restore return address

asm_reg_align:

   ; asm_reg_align
   ;
   ; enter : hl = register pointer,
   ;          a = size
   ; exit  : hl = size-aligned register pointer
   ; uses  : af, hl

; if (size == 1)
   cp a,1
   jr nz,reg_align_else
; p += 2;
   inc hl
   inc hl
   jr reg_align_endif
reg_align_else:
; else if (size == 0)
   or a,a
   jr nz,reg_align_endif
; p += 3;
   inc hl
   inc hl
   inc hl
reg_align_endif:

; return p;
   ; hl = size-aligned register pointer
   ret

;;
;; Optimization:
;; The read_reg() and write_reg() functions have been split into several
;; functions per register type and size for improved performance.
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_REG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_reg_w:

   ; type16 read_reg_w(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = register number
   ; exit  : hl = register word value
   ; uses  : af, hl

; if (i < 8)
   ld a,l                      ; a = register number
   cp a,8
   jr nc,read_reg_w_else
; ptr = (type8 *) &dreg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]
   jr read_reg_w_endif
; else
read_reg_w_else:
; ptr = (type8 *) &areg[i - 8];
   sub a,8
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[i - 8]
; end-if
read_reg_w_endif:

; return read_w(ptr + 2);
   inc hl
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; hl = endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_REG_L
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_reg_l:

   ; type32 read_reg_l(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = register number
   ; exit  : dehl = register longword value
   ; uses  : af, de, hl

; if (i < 8)
   ld a,l                      ; a = register number
   cp a,8
   jr nc,read_reg_l_else
; ptr = (type8 *) &dreg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]
   jr read_reg_l_endif
; else
read_reg_l_else:
; ptr = (type8 *) &areg[i - 8];
   sub a,8
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[i - 8]
; end-if
read_reg_l_endif:

; return read_l(ptr);
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_DREG_B
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_dreg_b:

   ; type8 read_dreg_b(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = data register number
   ; exit  : l = data register byte value
   ; uses  : af, hl

; type8 *ptr = (type8 *) &dreg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]

; return ptr[3];
   inc hl
   inc hl
   inc hl
   ld l,(hl)                   ; l = register byte value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_DREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_dreg_w:

   ; type16 read_dreg_w(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = data register number
   ; exit  : hl = data register word value
   ; uses  : af, hl

; type8 *ptr = (type8 *) &dreg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]

; return read_w(ptr + 2);
   inc hl
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; hl = endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_DREG_L
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_dreg_l:

   ; type32 read_dreg_l(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = data register number
   ; exit  : dehl = data register longword value
   ; uses  : af, de, hl

; type8 *ptr = (type8 *) &dreg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]

; return read_l(ptr);
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_AREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_areg_w:

   ; type16 read_areg_w(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = address register number
   ; exit  : hl = address register word value
   ; uses  : af, hl

; type8 *ptr = (type8 *) &areg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[i]

; return read_w(ptr + 2);
   inc hl
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; hl = endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_AREG_L
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_areg_l:

   ; type32 read_areg_l(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = address register number
   ; exit  : dehl = address register longword value
   ; uses  : af, de, hl

; type8 *ptr = (type8 *) &areg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[i]

; return read_l(ptr);
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _WRITE_DREG_B
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_write_dreg_b:

   ; void write_dreg_b(type8 i, type8 val) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   pop de                      ; e = register number, d = register value
   push hl                     ; restore return address

; type8 *ptr = (type8 *) &dreg[i];
   ld a,e                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]

; ptr[3] = val;
   inc hl
   inc hl
   inc hl
   ld (hl),d
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _WRITE_DREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_write_dreg_w:

   ; void write_dreg_w(type8 i, type16 val) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   dec sp
   pop af                      ; a = register number
   pop de                      ; de = register value
   push hl                     ; restore return address

; type8 *ptr = (type8 *) &dreg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]

; write_w(ptr + 2, val);
   inc hl
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; stored endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _WRITE_DREG_L
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_write_dreg_l:

   ; void write_dreg_l(type8 i, type32 val) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   dec sp
   pop af                      ; a = register number
   pop de                      ; de = low 16 bits of register value
   ex (sp),hl                  ; hl = high 16 bits of register value

   ex de,hl                    ; dehl = 32 bits register value
   push hl                     ; save hl

; type8 *ptr = (type8 *) &dreg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[i]

; write_l(ptr, val);
   ld (hl),d
   inc hl
   ld (hl),e                   ; stored high 16 bits of register value
   inc hl
   pop de                      ; de = saved hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; stored low 16 bits of register value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _WRITE_AREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_write_areg_w:

   ; void write_areg_w(type8 i, type16 val) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   dec sp
   pop af                      ; a = register number
   pop de                      ; de = register value
   push hl                     ; restore return address

; type8 *ptr = (type8 *) &areg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[i]

; write_w(ptr + 2, val);
   inc hl
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; stored endian converted value
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _WRITE_AREG_L
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_write_areg_l:

   ; void write_areg_l(type8 i, type32 val) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   dec sp
   pop af                      ; a = register number
   pop de                      ; de = low 16 bits of register value
   ex (sp),hl                  ; hl = high 16 bits of register value

   ex de,hl                    ; dehl = 32 bits register value
   push hl                     ; save hl

; type8 *ptr = (type8 *) &areg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[i]

; write_l(ptr, val);
   ld (hl),d
   inc hl
   ld (hl),e                   ; stored high 16 bits of register value
   inc hl
   pop de                      ; de = saved hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; stored low 16 bits of register value
   ret

;;
;; Optimization: Specialized functions for incrementing/decrementing a register.
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _INC_DREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_inc_dreg_w:

   ; void inc_dreg_w(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = data register number
   ; exit  : none
   ; uses  : af, de, hl

; type8 *ptr = (type8 *) &dreg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; ptr = hl = &dreg[i]

; ptr += 2;
   inc hl
   inc hl                      ; ptr = ptr + 2

; write_w(ptr, read_w(ptr) + 1);
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = read_w(ptr)

   inc de                      ; de = read_w(ptr) + 1

   ; hl was incremented when reading so we go backwards when writing
   ld (hl),e
   dec hl
   ld (hl),d                   ; write_w(ptr, de)

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DEC_DREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_dec_dreg_w:

   ; void dec_dreg_w(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = data register number
   ; exit  : none
   ; uses  : af, de, hl

; type8 *ptr = (type8 *) &dreg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_dreg
   add hl,a                    ; ptr = hl = &dreg[i]

; ptr += 2;
   inc hl
   inc hl                      ; ptr = ptr + 2

; write_w(ptr, read_w(ptr) - 1);
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = read_w(ptr)

   dec de                      ; de = read_w(ptr) - 1

   ; hl was incremented when reading so we go backwards when writing
   ld (hl),e
   dec hl
   ld (hl),d                   ; write_w(ptr, de)

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _INC_AREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_inc_areg_w:

   ; void inc_areg_w(type8 i) __z88dk_fastcall;
   ;
   ; enter : l = address register number
   ; exit  : none
   ; uses  : af, de, hl

; type8 *ptr = (type8 *) &areg[i];
   ld a,l                      ; a = register number
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; ptr = hl = &areg[i]

; ptr += 2;
   inc hl
   inc hl                      ; ptr = ptr + 2

; write_w(ptr, read_w(ptr) + 1);
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = read_w(ptr)

   inc de                      ; de = read_w(ptr) + 1

   ; hl was incremented when reading so we go backwards when writing
   ld (hl),e
   dec hl
   ld (hl),d                   ; write_w(ptr, de)

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _INC_VAR_AREG_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_inc_var_areg_w:

   ; void inc_var_areg_w(type8 i, type16 val) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, bc, de, hl

   pop hl                      ; return address
   dec sp
   pop af                      ; a = register number
   pop de                      ; de = register value
   push hl                     ; restore return address

; type8 *ptr = (type8 *) &areg[i];
   add a,a
   add a,a                     ; a = register number as byte index
   ld hl,_areg
   add hl,a                    ; ptr = hl = &areg[i]

; ptr += 2;
   inc hl
   inc hl                      ; ptr = ptr + 2

   push hl                     ; save ptr = hl

; write_w(ptr, read_w(ptr) + val);
   ld b,(hl)
   inc hl
   ld c,(hl)                   ; bc = read_w(ptr)

   ex de,hl                    ; hl = val
   add hl,bc
   ex de,hl                    ; de = read_w(ptr) + val

   pop hl                      ; restore ptr = hl

   ld (hl),d
   inc hl
   ld (hl),e                   ; write_w(ptr, de)

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SET_INFO
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_set_info:

   ; void set_info(type8 b) __z88dk_fastcall;
   ;
   ; enter : l
   ; exit  : none
   ; uses  : af, l

; regnr = (type8) (b & 0x07);
   ld a,l
   and a,0x07
   ld (_regnr),a

; admode = (type8) ((b >> 3) & 0x07);
   ld a,l
   rrca
   rrca
   rrca
   and a,0x07
   ld (_admode),a

; opsize = (type8) (b >> 6);
   ld a,l
   rlca
   rlca
   and a,0x03
   ld (_opsize),a

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _READ_WORD
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_word:

   ; void read_word(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

;  type8 *epc = effective(pc);
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value

; byte1 = epc[0];
   ld a,(hl)
   ld (_byte1),a

; byte2 = epc[1];
   inc hl
   ld a,(hl)
   ld (_byte2),a

; pc += 2;
   ld hl,(_pc)
   ld de,2
   add hl,de
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld de,0
   adc hl,de
   ld (_pc + 2),hl
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SET_ARG1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_set_arg1:

   ; void set_arg1(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; is_reversible = 1;
   ld hl,_is_reversible
   ld (hl),1

; switch (admode)
   ld a,7
   ld hl,_admode
   sub a,(hl)
   jp c,set_arg1_end_switch

   ld c,(hl)
   ld b,0                      ; bc = admode
   ld hl,set_arg1_switch
   add hl,bc
   add hl,bc
   add hl,bc
   jp (hl)
set_arg1_switch:
   jp admode_0
   jp admode_1
   jp admode_2
   jp admode_3
   jp admode_4
   jp admode_5
   jp admode_6
   jp admode_7

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 0:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_0:
; arg1 = reg_align((type8 *) &dreg[regnr], opsize); /* Dx */
   ld a,(_regnr)               ; a = regnr
   add a,a
   add a,a                     ; a = regnr as byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[regnr] = ptr parameter
   ld a,(_opsize)              ; a = opsize = size parameter
   call asm_reg_align          ; hl = return value
   ld (_arg1),hl

; is_reversible = 0;
   ld hl,_is_reversible
   ld (hl),0

IF LOGEMU
; out(" d%.1d", regnr);
   ld hl,_regnr
   ld c,(hl)
   ld b,0
   push bc
   ld hl,print_admode_0_str
   push hl
   call _out
   pop af
   pop af
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 1:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_1:
; arg1 = reg_align((type8 *) &areg[regnr], opsize); /* Ax */
   ld a,(_regnr)               ; a = regnr
   add a,a
   add a,a                     ; a = regnr as byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[regnr] = ptr parameter
   ld a,(_opsize)              ; a = opsize = size parameter
   call asm_reg_align          ; hl = return value
   ld (_arg1),hl

; is_reversible = 0;
   ld hl,_is_reversible
   ld (hl),0

IF LOGEMU
; out(" a%.1d", regnr);
   ld hl,_regnr
   ld c,(hl)
   ld b,0
   push bc
   ld hl,print_admode_1_str
   push hl
   call _out
   pop af
   pop af
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 2:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_2:
; arg1i = read_areg_l(regnr); /* (Ax) */
   ld hl,(_regnr)              ; l = regnr
   call _read_areg_l           ; dehl = return value
   ld (_arg1i),hl
   ld (_arg1i + 2),de

IF LOGEMU
; out(" (a%.1d)", regnr);
   ld hl,_regnr
   ld c,(hl)
   ld b,0
   push bc
   ld hl,print_admode_2_str
   push hl
   call _out
   pop af
   pop af
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 3:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_3:
; arg1i = read_areg_l(regnr); /* (Ax)+ */
   ld hl,(_regnr)              ; l = regnr
   call _read_areg_l           ; dehl = return value
   ld (_arg1i),hl
   ld (_arg1i + 2),de

; write_areg_l(regnr, arg1i + (type8) (1 << opsize));
   ld de,1
   ld hl,_opsize
   ld b,(hl)
   bsla de,b                   ; de = (1 << opsize)

   ld hl,(_arg1i)
   add hl,de
   ex de,hl                    ; de = low 16 bits of addition
   ld hl,(_arg1i + 2)
   ld bc,0
   adc hl,bc                   ; hl = high 16 bits of addition
   ld a,(_regnr)
   push hl
   push de                     ; push value parameter
   push af                     ; push regnr parameter
   inc sp
   call _write_areg_l

IF LOGEMU
; out(" (a%.1d)+", regnr);
   ld hl,_regnr
   ld c,(hl)
   ld b,0
   push bc
   ld hl,print_admode_3_str
   push hl
   call _out
   pop af
   pop af
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_4:
; arg1i = read_areg_l(regnr) - (type8) (1 << opsize); /* -(Ax) */
   ld hl,(_regnr)              ; l = regnr
   call _read_areg_l           ; dehl = return value
   ld (_arg1i),hl
   ld (_arg1i + 2),de          ; save intermediate result

   ld de,1
   ld hl,_opsize
   ld b,(hl)
   bsla de,b                   ; de = (1 << opsize)

   ld hl,(_arg1i)
   xor a                       ; clear carry
   sbc hl,de
   ex de,hl                    ; de = low 16 bits of subtraction
   ld hl,(_arg1i + 2)
   ld bc,0
   sbc hl,bc                   ; hl = high 16 bits of subtraction
   ld (_arg1i),de
   ld (_arg1i + 2),hl          ; store result in arg1i

; write_areg_l(regnr, arg1i);
   ld a,(_regnr)
   push hl
   push de                     ; push value parameter
   push af                     ; push regnr parameter
   inc sp
   call _write_areg_l

IF LOGEMU
; out(" -(a%.1d)", regnr);
   ld hl,_regnr
   ld c,(hl)
   ld b,0
   push bc
   ld hl,print_admode_4_str
   push hl
   call _out
   pop af
   pop af
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_5:
; type16s i = (type16s) read_w(effective(pc));
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = endian converted value
   push de                     ; save de

; arg1i = read_areg_l(regnr) + i;
   ld hl,(_regnr)              ; l = regnr
   call _read_areg_l           ; dehl = return value
   ld (_arg1i),hl
   ld (_arg1i + 2),de          ; save intermediate result

   pop de                      ; de = unsigned 16-bit value
   ld a,d
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld c,a
   ld b,a                      ; bcde = sign extended 32-bit value

   ld hl,(_arg1i)
   add hl,de
   ld (_arg1i),hl              ; store low 16 bits of addition
   ld hl,(_arg1i + 2)
   adc hl,bc
   ld (_arg1i + 2),hl          ; store high 16 bits of addition

; pc += 2; /* offset.w(Ax) */
   ld hl,(_pc)
   ld bc,2
   add hl,bc
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld bc,0
   adc hl,bc
   ld (_pc + 2),hl

IF LOGEMU
; out(" %X(a%.1d)", i, regnr);
   ld hl,_regnr
   ld c,(hl)
   ld b,0
   push bc
   push de                     ; de = i
   ld hl,print_admode_5_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 6:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_6:
; type8 tmp1 = byte1;
   ld a,(_byte1)
   push af

; type8 tmp2 = byte2;
   ld a,(_byte2)
   push af

; read_word(); /* offset.b(Ax, Dx/Ax) [1d1c] */
   call _read_word

IF LOGEMU
; out(" %.2X(a%.1d,", (type16) byte2, regnr);
   ld hl,_regnr
   ld e,(hl)
   ld d,0
   push de
   ld hl,_byte2
   ld e,(hl)                   ; d = 0
   push de
   ld hl,print_admode_6_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; arg1i = read_areg_l(regnr) + (type8s) byte2;
   ld hl,(_regnr)              ; l = regnr
   call _read_areg_l           ; dehl = return value
   ld (_arg1i),hl
   ld (_arg1i + 2),de          ; save intermediate result

   ld a,(_byte2)               ; a = byte2
   ld e,a
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld d,a                      ; de = (type16s) byte2
   ld c,a
   ld b,a                      ; bcde = (type32s) byte2

   ld hl,(_arg1i)
   add hl,de
   ld (_arg1i),hl              ; store low 16 bits of addition
   ld hl,(_arg1i + 2)
   adc hl,bc
   ld (_arg1i + 2),hl          ; store high 16 bits of addition

   ; precalculate (byte1 >> 4)
   ld a,(_byte1)               ; a = byte1
   ld e,a
   ld d,0
   ld b,4
   bsrl de,b
   ld l,e                      ; l = (byte1 >> 4)

IF LOGEMU
   push af
   push hl

; if ((byte1 >> 4) > 8)
   ld a,8
   cp a,l
   jr nc,admode_6_logemu_else
; out("a%.1d", (byte1 >> 4) - 8);
   ld a,l
   sub a,8
   ld e,a
   ld d,0
   push de
   ld hl,print_admode_6_areg_str
   push hl
   call _out
   pop af
   pop af
   jr admode_6_logemu_endif
; else
admode_6_logemu_else:
; out("d%.1d", byte1 >> 4);
   ld a,l
   ld e,a
   ld d,0
   push de
   ld hl,print_admode_6_dreg_str
   push hl
   call _out
   pop af
   pop af
; end-if
admode_6_logemu_endif:

  pop hl
  pop af
ENDIF

; if (byte1 & 0x08)
   bit 3,a
   jr z,admode_6_word
; arg1i += (type32s) read_reg_l(byte1 >> 4);
   ; l = regnr = (byte1 >> 4)
   call _read_reg_l            ; dehl = return value
   ld bc,(_arg1i)
   add hl,bc
   ld (_arg1i),hl
   ld hl,(_arg1i + 2)
   adc hl,de
   ld (_arg1i + 2),hl
IF LOGEMU
;  out(".l)");
   ld hl,print_admode_6_l_str
   push hl
   call _out
   pop af
ENDIF
   jr admode_6_endif
; else
admode_6_word:
; arg1i += (type16s) read_reg_w(byte1 >> 4);
   ; l = regnr = (byte1 >> 4)
   call _read_reg_w            ; hl = return value
   ex de,hl                    ; de = unsigned 16-bit value
   ld a,d
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld c,a
   ld b,a                      ; bcde = sign extended 32-bit value

   ld hl,(_arg1i)
   add hl,de
   ld (_arg1i),hl              ; store low 16 bits of addition
   ld hl,(_arg1i + 2)
   adc hl,bc
   ld (_arg1i + 2),hl          ; store high 16 bits of addition
IF LOGEMU
;  out(".w)");
   ld hl,print_admode_6_w_str
   push hl
   call _out
   pop af
ENDIF
; end-if
admode_6_endif:

; byte2 = tmp2;
   pop af
   ld (_byte2),a

; byte1 = tmp1;
   pop af
   ld (_byte1),a

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 7: /* specials */
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
admode_7:
; switch (regnr)
   ld a,4
   ld hl,_regnr
   sub a,(hl)
   jp c,set_arg1_end_switch

   ld c,(hl)
   ld b,0                      ; bc = regnr
   ld hl,set_arg1_regnr_switch
   add hl,bc
   add hl,bc
   add hl,bc
   jp (hl)
set_arg1_regnr_switch:
   jp regnr_0
   jp regnr_1
   jp regnr_2
   jp regnr_3
   jp regnr_4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 7: case 0:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
regnr_0:
; arg1i = read_w(effective(pc)); /* $xxxx.W */
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = endian converted value
   ld (_arg1i),de
   ld hl,0
   ld (_arg1i + 2),hl

; pc += 2;
   ld hl,(_pc)
   ld bc,2
   add hl,bc
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld bc,0
   adc hl,bc
   ld (_pc + 2),hl

IF LOGEMU
; out(" %.4X.w", arg1i);
   ld hl,(_arg1i + 2)
   push hl
   ld hl,(_arg1i)
   push hl
   ld hl,print_admode_7_regnr_0_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 7: case 1:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
regnr_1:
; arg1i = read_l(effective(pc)); /* $xxxx */
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value

   ; Big endian 0A0B0C0D is little endian 0D0C0B0A. Note that register pairs
   ; store MSB first, e.g. if bc = 0A0B it is written to memory as 0B0A.
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; bcde = endian converted value
   ld (_arg1i),de
   ld (_arg1i + 2),bc

; pc += 4;
   ld hl,(_pc)
   ld bc,4
   add hl,bc
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld bc,0
   adc hl,bc
   ld (_pc + 2),hl

IF LOGEMU
; out(" %.4X", arg1i);
   ld hl,(_arg1i + 2)
   push hl
   ld hl,(_arg1i)
   push hl
   ld hl,print_admode_7_regnr_1_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 7: case 2:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
regnr_2:
; arg1i = (type16s) read_w(effective(pc)) + pc; /* $xxxx(PC) */
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = endian converted value

   ld a,d
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld c,a
   ld b,a                      ; bcde = sign extended 32-bit value

   ld hl,(_pc)
   add hl,de
   ld (_arg1i),hl              ; store low 16 bits of addition
   ld hl,(_pc + 2)
   adc hl,bc
   ld (_arg1i + 2),hl          ; store high 16 bits of addition

; pc += 2;
   ld hl,(_pc)
   ld bc,2
   add hl,bc
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld bc,0
   adc hl,bc
   ld (_pc + 2),hl

IF LOGEMU
; out(" %.4X(pc)", arg1i);
   ld hl,(_arg1i + 2)
   push hl
   ld hl,(_arg1i)
   push hl
   ld hl,print_admode_7_regnr_2_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; break;
   jp set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 7: case 3:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
regnr_3:
IF LOGEMU
; out(" ???2", arg1i);
   ld hl,(_arg1i + 2)
   push hl
   ld hl,(_arg1i)
   push hl
   ld hl,print_admode_7_regnr_3_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; type8 l1c = effective(pc)[0]; /* $xx(PC,A/Dx) */
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value
   ld a,(hl)
   ld b,a                      ; b = l1c

   ; precalculate (l1c >> 4)
   rlca
   rlca
   rlca
   rlca
   and a,0x0f
   ld l,a                      ; l = (l1c >> 4)

; if (l1c & 0x08)
   bit 3,b
   jr z,regnr_3_word
; arg1i = pc + (type32s) read_reg_l(l1c >> 4);
   ; l = regno = (l1c >> 4)
   call _read_reg_l            ; dehl = return value
   ld bc,(_pc)
   add hl,bc
   ld (_arg1i),hl
   ld hl,(_pc + 2)
   adc hl,de
   ld (_arg1i + 2),hl
   jr regnr_3_endif
; else
regnr_3_word:
; arg1i = pc + (type16s) read_reg_w(l1c >> 4);
   ; l = regno = (l1c >> 4)
   call _read_reg_w            ; hl = return value
   ex de,hl                    ; de = unsigned 16-bit value

   ld a,d
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld c,a
   ld b,a                      ; bcde = sign extended 32-bit value

   ld hl,(_pc)
   add hl,de
   ld (_arg1i),hl
   ld hl,(_pc + 2)
   adc hl,bc
   ld (_arg1i + 2),hl
; end-if
regnr_3_endif:

; l1c = effective(pc)[1];
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value
   inc hl
   ld a,(hl)
   ld c,a                      ; c = l1c

; pc += 2;
   ld hl,(_pc)
   ld de,2
   add hl,de
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld de,0
   adc hl,de
   ld (_pc + 2),hl

; arg1i += (type8s) l1c;
   ld a,c
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld b,a                      ; bc = sign extended 16-bit value
   ld e,a
   ld d,a                      ; debc = sign extended 32-bit value

   ld hl,(_arg1i)
   add hl,bc
   ld (_arg1i),hl
   ld hl,(_arg1i + 2)
   adc hl,de
   ld (_arg1i + 2),hl

; break;
   jr set_arg1_end_switch

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; case 7: case 4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
regnr_4:
; arg1i = pc; /* #$xxxx */
   ld hl,(_pc)
   ld (_arg1i),hl
   ld hl,(_pc + 2)
   ld (_arg1i + 2),hl

; if (opsize == 0)
   ld a,(_opsize)
   or a,a
   jr nz,regnr_4_endif
; arg1i += 1;
   ld hl,_arg1i
   inc (hl)
   jr nz,regnr_4_endif
   inc hl
   inc (hl)
   jr nz,regnr_4_endif
   inc hl
   inc (hl)
   jr nz,regnr_4_endif
   inc hl
   inc (hl)
; end-if
regnr_4_endif:

; if (opsize == 2)
   ld a,(_opsize)
   cp a,2
   jr nz,regnr_4_word
; pc += 4;
   ld hl,(_pc)
   ld bc,4
   add hl,bc
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld bc,0
   adc hl,bc
   ld (_pc + 2),hl
   jr regnr_4_word_endif
; else
regnr_4_word:
; pc += 2;
   ld hl,(_pc)
   ld bc,2
   add hl,bc
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld bc,0
   adc hl,bc
   ld (_pc + 2),hl
; end-if
regnr_4_word_endif:

IF LOGEMU
;  out(" #%.4X", arg1i);
   ld hl,(_arg1i + 2)
   push hl
   ld hl,(_arg1i)
   push hl
   ld hl,print_admode_7_regnr_4_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF
; end-inner-switch
; end-outer-switch
set_arg1_end_switch:

; if (is_reversible)
   ld a,(_is_reversible)
   or a,a
   jr z,set_arg1_end
; arg1 = effective(arg1i);
   ld hl,(_arg1i)
   ld de,(_arg1i + 2)          ; dehl = arg1i
   call _effective             ; hl = return value
   ld (_arg1),hl
; end-if

set_arg1_end:
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SET_ARG2_NOSIZE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_set_arg2_nosize:

   ; void set_arg2_nosize(type8 use_dx, type8 b) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   pop de                      ; e = use_dx, d = b
   push hl                     ; restore return address

asm_set_arg2_nosize:

   ; asm_set_arg2_nosize
   ;
   ; enter : e = use_dx
   ;         d = b
   ; exit  : none
   ; uses  : af, de, hl

; if (use_dx)
   ld a,e
   or a,a
   jr z,set_arg2_nosize_else

; arg2 = (type8 *) dreg;
   ld hl,_dreg
   ld (_arg2),hl
   jr set_arg2_nosize_endif

; else
set_arg2_nosize_else:

; arg2 = (type8 *) areg;
   ld hl,_areg
   ld (_arg2),hl

; end-if
set_arg2_nosize_endif:

; arg2 += (b & 0x0e) << 1;
   ld a,d
   and a,0x0e
   ld l,a
   ld h,0                      ; hl = (b & 0x0e)
   add hl,hl                   ; hl = (b & 0x0e) << 1

   ld de,(_arg2)
   add hl,de
   ld (_arg2),hl

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SET_ARG2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_set_arg2:

   ; void set_arg2(type8 use_dx, type8 b) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, de, hl

   pop hl                      ; return address
   pop de                      ; e = use_dx, d = b
   push hl                     ; restore return address

; set_arg2_nosize(use_dx, b);
   call asm_set_arg2_nosize    ; e = use_dx, d = b

; arg2 = reg_align(arg2, opsize);
   ld hl,(_arg2)               ; hl = pointer
   ld a,(_opsize)              ; a = size
   call asm_reg_align          ; hl = return value
   ld (_arg2),hl

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SWAP_ARGS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_swap_args:

   ; void swap_args(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : de, hl

; type8 *tmp = arg1;
   ld de,(_arg1)

; arg1 = arg2;
   ld hl,(_arg2)
   ld (_arg1),hl

; arg2 = tmp;
   ld (_arg2),de

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _PUSH
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_push:

   ; void push(type32 c) __z88dk_fastcall;
   ;
   ; enter : dehl = c
   ; exit  : none
   ; uses  : af, bc, de, hl

   ; Note: The statements are reordered compared to the
   ; original C code to minimize the read_areg_l() calls.

   push de
   push hl                     ; save c = dehl

; type32 new_sp = read_areg_l(7) - 4;
   ld l,7                      ; l = regnr
   call _read_areg_l           ; dehl = return value

   xor a                       ; clear carry
   ld bc,4
   sbc hl,bc
   ex de,hl                    ; de = new_sp low word, hl = reg high word
   ld bc,0
   sbc hl,bc                   ; new_sp = hlde

   push hl
   push de                     ; save new_sp = hlde

; write_areg_l(7, new_sp);
   push hl
   push de                     ; push value parameter
   ld a,7
   push af                     ; push regnr parameter
   inc sp
   call _write_areg_l

; write_l(effective(new_sp), c);
   pop hl
   pop de                      ; dehl = new_sp
   call _effective             ; hl = return value

   pop bc
   pop de                      ; restore c = debc

   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   ld (hl),b
   inc hl
   ld (hl),c                   ; store endian converted value

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _POP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_pop:

   ; type32 pop(void);
   ;
   ; enter : none
   ; exit  : dehl
   ; uses  : af, bc, de, hl

   ; Note: The statements are reordered compared to the
   ; original C code to minimize the read_areg_l() calls.

; type32 old_sp = read_areg_l(7);
   ld l,7                      ; l = regnr
   call _read_areg_l           ; old_sp = dehl

   push de
   push hl                     ; save old_sp = dehl

; write_areg_l(7, old_sp + 4);
   ld bc,4
   add hl,bc
   ex de,hl                    ; de = result low word, hl = old_sp high word
   ld bc,0
   adc hl,bc                   ; hlde = old_sp + 4

   push hl
   push de                     ; push value parameter
   ld a,7
   push af                     ; push regnr parameter
   inc sp
   call _write_areg_l

; return read_l(effective(old_sp));
   pop hl
   pop de                      ; dehl = old_sp
   call _effective             ; hl = return value

   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = endian converted return value

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _GET_ARG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   ; void get_arg(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

_get_arg:

IF LOGEMU
; out(" %.4X", pc);
   ld hl,(_pc + 2)
   push hl
   ld hl,(_pc)
   push hl
   ld hl,print_pc_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

; set_info(byte2);
   ld hl,(_byte2)              ; l = argument
   call _set_info

; arg2 = effective(pc);
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value
   ld (_arg2),hl

   ld a,(_opsize)

; if (opsize == 2)
   cp a,2
   jr nz,get_arg_pc_else
; pc += 4;
   ld hl,(_pc)
   ld de,4
   add hl,de
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld de,0
   adc hl,de
   ld (_pc + 2),hl
   jr get_arg_pc_endif
; else
get_arg_pc_else:
; pc += 2;
   ld hl,(_pc)
   ld de,2
   add hl,de
   ld (_pc),hl
   ld hl,(_pc + 2)
   ld de,0
   adc hl,de
   ld (_pc + 2),hl
; end-if
get_arg_pc_endif:

; if (opsize == 0)
   or a,a
   jr nz,get_arg_arg2_endif
; arg2 += 1;
   ld hl,(_arg2)
   inc hl
   ld (_arg2),hl
; end-if
get_arg_arg2_endif:

; save_arg2();
   call _save_arg2

; set_arg1();
   jp _set_arg1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _SET_FLAGS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_set_flags:

   ; void set_flags(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, de, hl

; zflag = nflag = 0;
   ld hl,_nflag
   ld (hl),0
   ld hl,_zflag
   ld (hl),0

; switch (opsize)
   ld a,(_opsize)
   or a,a
   jr z,set_flags_opsize_0
   cp a,1
   jr z,set_flags_opsize_1
   cp a,2
   jr z,set_flags_opsize_2
   jr set_flags_end_switch

; case 0:
set_flags_opsize_0:
   ld de,(_arg1)
   ld a,(de)                    ; a = arg1[0]

; if (arg1[0] > 127)
   bit 7,a
   jr z,set_flags_opsize_0_else
; nflag = 0xff;
   ld hl,_nflag
   ld (hl),0xff
   jr set_flags_end_switch
set_flags_opsize_0_else:
; else if (arg1[0] == 0)
   or a,a
   jr nz,set_flags_end_switch
; zflag = 0xff;
   ld hl,_zflag
   ld (hl),0xff
; end-if
; break;
   jr set_flags_end_switch

; case 1:
set_flags_opsize_1:
; type16 i = read_w(arg1);
   ld hl,(_arg1)
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = i = endian converted value

; if (i == 0)
   ld a,d
   or a,e
   jr nz,set_flags_opsize_1_else
; zflag = 0xff;
   ld hl,_zflag
   ld (hl),0xff
   jr set_flags_end_switch
set_flags_opsize_1_else:
; else if (i & 0x8000)
   bit 7,d
   jr z,set_flags_end_switch
; nflag = 0xff;
   ld hl,_nflag
   ld (hl),0xff
; end-if
; break;
   jr set_flags_end_switch

; case 2:
set_flags_opsize_2:
; type32 j = read_l(arg1);
   ld hl,(_arg1)
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = j = endian converted value

; if (j == 0)
   ld a,d
   or a,e
   or a,h
   or a,l
   jr nz,set_flags_opsize_2_else
; zflag = 0xff;
   ld hl,_zflag
   ld (hl),0xff
   jr set_flags_end_switch
set_flags_opsize_2_else:
; else if (j & 0x80000000UL)
   bit 7,d
   jr z,set_flags_end_switch
; nflag = 0xff;
   ld hl,_nflag
   ld (hl),0xff
; end-if
; break;
set_flags_end_switch:

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _CONDITION
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_condition:

   ; type8 condition(type8 b) __z88dk_fastcall;
   ;
   ; enter : l
   ; exit  : l
   ; uses  : af, bc, hl

   ; Note: The switch parameter is a 4-bit value between 0 and 15 and the switch
   ; statement covers all the cases, so no out-of-range checking is required.

; switch (b & 0x0f)
   ld a,l
   and a,0x0f
   ld c,a
   ld b,0                      ; bc = switch parameter
   ld hl,condition_switch
   add hl,bc
   add hl,bc
   add hl,bc
   jp (hl)
condition_switch:
   jp condition_case_0         ;  0
   jp condition_case_1         ;  1
   jp condition_case_2         ;  2
   jp condition_case_3         ;  3
   jp condition_case_4         ;  4
   jp condition_case_5         ;  5
   jp condition_case_6         ;  6
   jp condition_case_7         ;  7
   jp condition_case_8         ;  8
   jp condition_case_9         ;  9
   jp condition_case_12        ; 10 (fall-through to 12)
   jp condition_case_13        ; 11 (fall-through to 13)
   jp condition_case_12        ; 12
   jp condition_case_13        ; 13
   jp condition_case_14        ; 14
   jp condition_case_15        ; 15

; case 0:
condition_case_0:
; return 0xff;
   ld l,0xff
   ret

; case 1:
condition_case_1:
; return 0x00;
   ld l,0x00
   ret

; case 2:
condition_case_2:
; return (zflag | cflag) ^ 0xff;
   ld a,(_zflag)
   ld hl,_cflag
   or a,(hl)
   cpl
   ld l,a
   ret

; case 3:
condition_case_3:
; return (zflag | cflag);
   ld a,(_zflag)
   ld hl,_cflag
   or a,(hl)
   ld l,a
   ret

; case 4:
condition_case_4:
; return cflag ^ 0xff;
   ld a,(_cflag)
   cpl
   ld l,a
   ret

; case 5:
condition_case_5:
; return cflag;
   ld hl,(_cflag)
   ret

; case 6:
condition_case_6:
; return zflag ^ 0xff;
   ld a,(_zflag)
   cpl
   ld l,a
   ret

; case 7:
condition_case_7:
; return zflag;
   ld hl,(_zflag)
   ret

; case 8:
condition_case_8:
; return vflag ^ 0xff;
   ld a,(_vflag)
   cpl
   ld l,a
   ret

; case 9:
condition_case_9:
; return vflag;
   ld hl,(_vflag)
   ret

; case 10:
; case 12:
condition_case_12:
; return nflag ^ 0xff;
   ld a,(_nflag)
   cpl
   ld l,a
   ret

; case 11:
; case 13:
condition_case_13:
; return nflag;
   ld hl,(_nflag)
   ret

; case 14:
condition_case_14:
; return (zflag | nflag) ^ 0xff;
   ld a,(_zflag)
   ld hl,_nflag
   or a,(hl)
   cpl
   ld l,a
   ret

; case 15:
condition_case_15:
; return (zflag | nflag);
   ld a,(_zflag)
   ld hl,_nflag
   or a,(hl)
   ld l,a
   ret
; end-switch

; Note: The original C function had a "return 0x00;" after the swith statement
; but we have removed that since it's dead code (the switch statement covers all
; the cases).

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _BRANCH
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_branch:

   ; void branch(type8 b) __z88dk_fastcall;
   ;
   ; enter : l
   ; exit  : none
   ; uses  : af, bc, de, hl

; if (b == 0)
   ld a,l
   or a,a
   jr nz,branch_else

; pc += (type16s) read_w(effective(pc));
   ld hl,(_pc)
   ld de,(_pc + 2)             ; dehl = pc
   call _effective             ; hl = return value

   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = unsigned 16-bit value

   ld a,d
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld c,a
   ld b,a                      ; bcde = sign extended 32-bit value

   ; do addition to PC after if statement

   jr branch_endif
; else
branch_else:
; pc += (type8s) b;
   ld a,l
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld e,l
   ld d,a                      ; de = sign extended 16-bit value
   ld c,a
   ld b,a                      ; bcde = sign extended 32-bit value

   ; do addition to PC after if statement

; end-if
branch_endif:

   ld hl,(_pc)
   add hl,de
   ld (_pc),hl
   ld hl,(_pc + 2)
   adc hl,bc
   ld (_pc + 2),hl             ; pc += bcde

IF LOGEMU
; out(" %.4X", pc);
   ld hl,(_pc + 2)
   push hl
   ld hl,(_pc)
   push hl
   ld hl,print_pc_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl
ENDIF

   ret

;;
;; Optimization:
;; The do_add(type8 adda) function has been split into do_add() and do_adda()
;; for improved performance.
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_ADD
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_add:

   ; void do_add(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; cflag = 0;
   ld hl,_cflag
   ld (hl),0

   ld a,(_opsize)
; if (opsize == 0)
   or a,a
   jr nz,do_add_opsize_1

; arg1[0] += arg2[0];
   ld bc,(_arg1)
   ld a,(bc)
   ld l,a                      ; l = arg1[0]
   ld de,(_arg2)
   ld a,(de)
   ld h,a                      ; h = arg2[0]
   add a,l
   ld (bc),a                   ; a = arg1[0] = arg1[0] + arg2[0]

; if (arg2[0] > arg1[0])
   sub a,h
   jp nc,do_add_opsize_endif
; cflag = 0xff;
   ld hl,_cflag
   ld (hl),0xff
   jp do_add_opsize_endif
; end-if

do_add_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_add_opsize_2

; write_w(arg1, (type16) (read_w(arg1) + read_w(arg2)));
   ld hl,(_arg1)
   ld b,(hl)
   inc hl
   ld c,(hl)                   ; bc = read_w(arg1)

   ld hl,(_arg2)
   ld d,(hl)
   inc hl
   ld e,(hl)
   ex de,hl                    ; hl = read_w(arg2)
   push hl                     ; save hl

   add hl,bc
   ex de,hl                    ; de = read_w(arg1) + read_w(arg2)

   ld hl,(_arg1)
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_w(arg1, de)

; if (read_w(arg2) > read_w(arg1))
   ex de,hl                    ; hl = read_w(arg1)
   pop de                      ; de = read_w(arg2)
   xor a                       ; clear carry
   sbc hl,de
   jp nc,do_add_opsize_endif
; cflag = 0xff;
   ld hl,_cflag
   ld (hl),0xff
   jp do_add_opsize_endif
; end-if

do_add_opsize_2:
; else if (opsize == 2)
   cp a,2
   jp nz,do_add_opsize_endif

; write_l(arg1, read_l(arg1) + read_l(arg2));
   ld hl,(_arg1)
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld b,(hl)
   inc hl
   ld c,(hl)                   ; debc = read_l(arg1)
   push de                     ; save de

   ld hl,(_arg2)
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = read_l(arg2)

   add hl,bc                   ; add arg1 and arg2 low words
   ex de,hl                    ; de = result low word, hl = arg2 high word
   pop bc                      ; bc = arg1 high word
   push hl                     ; save arg2 high word
   adc hl,bc                   ; add arg1 and arg2 high words
   ld c,l
   ld b,h                      ; bc = result high word, bcde = total result

   ld hl,(_arg1)
   ld (hl),b
   inc hl
   ld (hl),c
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_l(arg1, bcde)

; if (read_l(arg2) > read_l(arg1))
   ld hl,(_arg2)
   inc hl
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; hl = arg2 low word

   ; arg1 = bcde
   ex de,hl                    ; hl = arg1 low word, de = arg2 low word

   xor a                       ; clear carry
   sbc hl,de                   ; subtract arg1 and arg2 low words
   ld l,c
   ld h,b                      ; hl = arg1 high word
   pop de                      ; de = arg2 high word
   sbc hl,de                   ; subtract arg1 and arg2 high words
   jr nc,do_add_opsize_endif
; cflag = 0xff;
   ld hl,_cflag
   ld (hl),0xff
; end-if
; end-if
do_add_opsize_endif:

; if (version < 3 || !quick_flag)
   ld a,(_version)
   cp a,3
   jr c,do_add_flags
   ld a,(_quick_flag)
   or a,a
   jr nz,do_add_flags_endif
do_add_flags:
; vflag = 0;
   ld hl,_vflag
   ld (hl),0
; set_flags();
   call _set_flags
; end-if
do_add_flags_endif:

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_ADDA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_adda:

   ; void do_adda(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; The original C code looked like this:
;
;    if (opsize == 0)
;        write_l(arg1, read_l(arg1) + (type8s) arg2[0]);
;    else if (opsize == 1)
;        write_l(arg1, read_l(arg1) + (type16s) read_w(arg2));
;    else if (opsize == 2)
;        write_l(arg1, read_l(arg1) + (type32s) read_l(arg2));
;
; Since the if branches all boil down to write_l(arg1, read_l(arg1) + <arg2>),
; we only handle the calculation of <arg2> in the if branches and break out
; the common parts to before and after the if statement.

; type32 a1 = read_l(arg1);
   ld hl,(_arg1)
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld b,(hl)
   inc hl
   ld c,(hl)                   ; debc = read_l(arg1)
   push de                     ; save de

   ; bc must not be used in the if statement below

   ld a,(_opsize)
; if (opsize == 0)
   or a,a
   jr nz,do_adda_opsize_1

; type32s a2 = (type32s) arg2[0];
   ld hl,(_arg2)
   ld a,(hl)
   ld l,a                      ; l = arg2[0]
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld h,a                      ; hl = (type16s) arg2[0]
   ld e,a
   ld d,a                      ; dehl = (type32s) arg2[0]

   jp do_adda_opsize_endif
do_adda_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_adda_opsize_2

; type32s a2 = (type32s) read_w(arg2);
   ld hl,(_arg2)
   ld d,(hl)
   inc hl
   ld e,(hl)
   ex de,hl                    ; hl = read_w(arg2)
   ld a,h
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld e,a
   ld d,a                      ; dehl = (type32s) read_w(arg2)

   jp do_adda_opsize_endif
do_adda_opsize_2:
; else if (opsize == 2)
   cp a,2
   jp nz,do_adda_opsize_endif

; type32s a2 = (type32s) read_l(arg2);
   ld hl,(_arg2)
   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; dehl = read_l(arg2)

; end-if
do_adda_opsize_endif:

   ; a1 = <sp>bc, a2 = dehl

; type32 result = a1 + a2;
   add hl,bc                   ; add arg1 and arg2 low words
   ex de,hl                    ; de = result low word, hl = arg2 high word
   pop bc                      ; bc = arg1 high word
   adc hl,bc                   ; add arg1 and arg2 high words
   ld c,l
   ld b,h                      ; bc = result high word, bcde = total result

; write_l(arg1, result);
   ld hl,(_arg1)
   ld (hl),b
   inc hl
   ld (hl),c
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_l(arg1, bcde)

   ret

;;
;; Optimization:
;; The do_sub(type8 suba) function has been split into do_sub() and do_suba()
;; for improved performance.
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_SUB
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_sub:

   ; void do_sub(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; cflag = 0;
   ld hl,_cflag
   ld (hl),0

   ld a,(_opsize)
; if (opsize == 0)
   or a,a
   jr nz,do_sub_opsize_1

; arg1[0] -= arg2[0];
   ld bc,(_arg1)
   ld a,(bc)
   ld l,a                      ; l = arg1[0]
   ld de,(_arg2)
   ld a,(de)
   ld h,a                      ; h = arg2[0]
   ld a,l
   sub a,h                     ; arg1[0] - arg2[0]
   ld (bc),a                   ; arg1[0] = arg1[0] - arg2[0]

; if (arg2[0] > arg1[0])
   ; This if statement was originally placed above the preceeding subtraction.
   ; Since evaluating the condition of the if statement is the same as
   ; determining the carry status of the same subtraction as above, we can move
   ; the if statement down here and reuse the carry status from the subtraction
   ; above instead. The end result will be the same.
   jp nc,do_sub_opsize_endif
; cflag = 0xff;
   ld hl,_cflag
   ld (hl),0xff
   jp do_sub_opsize_endif
; end-if

do_sub_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_sub_opsize_2

; write_w(arg1, (type16) (read_w(arg1) - read_w(arg2)));
   ld hl,(_arg1)
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; de = read_w(arg1)

   ld hl,(_arg2)
   ld b,(hl)
   inc hl
   ld c,(hl)                   ; bc = read_w(arg2)

   ex de,hl                    ; hl = read_w(arg1)

   xor a                       ; clear carry
   sbc hl,bc                   ; read_w(arg1) - read_w(arg2)
   ex de,hl                    ; de = read_w(arg1) - read_w(arg2)

   ld hl,(_arg1)
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_w(arg1, de)

; if (read_w(arg2) > read_w(arg1))
   ; This if statement was originally placed above the preceeding subtraction.
   ; Since evaluating the condition of the if statement is the same as
   ; determining the carry status of the same subtraction as above, we can move
   ; the if statement down here and reuse the carry status from the subtraction
   ; above instead. The end result will be the same.
   jp nc,do_sub_opsize_endif
; cflag = 0xff;
   ld hl,_cflag
   ld (hl),0xff
   jp do_sub_opsize_endif
; end-if

do_sub_opsize_2:
; else if (opsize == 2)
   cp a,2
   jp nz,do_sub_opsize_endif

; write_l(arg1, read_l(arg1) - read_l(arg2));
   ld hl,(_arg1)
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; bcde = read_l(arg1)
   push bc                     ; save bc

   ld hl,(_arg2)
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; bchl = read_l(arg2)

   ex de,hl                    ; arg1 = <sp>hl, arg2 = bcde

   xor a                       ; clear carry
   sbc hl,de                   ; arg1_low_word - arg2_low_word
   ex de,hl                    ; de = result_low_word
   pop hl                      ; hl = arg1_high_word
   sbc hl,bc                   ; arg1_high_word - arg2_high_word
   ld c,l
   ld b,h                      ; bc = result_high_word, bcde = total result

   ld hl,(_arg1)
   ld (hl),b
   inc hl
   ld (hl),c
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_l(arg1, bcde)

; if (read_l(arg2) > read_l(arg1))
   ; This if statement was originally placed above the preceeding subtraction.
   ; Since evaluating the condition of the if statement is the same as
   ; determining the carry status of the same subtraction as above, we can move
   ; the if statement down here and reuse the carry status from the subtraction
   ; above instead. The end result will be the same.
   jr nc,do_sub_opsize_endif
; cflag = 0xff;
   ld hl,_cflag
   ld (hl),0xff
; end-if
; end-if
do_sub_opsize_endif:

; if (version < 3 || !quick_flag)
   ld a,(_version)
   cp a,3
   jr c,do_sub_flags
   ld a,(_quick_flag)
   or a,a
   jr nz,do_sub_flags_endif
do_sub_flags:
; vflag = 0;
   ld hl,_vflag
   ld (hl),0
; set_flags();
   call _set_flags
; end-if
do_sub_flags_endif:

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_SUBA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_suba:

   ; void do_suba(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; The original C code looked like this:
;
;    if (opsize == 0)
;        write_l(arg1, read_l(arg1) - (type8s) arg2[0]);
;    else if (opsize == 1)
;        write_l(arg1, read_l(arg1) - (type16s) read_w(arg2));
;    else if (opsize == 2)
;        write_l(arg1, read_l(arg1) - (type32s) read_l(arg2));
;
; Since the if branches all boil down to write_l(arg1, read_l(arg1) - <arg2>),
; we only handle the calculation of <arg2> in the if branches and break out
; the common parts to before and after the if statement.

; type32 a1 = read_l(arg1);
   ld hl,(_arg1)
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld d,(hl)
   inc hl
   ld e,(hl)                   ; bcde = read_l(arg1)
   push bc                     ; save bc

   ; de must not be used in the if statement below

   ld a,(_opsize)
; if (opsize == 0)
   or a,a
   jr nz,do_suba_opsize_1

; type32s a2 = (type32s) arg2[0];
   ld hl,(_arg2)
   ld a,(hl)
   ld l,a                      ; l = arg2[0]
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld h,a                      ; hl = (type16s) arg2[0]
   ld c,a
   ld b,a                      ; bchl = (type32s) arg2[0]

   jp do_suba_opsize_endif
do_suba_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_suba_opsize_2

; type32s a2 = (type32s) read_w(arg2);
   ld hl,(_arg2)
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; hl = read_w(arg2)
   rla                         ; move sign bit of a into carry flag
   sbc a,a                     ; a = 0x00 or 0xFF depending on the carry
   ld c,a
   ld b,a                      ; bchl = (type32s) read_w(arg2)

   jp do_suba_opsize_endif
do_suba_opsize_2:
; else if (opsize == 2)
   cp a,2
   jp nz,do_suba_opsize_endif

; type32s a2 = (type32s) read_l(arg2);
   ld hl,(_arg2)
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld l,(hl)
   ld h,a                      ; bchl = read_l(arg2)

; end-if
do_suba_opsize_endif:

   ; a1 = <sp>de, a2 = bchl
   ex de,hl
   ; a1 = <sp>hl, a2 = bcde

; type32 result = a1 - a2;
   xor a                       ; clear carry
   sbc hl,de                   ; subtract arg1 and arg2 low words
   ex de,hl                    ; de = result low word
   pop hl                      ; hl = arg1 high word
   sbc hl,bc                   ; subtract arg1 and arg2 high words
   ld c,l
   ld b,h                      ; bc = result high word, bcde = total result

; write_l(arg1, result);
   ld hl,(_arg1)
   ld (hl),b
   inc hl
   ld (hl),c
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_l(arg1, bcde)

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_EOR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_eor:

   ; void do_eor(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, b, de, hl

   ld a,(_opsize)

; if (opsize == 0)
   or a,a
   jr nz,do_eor_opsize_1

; arg1[0] ^= arg2[0];
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(de)
   xor a,(hl)
   ld (de),a
   jr do_eor_endif

do_eor_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_eor_opsize_2

; *(type16 *)(arg1) ^= *(type16 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(de)
   xor a,(hl)
   ld (de),a
   inc de
   inc hl
   ld a,(de)
   xor a,(hl)
   ld (de),a
   jr do_eor_endif

do_eor_opsize_2:
; else if (opsize == 2)
   cp a,2
   jr nz,do_eor_endif

; *(type32 *)(arg1) ^= *(type32 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ld b,4
do_eor_opsize_2_loop:
   ld a,(de)
   xor a,(hl)
   ld (de),a
   inc de
   inc hl
   djnz do_eor_opsize_2_loop
; end-if
do_eor_endif:

; cflag = vflag = 0;
   ld hl,_vflag
   ld (hl),0
   ld hl,_cflag
   ld (hl),0

; set_flags();
   jp _set_flags

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_AND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_and:

   ; void do_and(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, b, de, hl

   ld a,(_opsize)

; if (opsize == 0)
   or a,a
   jr nz,do_and_opsize_1

; arg1[0] &= arg2[0];
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(de)
   and a,(hl)
   ld (de),a
   jr do_and_endif

do_and_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_and_opsize_2

; *(type16 *)(arg1) &= *(type16 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(de)
   and a,(hl)
   ld (de),a
   inc de
   inc hl
   ld a,(de)
   and a,(hl)
   ld (de),a
   jr do_and_endif

do_and_opsize_2:
; else if (opsize == 2)
   cp a,2
   jr nz,do_and_endif

; *(type32 *)(arg1) &= *(type32 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ld b,4
do_and_opsize_2_loop:
   ld a,(de)
   and a,(hl)
   ld (de),a
   inc de
   inc hl
   djnz do_and_opsize_2_loop
; end-if
do_and_endif:

; cflag = vflag = 0;
   ld hl,_vflag
   ld (hl),0
   ld hl,_cflag
   ld (hl),0

; set_flags();
   jp _set_flags

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_OR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_or:

   ; void do_or(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, b, de, hl

   ld a,(_opsize)

; if (opsize == 0)
   or a,a
   jr nz,do_or_opsize_1

; arg1[0] |= arg2[0];
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(de)
   or a,(hl)
   ld (de),a
   jr do_or_endif

do_or_opsize_1:
; else if (opsize == 1)
   cp a,1
   jr nz,do_or_opsize_2

; *(type16 *)(arg1) |= *(type16 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(de)
   or a,(hl)
   ld (de),a
   inc de
   inc hl
   ld a,(de)
   or a,(hl)
   ld (de),a
   jr do_or_endif

do_or_opsize_2:
; else if (opsize == 2)
   cp a,2
   jr nz,do_or_endif

; *(type32 *)(arg1) |= *(type32 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ld b,4
do_or_opsize_2_loop:
   ld a,(de)
   or a,(hl)
   ld (de),a
   inc de
   inc hl
   djnz do_or_opsize_2_loop
; end-if
do_or_endif:

; cflag = vflag = 0;
   ld hl,_vflag
   ld (hl),0
   ld hl,_cflag
   ld (hl),0

; set_flags();
   jp _set_flags

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_CMP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_cmp:

   ; void do_cmp(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; memcpy(tmparg, arg1, sizeof(tmparg));
   ld de,_tmparg
   ld hl,(_arg1)
   ldi
   ldi
   ldi
   ldi

; type8 *tmp = arg1;
   ld bc,(_arg1)               ; bc = tmp

; arg1 = tmparg;
   ld hl,_tmparg
   ld (_arg1),hl

; quick_flag = 0;
   ld hl,_quick_flag
   ld (hl),0

; do_sub();
   push bc
   call _do_sub
   pop bc                      ; bc = tmp

; arg1 = tmp;
   ld (_arg1),bc

   ret

;;
;; Optimization:
;; The do_move() function has been split into several functions per size for
;; improved performance.
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_MOVE_B
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_move_b:

   ; void do_move_b(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, de, hl

; arg1[0] = arg2[0];
   ld de,(_arg1)
   ld hl,(_arg2)
   ld a,(hl)
   ld (de),a

; if (version < 2 || admode != 1)
   ld a,(_version)
   cp a,2
   jr c,do_move_b_startif
   ld a,(_admode)
   dec a
   jr z,do_move_b_endif

do_move_b_startif:
; cflag = vflag = 0;
   ld hl,_vflag
   ld (hl),0
   ld hl,_cflag
   ld (hl),0

; set_flags();
   jp _set_flags
; end-if
do_move_b_endif:

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_MOVE_W
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_move_w:

   ; void do_move_w(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; *(type16 *)(arg1) = *(type16 *)(arg2);
   ld de,(_arg1)
   ld hl,(_arg2)
   ldi
   ldi

; if (version < 2 || admode != 1)
   ld a,(_version)
   cp a,2
   jr c,do_move_w_startif
   ld a,(_admode)
   dec a
   jr z,do_move_w_endif

do_move_w_startif:
; cflag = vflag = 0;
   ld hl,_vflag
   ld (hl),0
   ld hl,_cflag
   ld (hl),0

; set_flags();
   jp _set_flags
; end-if
do_move_w_endif:

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_MOVE_L
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_move_l:

   ; void do_move_l(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

; memcpy(arg1, arg2, sizeof(type32));
   ld de,(_arg1)
   ld hl,(_arg2)
   ldi
   ldi
   ldi
   ldi

; if (version < 2 || admode != 1)
   ld a,(_version)
   cp a,2
   jr c,do_move_l_startif
   ld a,(_admode)
   dec a
   jr z,do_move_l_endif

do_move_l_startif:
; cflag = vflag = 0;
   ld hl,_vflag
   ld (hl),0
   ld hl,_cflag
   ld (hl),0

; set_flags();
   jp _set_flags
; end-if
do_move_l_endif:

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_BTST
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_btst:

   ; type8 do_btst(type8 a) __z88dk_fastcall;
   ;
   ; enter : l
   ; exit  : l
   ; uses  : af, b, de, hl

; a &= admode ? 0x7 : 0x1f;
   ld a,(_admode)
   or a,a
   jr z,do_btst_admode_z
   ld h,0x7
   jr do_btst_admode_nz
do_btst_admode_z:
   ld h,0x1f
do_btst_admode_nz:
   ld a,l
   and a,h
   ld b,a                      ; b = variable a = a & (admode ? 0x7 : 0x1f)

; if (admode == 0)
   ld a,(_admode)
   or a,a
   jr nz,do_btst_end_while

; while (a >= 8)
do_btst_while:
   ld a,b
   cp a,8
   jr c,do_btst_end_while

; a -= 8;
   sub a,8
   ld b,a                      ; b = variable a = a - 8

; arg1 -= 1;
   ld hl,(_arg1)
   dec hl
   ld (_arg1),hl
   jr do_btst_while
; end-while
do_btst_end_while:
; end-if

; zflag = 0;
   ld hl,_zflag                ; hl = _zflag
   ld (hl),0

; if ((arg1[0] & (type8) (1 << a)) == 0)
   ld de,(_arg1)
   ld a,(de)                   ; a = arg1[0]
   ld e,1
   ; b = variable a
   bsla de,b                   ; e = (type8) (1 << a)
   and a,e                     ; a = arg1[0] & (type8) (1 << a)
   jr nz,do_btst_end_if

; zflag = 0xff;
   ld (hl),0xff
; end-if
do_btst_end_if:

; return a;
   ld l,b
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _DO_BOP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_do_bop:

   ; void do_bop(type8 b, type8 a) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, bc, de, hl

   pop hl                      ; return address
   pop bc                      ; c = argument b
                               ; b = argument a
   push hl                     ; restore return address

IF LOGEMU
   push bc

; out("bop (%.2X,%.2X) ", (type16) b, (type16) a);
   ld e,b
   ld d,0
   push de
   ; c = argument b
   ld b,0
   push bc
   ld hl,print_bop_str
   push hl
   call _out
   ld hl,6
   add hl,sp
   ld sp,hl

   pop bc
ENDIF

; b = b & 0xc0;
   ld a,c
   and a,0xc0
   ld c,a                      ; c = argument b = b & 0xc0

; a = do_btst(a);
   ld l,b
   call _do_btst               ; l = return value, c is preserved
   ld b,l                      ; b = argument a = do_btst(a)

   ld a,c
; if (b == 0x00)
   or a,a
   jr nz,do_bop_else_0x40
   ; Note: This "no bop" case is more common than the actual
   ; bop cases below so we have added a fastpath for it.
IF LOGEMU
; out("no bop???");
   ld hl,print_no_bop_str
   push hl
   call _out
   pop af
ENDIF
   ret

do_bop_else_0x40:
   ; precalculations used in all else cases below
   ld hl,(_arg1)               ; hl = _arg1
   ld e,1
   ; b = argument a
   bsla de,b                   ; e = (1 << a)

; else if (b == 0x40)
   cp a,0x40
   jr nz,do_bop_else_0x80

; arg1[0] ^= (1 << a); /* bchg */
   ld a,e                      ; a = (1 << a)
   xor a,(hl)
   ld (hl),a
IF LOGEMU
; out("bchg");
   ld hl,print_bchg_str
   push hl
   call _out
   pop af
ENDIF
   ret

do_bop_else_0x80:
; else if (b == 0x80)
   cp a,0x80
   jr nz,do_bop_else_0xc0

; arg1[0] &= ((1 << a) ^ 0xff); /* bclr */
   ld a,e                      ; a = (1 << a)
   cpl                         ; a = ((1 << a) ^ 0xff)
   and a,(hl)
   ld (hl),a
IF LOGEMU
; out("bclr");
   ld hl,print_bclr_str
   push hl
   call _out
   pop af
ENDIF
   ret

do_bop_else_0xc0:
; else if (b == 0xc0)
   cp a,0xc0
   ret nz

; arg1[0] |= (1 << a); /* bset */
   ld a,e                      ; a = (1 << a)
   or a,(hl)
   ld (hl),a
IF LOGEMU
; out("bset");
   ld hl,print_bset_str
   push hl
   call _out
   pop af
ENDIF
   ret
; end-if

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _CHECK_BTST
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_check_btst:

   ; void check_btst(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

IF LOGEMU
; out("btst");
   ld hl,print_btst_str
   push hl
   call _out
   pop af
ENDIF

; set_info((type8) (byte2 & 0x3f));
   ld a,(_byte2)
   and a,0x3f
   ld l,a
   call _set_info

; set_arg1();
   call _set_arg1

; set_arg2(1, byte1);
   ld a,(_byte1)
   ld h,a
   ld l,1
   push hl
   call _set_arg2

; do_bop(byte2, arg2[0]);
   ld hl,(_arg2)
   ld a,(hl)
   ld hl,(_byte2)
   ld h,a
   push hl
   call _do_bop

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _CHECK_LEA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_check_lea:

   ; void check_lea(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

IF LOGEMU
; out("lea");
   ld hl,print_lea_str
   push hl
   call _out
   pop af
ENDIF

; if ((byte2 & 0xc0) == 0xc0)
   ld hl,_byte2
   ld a,(hl)
   and a,0xc0
   cp a,0xc0
   jr nz,check_lea_else_unimplemented

; set_info(byte2);
   ld l,(hl)
   call _set_info

; opsize = 2;
   ld hl,_opsize
   ld (hl),2

; set_arg1();
   call _set_arg1

; set_arg2(0, byte1);
   ld hl,(_byte1 - 1)
   xor a,a                     ; a = 0
   ld l,a
   push hl
   call _set_arg2

; *(type16 *)(arg2) = 0;
   ; skipped this statement since it is overwritten below if not a fatal error

; if (is_reversible)
   ld a,(_is_reversible)
   or a,a
   jr z,check_lea_else_illegal_amode

; write_l(arg2, arg1i);
   ld de,(_arg1i)
   ld bc,(_arg1i + 2)          ; bcde = arg1i
   ld hl,(_arg2)
   ld (hl),b
   inc hl
   ld (hl),c
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e                   ; write_l(arg2, bcde)
   jr check_lea_end

; else
check_lea_else_illegal_amode:
; ms_fatal("illegal addressing mode for LEA");
   ld hl,print_illegal_amode_str
   call _ms_fatal
   jr check_lea_end
; end-if

; else
check_lea_else_unimplemented:
; ms_fatal("unimplemented instruction CHK");
   ld hl,print_unimplemented_str
   call _ms_fatal
; end-if

check_lea_end:
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _CHECK_MOVEM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_check_movem:

   ; void check_movem(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

IF LOGEMU
; out("movem");
   ld hl,print_movem_str
   push hl
   call _out
   pop af
ENDIF

; set_info((type8) (byte2 - 0x40));
   ld a,(_byte2)
   sub a,0x40
   ld l,a
   call _set_info

; read_word();
   call _read_word

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; move address registers to memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; for (type8 l1c = 0; l1c < 8; l1c++)
   ld b,0                      ; b = l1c
check_movem_areg_loop:
   ld a,b
   cp a,8
   jr nc,check_movem_areg_loop_end

; if (byte2 & (type8) (1 << l1c))
   ld e,1
   ; b = l1c
   bsla de,b                   ; e = (1 << l1c)
   ld a,(_byte2)
   and a,e
   jr z,check_movem_areg_loop_endif

; set_arg1();
   push bc                     ; save b = l1c
   call _set_arg1
   pop bc                      ; restore b = l1c

; precalculate arg1
   ld de,(_arg1)               ; de = arg1

; precalculate &areg[7 - l1c]
   ld a,7
   sub a,b
   add a,a
   add a,a                     ; a = byte index
   ld hl,_areg
   add hl,a                    ; hl = &areg[7 - l1c]

; ldi below decrements bc but by setting c to 255 b will be preserved
   ld c,255

   ld a,(_opsize)
; if (opsize == 2)
   cp a,2
   jr nz,check_movem_areg_loop_else

; memcpy(arg1, &areg[7 - l1c], sizeof(type32));
   ; de = arg1
   ; hl = &areg[7 - l1c]
   ldi
   ldi
   ldi
   ldi
   jr check_movem_areg_loop_endif

check_movem_areg_loop_else:
; else if (opsize == 1)
   cp a,1
   jr nz,check_movem_areg_loop_endif

; *(type16 *)(arg1) = *(type16 *)(((type8 *) &areg[7 - l1c]) + 2);
   ; de = arg1
   ; hl = &areg[7 - l1c]
   add hl,2                    ; hl = &areg[7 - l1c]) + 2
   ldi
   ldi
check_movem_areg_loop_endif:
; end-if
; end-if

   inc b                       ; l1c++
   jr check_movem_areg_loop
check_movem_areg_loop_end:
; end-for

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; move data registers to memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; for (type8 l1c = 0; l1c < 8; l1c++)
   ld b,0                      ; b = l1c
check_movem_dreg_loop:
   ld a,b
   cp a,8
   jr nc,check_movem_dreg_loop_end

; if (byte1 & (type8) (1 << l1c))
   ld e,1
   ; b = l1c
   bsla de,b                   ; e = (1 << l1c)
   ld a,(_byte1)
   and a,e
   jr z,check_movem_dreg_loop_endif

; set_arg1();
   push bc                     ; save b = l1c
   call _set_arg1
   pop bc                      ; restore b = l1c

; precalculate arg1
   ld de,(_arg1)               ; de = arg1

; precalculate &dreg[7 - l1c]
   ld a,7
   sub a,b
   add a,a
   add a,a                     ; a = byte index
   ld hl,_dreg
   add hl,a                    ; hl = &dreg[7 - l1c]

; ldi below decrements bc but by setting c to 255 b will be preserved
   ld c,255

   ld a,(_opsize)
; if (opsize == 2)
   cp a,2
   jr nz,check_movem_dreg_loop_else

; memcpy(arg1, &dreg[7 - l1c], sizeof(type32));
   ; de = arg1
   ; hl = &dreg[7 - l1c]
   ldi
   ldi
   ldi
   ldi
   jr check_movem_dreg_loop_endif

check_movem_dreg_loop_else:
; else if (opsize == 1)
   cp a,1
   jr nz,check_movem_dreg_loop_endif

; *(type16 *)(arg1) = *(type16 *)(((type8 *) &dreg[7 - l1c]) + 2);
   ; de = arg1
   ; hl = &dreg[7 - l1c]
   add hl,2                    ; hl = &dreg[7 - l1c]) + 2
   ldi
   ldi
check_movem_dreg_loop_endif:
; end-if
; end-if

   inc b                       ; l1c++
   jr check_movem_dreg_loop
check_movem_dreg_loop_end:
; end-for

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _CHECK_MOVEM2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_check_movem2:

   ; void check_movem2(void);
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, de, hl

IF LOGEMU
; out("movem (2)");
   ld hl,print_movem2_str
   push hl
   call _out
   pop af
ENDIF

; set_info((type8) (byte2 - 0x40));
   ld a,(_byte2)
   sub a,0x40
   ld l,a
   call _set_info

; read_word();
   call _read_word

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; move memory to data registers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; for (type8 l1c = 0; l1c < 8; l1c++)
   ld b,0                      ; b = l1c
check_movem2_dreg_loop:
   ld a,b
   cp a,8
   jr nc,check_movem2_dreg_loop_end

; if (byte2 & (type8) (1 << l1c))
   ld e,1
   ; b = l1c
   bsla de,b                   ; e = (1 << l1c)
   ld a,(_byte2)
   and a,e
   jr z,check_movem2_dreg_loop_endif

; set_arg1();
   push bc                     ; save b = l1c
   call _set_arg1
   pop bc                      ; restore b = l1c

; precalculate &dreg[l1c]
   ld a,b
   add a,a
   add a,a                     ; a = byte index
   ld de,_dreg
   add de,a                    ; de = &dreg[l1c]

; precalculate arg1
   ld hl,(_arg1)               ; hl = arg1

; ldi below decrements bc but by setting c to 255 b will be preserved
   ld c,255

   ld a,(_opsize)
; if (opsize == 2)
   cp a,2
   jr nz,check_movem2_dreg_loop_else

; memcpy(&dreg[l1c], arg1, sizeof(type32));
   ; de = &dreg[l1c]
   ; hl = arg1
   ldi
   ldi
   ldi
   ldi
   jr check_movem2_dreg_loop_endif

check_movem2_dreg_loop_else:
; else if (opsize == 1)
   cp a,1
   jr nz,check_movem2_dreg_loop_endif

; *(type16 *)(((type8 *) &dreg[l1c]) + 2) = *(type16 *)(arg1);
   ; de = &dreg[l1c]
   ; hl = arg1
   add de,2                    ; de = &dreg[l1c]) + 2
   ldi
   ldi
check_movem2_dreg_loop_endif:
; end-if
; end-if

   inc b                       ; l1c++
   jr check_movem2_dreg_loop
check_movem2_dreg_loop_end:
; end-for

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; move memory to address registers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; for (type8 l1c = 0; l1c < 8; l1c++)
   ld b,0                      ; b = l1c
check_movem2_areg_loop:
   ld a,b
   cp a,8
   jr nc,check_movem2_areg_loop_end

; if (byte1 & (type8) (1 << l1c))
   ld e,1
   ; b = l1c
   bsla de,b                   ; e = (1 << l1c)
   ld a,(_byte1)
   and a,e
   jr z,check_movem2_areg_loop_endif

; set_arg1();
   push bc                     ; save b = l1c
   call _set_arg1
   pop bc                      ; restore b = l1c

; precalculate &areg[l1c]
   ld a,b
   add a,a
   add a,a                     ; a = byte index
   ld de,_areg
   add de,a                    ; de = &areg[l1c]

; precalculate arg1
   ld hl,(_arg1)               ; hl = arg1

; ldi below decrements bc but by setting c to 255 b will be preserved
   ld c,255

   ld a,(_opsize)
; if (opsize == 2)
   cp a,2
   jr nz,check_movem2_areg_loop_else

; memcpy(&areg[l1c], arg1, sizeof(type32));
   ; de = &areg[l1c]
   ; hl = arg1
   ldi
   ldi
   ldi
   ldi
   jr check_movem2_areg_loop_endif

check_movem2_areg_loop_else:
; else if (opsize == 1)
   cp a,1
   jr nz,check_movem2_areg_loop_endif

; *(type16 *)(((type8 *) &areg[l1c]) + 2) = *(type16 *)(arg1);
   ; de = &areg[l1c]
   ; hl = arg1
   add de,2                    ; de = &areg[l1c]) + 2
   ldi
   ldi
check_movem2_areg_loop_endif:
; end-if
; end-if

   inc b                       ; l1c++
   jr check_movem2_areg_loop
check_movem2_areg_loop_end:
; end-for

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DATA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

print_illegal_amode_str:
   DEFM "illegal addressing mode for LEA"
   DEFB 0

print_unimplemented_str:
   DEFM "unimplemented instruction CHK"
   DEFB 0

IF LOGEMU

SECTION rodata_user

print_admode_0_str:
   DEFM " d%.1d"
   DEFB 0

print_admode_1_str:
   DEFM " a%.1d"
   DEFB 0

print_admode_2_str:
   DEFM " (a%.1d)"
   DEFB 0

print_admode_3_str:
   DEFM " (a%.1d)+"
   DEFB 0

print_admode_4_str:
   DEFM " -(a%.1d)"
   DEFB 0

print_admode_5_str:
   DEFM " %X(a%.1d)"
   DEFB 0

print_admode_6_str:
   DEFM " %.2X(a%.1d,"
   DEFB 0

print_admode_6_areg_str:
   DEFM "a%.1d"
   DEFB 0

print_admode_6_dreg_str:
   DEFM "d%.1d"
   DEFB 0

print_admode_6_l_str:
   DEFM ".l)"
   DEFB 0

print_admode_6_w_str:
   DEFM ".w)"
   DEFB 0

print_admode_7_regnr_0_str:
   DEFM " %.4X.w"
   DEFB 0

print_admode_7_regnr_1_str:
   DEFM " %.4X"
   DEFB 0

print_admode_7_regnr_2_str:
   DEFM " %.4X(pc)"
   DEFB 0

print_admode_7_regnr_3_str:
   DEFM " ???2"
   DEFB 0

print_admode_7_regnr_4_str:
   DEFM " #%.4X"
   DEFB 0

print_pc_str:
   DEFM " %.4X"
   DEFB 0

print_bop_str:
   DEFM "bop (%.2X,%.2X) "
   DEFB 0

print_no_bop_str:
   DEFM "no bop???"
   DEFB 0

print_bchg_str:
   DEFM "bchg"
   DEFB 0

print_bclr_str:
   DEFM "bclr"
   DEFB 0

print_bset_str:
   DEFM "bset"
   DEFB 0

print_btst_str:
   DEFM "btst"
   DEFB 0

print_lea_str:
   DEFM "lea"
   DEFB 0

print_movem_str:
   DEFM "movem"
   DEFB 0

print_movem2_str:
   DEFM "movem (2)"
   DEFB 0

ENDIF
