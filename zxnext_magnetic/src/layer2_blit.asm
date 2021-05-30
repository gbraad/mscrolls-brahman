;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Stefan Bylund 2020
;;
;; Layer 2 blit functions, see layer2_blit.h for API documentation.
;;
;; Note: MMU slot 2 is temporarily used when accessing the layer 2 screen.
;;
;; This module was first developed in C and then compiled to Z80 assembly and
;; manually optimized.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION code_user

PUBLIC _layer2_blit
PUBLIC _layer2_blit_trans
PUBLIC _layer2_blit_screen
PUBLIC _layer2_blit_screen_section

EXTERN __IO_NEXTREG_REG

defc MMU_PAGE_SIZE = 0x2000
defc MMU0_ADDRESS = 0
defc MMU2_ADDRESS = 0x4000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _LAYER2_BLIT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_layer2_blit:

   ; void layer2_blit(layer2_blit_context_t *blit_context) __z88dk_fastcall;
   ;
   ; enter : hl = &layer2_blit_context_t
   ; exit  : none
   ; uses  : af, bc, de, hl, ix
   ;
   ; typedef struct layer2_blit_context
   ; {
   ;     uint8_t screen_base_page;          ix + 0
   ;     uint8_t *src;                      ix + 1, ix + 2   (modified)
   ;     uint16_t src_line_width;           ix + 3, ix + 4
   ;     uint8_t x;                         ix + 5
   ;     uint8_t y;                         ix + 6           (modified)
   ;     uint16_t width;                    ix + 7, ix + 8
   ;     uint8_t height;                    ix + 9           (modified)
   ; } layer2_blit_context_t;

   push ix                     ; save ix
   push hl
   pop ix                      ; ix = &layer2_blit_context_t

   ; The x, y, width, height arguments should be validated by the caller.

; uint8_t page = screen_base_page + (y >> 5);
   ld a,(ix+6)                 ; a = y
   rlca
   rlca
   rlca
   and a,0x07                  ; a = y >> 5
   add a,(ix+0)
   ld c,a                      ; c = page = screen_base_page + (y >> 5)

; while (height)
blit_loop:
   ld a,(ix+9)
   or a,a
   jr z,blit_end

; ZXN_WRITE_MMU2(page++);
   ld a,c
   inc c
   mmu2 a

; uint8_t section_y = y % 32;
   ld a,(ix+6)
   and a,0x1F
   ld d,a                      ; d = section_y = y % 32

; uint8_t section_height = 32 - section_y;
   ld a,32
   sub a,d
   ld b,a                      ; b = section_height = 32 - section_y

; if (section_height > height)
   ld a,(ix+9)
   cp a,b
   jr nc,blit_perform
; section_height = height;
   ld b,(ix+9)
; end-if

blit_perform:
; uint8_t *dst = MMU2_ADDRESS + x + (section_y << 8);
   ld e,0                      ; de = section_y << 8
   ld l,(ix+5)
   ld h,0x40                   ; hl = MMU2_ADDRESS + x
   add hl,de                   ; hl = dst = MMU2_ADDRESS + x + (section_y << 8)

   push bc                     ; save bc = section_height, page

   ; Blit a rectangle, line by line, of the specified width (1-256) and height
   ; (1-32) from a source bitmap to a destination bitmap.
   ex de,hl                    ; de = dst
   ld l,(ix+1)
   ld h,(ix+2)                 ; hl = src
   ld a,b                      ; a = num_lines = section_height
   ld c,(ix+7)
   ld b,(ix+8)                 ; bc = width
blit_inner_loop:
   push bc
   push de
   push hl
   ldir                        ; blit one line (width pixels) from src to dst
   pop hl
   ld e,(ix+3)
   ld d,(ix+4)
   add hl,de                   ; hl = src = src + src_line_width
   pop de
   inc d                       ; de = dst = dst + 256
   pop bc
   dec a                       ; num_lines--
   jr nz,blit_inner_loop

   pop bc                      ; restore bc = section_height, page

; src += section_height * src_line_width;
   ; multiplication of an 8-bit number (1-32) and a 16-bit number (1-256)
   ld a,b
   ld e,a                      ; e = section_height (1-32)
   ld d,(ix+3)                 ; d = src_line_width (1-256)
   dec d                       ; d--
   mul de                      ; de = d * e
   add de,a                    ; de += a
   ; de = section_height * src_line_width
   ld a,(ix+1)
   add a,e
   ld (ix+1),a
   ld a,(ix+2)
   adc a,d
   ld (ix+2),a

; y += section_height;
   ld a,(ix+6)
   add a,b
   ld (ix+6),a

; height -= section_height;
   ld a,(ix+9)
   sub a,b
   ld (ix+9),a

   jp blit_loop
; end-while

blit_end:
; ZXN_WRITE_MMU2(10);
   ld a,10
   mmu2 a

   pop ix                      ; restore ix
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _LAYER2_BLIT_TRANS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_layer2_blit_trans:

   ; void layer2_blit_trans(layer2_blit_context_t *blit_context) __z88dk_fastcall;
   ;
   ; enter : hl = &layer2_blit_context_t
   ; exit  : none
   ; uses  : af, af', bc, de, hl, ix
   ;
   ; typedef struct layer2_blit_context
   ; {
   ;     uint8_t screen_base_page;          ix + 0
   ;     uint8_t *src;                      ix + 1, ix + 2   (modified)
   ;     uint16_t src_line_width;           ix + 3, ix + 4
   ;     uint8_t x;                         ix + 5
   ;     uint8_t y;                         ix + 6           (modified)
   ;     uint16_t width;                    ix + 7, ix + 8
   ;     uint8_t height;                    ix + 9           (modified)
   ;     uint8_t transparency_color;        ix + 10
   ; } layer2_blit_context_t;

   push ix                     ; save ix
   push hl
   pop ix                      ; ix = &layer2_blit_context_t

   ; The x, y, width, height arguments should be validated by the caller.

; uint8_t page = screen_base_page + (y >> 5);
   ld a,(ix+6)                 ; a = y
   rlca
   rlca
   rlca
   and a,0x07                  ; a = y >> 5
   add a,(ix+0)
   ld c,a                      ; c = page = screen_base_page + (y >> 5)

; while (height)
blit_trans_loop:
   ld a,(ix+9)
   or a,a
   jr z,blit_trans_end

; ZXN_WRITE_MMU2(page++);
   ld a,c
   inc c
   mmu2 a

; uint8_t section_y = y % 32;
   ld a,(ix+6)
   and a,0x1F
   ld d,a                      ; d = section_y = y % 32

; uint8_t section_height = 32 - section_y;
   ld a,32
   sub a,d
   ld b,a                      ; b = section_height = 32 - section_y

; if (section_height > height)
   ld a,(ix+9)
   cp a,b
   jr nc,blit_trans_perform
; section_height = height;
   ld b,(ix+9)
; end-if

blit_trans_perform:
; uint8_t *dst = MMU2_ADDRESS + x + (section_y << 8);
   ld e,0                      ; de = section_y << 8
   ld l,(ix+5)
   ld h,0x40                   ; hl = MMU2_ADDRESS + x
   add hl,de                   ; hl = dst = MMU2_ADDRESS + x + (section_y << 8)

   push bc                     ; save bc = section_height, page

   ; Blit a rectangle, line by line, of the specified width (1-256) and height
   ; (1-32) from a source bitmap to a destination bitmap. Transparent pixels
   ; in the source bitmap are skipped.
   ex de,hl                    ; de = dst
   ld l,(ix+1)
   ld h,(ix+2)                 ; hl = src
   ld a,b                      ; a = num_lines = section_height
   ld c,(ix+7)
   ld b,(ix+8)                 ; bc = width
   ex af,af'
   ld a,(ix+10)
   ex af,af'                   ; a' = transparency color index
blit_trans_inner_loop:
   push bc
   push de
   push hl
   ex af,af'                   ; a = transparency color index
   ldirx                       ; blit one line (width pixels) from src to dst
   pop hl
   ld e,(ix+3)
   ld d,(ix+4)
   add hl,de                   ; hl = src = src + src_line_width
   pop de
   inc d                       ; de = dst = dst + 256
   pop bc
   ex af,af'                   ; a = num_lines
   dec a                       ; num_lines--
   jr nz,blit_trans_inner_loop

   pop bc                      ; restore bc = section_height, page

; src += section_height * src_line_width;
   ; multiplication of an 8-bit number (1-32) and a 16-bit number (1-256)
   ld a,b
   ld e,a                      ; e = section_height (1-32)
   ld d,(ix+3)                 ; d = src_line_width (1-256)
   dec d                       ; d--
   mul de                      ; de = d * e
   add de,a                    ; de += a
   ; de = section_height * src_line_width
   ld a,(ix+1)
   add a,e
   ld (ix+1),a
   ld a,(ix+2)
   adc a,d
   ld (ix+2),a

; y += section_height;
   ld a,(ix+6)
   add a,b
   ld (ix+6),a

; height -= section_height;
   ld a,(ix+9)
   sub a,b
   ld (ix+9),a

   jp blit_trans_loop
; end-while

blit_trans_end:
; ZXN_WRITE_MMU2(10);
   ld a,10
   mmu2 a

   pop ix                      ; restore ix
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _LAYER2_BLIT_SCREEN
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_layer2_blit_screen:

   ; void layer2_blit_screen(layer2_screen_t dst, layer2_screen_t src) __z88dk_callee;
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, bc, de, hl

   pop hl                      ; return address
   pop de                      ; d = src, e = dst
   push hl                     ; restore return address

; uint8_t src_screen_base_page = GET_SCREEN_BASE_PAGE(src);
   ld l,d
   ld bc,__IO_NEXTREG_REG
   out (c),l
   inc b
   in d,(c)                    ; d = src_screen_base_bank
   sla d                       ; d = src_screen_base_page

; uint8_t dst_screen_base_page = GET_SCREEN_BASE_PAGE(dst);
   ld l,e
   ld bc,__IO_NEXTREG_REG
   out (c),l
   inc b
   in e,(c)                    ; e = dst_screen_base_bank
   sla e                       ; e = dst_screen_base_page

; for (uint8_t page = 0; page < 6; page++)
   ld b,0                      ; b = page = 0
blit_screen_loop:
   ld a,b
   cp a,6
   jr nc,blit_screen_end

; ZXN_WRITE_MMU0(src_screen_base_page + page);
   ld a,d
   add a,b
   mmu0 a

; ZXN_WRITE_MMU2(dst_screen_base_page + page);
   ld a,e
   add a,b
   mmu2 a

; memcpy(MMU2_ADDRESS, MMU0_ADDRESS, MMU_PAGE_SIZE);
   push de                     ; save de = src_screen_base_page, dst_screen_base_page
   push bc                     ; save b = page
   ld de,MMU2_ADDRESS
   ld hl,MMU0_ADDRESS
   ld bc,MMU_PAGE_SIZE
   ldir                        ; memcpy
   pop bc                      ; restore b
   pop de                      ; restore de

   inc b                       ; b = page++
   jr blit_screen_loop
; end-for

blit_screen_end:
; ZXN_WRITE_MMU0(255);
   ld a,255
   mmu0 a

; ZXN_WRITE_MMU2(10);
   ld a,10
   mmu2 a

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _LAYER2_BLIT_SCREEN_SECTION
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_layer2_blit_screen_section:

   ; void layer2_blit_screen_section(uint8_t *dst,
   ;                                 uint8_t src_screen_base_page,
   ;                                 uint8_t y,
   ;                                 uint8_t height);
   ;
   ; enter : stack = arguments
   ; exit  : none
   ; uses  : af, bc, de, hl, ix
   ;
   ; Stack:
   ; ix + 8: uint8_t height
   ; ix + 7: uint8_t y
   ; ix + 6: uint8_t src_screen_base_page
   ; ix + 5: uint8_t *dst
   ; ix + 4: -"-
   ; ix + 3: return address
   ; ix + 2: -"-
   ; ix + 1: ix
   ; ix + 0: -"-
   ; ix - 1: uint8_t page

   push ix                     ; save ix
   ld ix,0
   add ix,sp                   ; ix = sp

; uint8_t page = src_screen_base_page + (y >> 5);
   ld a,(ix+7)                 ; a = y
   rlca
   rlca
   rlca
   and a,0x07                  ; a = y >> 5
   add a,(ix+6)                ; a = page = src_screen_base_page + (y >> 5)
   push af                     ; store page on stack at ix-1

; uint16_t addr = (y % 32) << 8;
   ld a,(ix+7)
   and a,0x1F
   ld d,a
   ld e,0                      ; de = addr = (y % 32) << 8

; uint16_t size = height << 8;
   ld h,(ix+8)
   ld l,0                      ; hl = size = height << 8

; while (size)
blit_screen_section_loop:
   ld a,h
   or a,l
   jr z,blit_screen_section_end

; ZXN_WRITE_MMU2(page++);
    pop af                     ; a = (ix-1) = page
    mmu2 a
    inc a
    push af                    ; store page++ on stack at ix-1

; uint16_t buffer_size = MMU_PAGE_SIZE - addr;
   xor a,a
   sub a,e
   ld c,a
   ld a,0x20
   sbc a,d
   ld b,a                      ; bc = buffer_size = MMU_PAGE_SIZE - addr

; if (buffer_size > size)
   ld a,l
   sub a,c
   ld a,h
   sbc a,b
   jr nc,blit_screen_section_perform
; buffer_size = size;
   ld c,l
   ld b,h
; end-if

blit_screen_section_perform:
; memcpy(dst, MMU2_ADDRESS + addr, buffer_size);
   push hl                     ; save hl = size
   push de                     ; save de = addr
   push bc                     ; save bc = buffer_size
   ld hl,MMU2_ADDRESS
   add hl,de                   ; hl = src = MMU2_ADDRESS + addr
   ld e,(ix+4)
   ld d,(ix+5)                 ; de = dst = ix+5,ix+4
   ldir                        ; memcpy
   pop bc                      ; restore bc = buffer_size

; dst += buffer_size;
   ; de has already been incremented by ldir to dst + buffer_size
   ld (ix+4),e
   ld (ix+5),d

; addr = (addr + buffer_size) % MMU_PAGE_SIZE;
   pop hl                      ; restore hl = addr
   add hl,bc
   ld a,h
   and a,0x1F
   ld e,l
   ld d,a                      ; de = addr = (addr + buffer_size) % MMU_PAGE_SIZE

   pop hl                      ; restore hl = size
; size -= buffer_size;
   xor a,a
   sbc hl,bc

   jr blit_screen_section_loop
; end-while

blit_screen_section_end:
; ZXN_WRITE_MMU2(10);
   ld a,10
   mmu2 a

   pop af                      ; pop page variable at ix-1
   pop ix                      ; restore ix
   ret
