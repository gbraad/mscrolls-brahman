;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Stefan Bylund 2019
;;
;; FZX proportional font drawing function for layer 2.
;; See layer2_fzx.h for information on how to use it.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION code_user

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _LAYER2_FZX_SET_STATE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUBLIC _layer2_fzx_set_state

_layer2_fzx_set_state:

   ; Set layer 2 specific FZX state.
   ;
   ; enter : hl = & struct layer2_fzx_state
   ; exit  : none
   ; uses  : af, hl

   ld a,(hl)
   ld (base_page),a

   inc hl
   ld a,(hl)
   ld (color),a

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; __LAYER2_FZX_DRAW
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUBLIC __layer2_fzx_draw

__layer2_fzx_draw:

   ; Draw one row in a glyph bitmap on a layer 2 screen memory area.
   ; Converts ULA screen address to screen x,y coordinates and then
   ; back to layer 2 screen address.
   ;
   ; Note: Must not modify ix, b, e, af', hl.
   ;
   ; enter : hl = ULA screen address
   ;         dca = bytes representing a glyph bitmap row
   ; exit  : none
   ; uses  : af, c, d

   push hl
   push af
   push bc
   push de

   ; calculate ULA screen x,y coordinates
   call zx_saddr2pxy            ; input hl = screen address, output bc = xy

   ; page = base_page + (y / 32)
   ld a,c
   rlca
   rlca
   rlca
   and 0x07
   ld hl,base_page
   add a,(hl)                  ; a = page

   ; swap in page in mmu slot 2
   mmu2 a

   ; layer 2 screen address = 0x4000 + x + (y % 32) << 8
   ld a,c
   and 0x1f
   ld h,a
   ld l,0                      ; hl = (y % 32) << 8
   ld de,0x4000
   add hl,de                   ; hl = 0x4000 + (y % 32) << 8
   ld d,0
   ld e,b
   add hl,de                   ; hl = 0x4000 + x + (y % 32) << 8

   ; draw bitmap byte d
   ld a,(color)
   ld c,a
   pop de
   ld a,d
   ld d,c
   call draw_bitmap_byte       ; hl = screen address, a = bitmap byte d, d = color

   ; draw bitmap byte c
   pop bc
   push bc
   ld a,c
   call draw_bitmap_byte       ; hl = next screen address, a = bitmap byte c, d = color

   ; draw bitmap byte a (often 0)
   pop bc
   pop af
   push bc
   or a
   call nz, draw_bitmap_byte   ; hl = next screen address, a = bitmap byte a, d = color

   ; restore default page 10 in mmu slot 2
   mmu2 10

   pop bc
   pop hl

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; zx_saddr2pxy
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

zx_saddr2pxy:

   ; Return pixel x,y coordinates corresponding to the given ULA screen address.
   ;
   ; enter : hl = ULA screen address
   ; exit  : b = pixel x coordinate of leftmost pixel in byte at screen address
   ;         c = pixel y coordinate at screen address
   ; uses  : af, bc

   ; calculate x coordinate

   ld a,l
   add a,a
   add a,a
   add a,a
   ld b,a                      ; b = x coordinate

   ; calculate y coordinate

   ld a,l
   rra
   rra
   and 0x38
   ld c,a

   ld a,h
   add a,a
   add a,a
   add a,a
   and 0xc0
   or c
   ld c,a

   ld a,h
   and 0x07
   or c
   ld c,a                      ; c = y coordinate

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DRAW_BITMAP_BYTE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

draw_bitmap_byte:

   ; Draw pixels for the set bits in the given bitmap byte starting
   ; at the given layer 2 screen address using the given color.
   ;
   ; enter : hl = layer 2 screen address
   ;          a = bitmap byte
   ;          d = color (layer 2 palette index)
   ; exit  : hl = screen address for next bitmap byte
   ; uses  : af, b, hl

   ld b,8                      ; b = loop counter
loop:
   bit 7,a
   jr z, next_pixel
   ld (hl),d                   ; draw pixel
next_pixel:
   sla a
   inc hl
   djnz loop

   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DATA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION data_user

base_page:
   DEFB 0

color:
   DEFB 0
