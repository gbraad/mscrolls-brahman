; ===============================================================
; Sep 2018
; ===============================================================
;
; void in_mouse_kempston(uint8_t *buttons, uint16_t *x, uint16_t *y)
;
; Returns mouse coordinates and button state.
;
; Based on the Kempston mouse driver by Chris Cowley (Copyright 2003).
;
; Replaces the original Kempston mouse driver in z88dk.
;
; ===============================================================

SECTION code_clib
SECTION code_input

PUBLIC asm_in_mouse_kempston

EXTERN __input_kempston_mouse_x
EXTERN __input_kempston_mouse_y
EXTERN __input_kempston_mouse_rawx
EXTERN __input_kempston_mouse_rawy

asm_in_mouse_kempston:

   ; exit  :  a = button state = 0000 0MLR active high
   ;         de = x-coordinate
   ;         bc = y-coordinate
   ;
   ; uses  : af, bc, de, hl

;mouse_x:
   ; Read Kempston x-coordinate
   ld a,(__input_kempston_mouse_rawx)
   ld d,a
   ld bc,64479
   in a,(c)
   ld (__input_kempston_mouse_rawx),a
   sub d                           ; a = signed x-displacement
   sra a                           ; scale it down by a factor of 2
   ld b,a                          ; b = scaled and signed x-displacement
   ; Mouse left/right?
   cp $80
   jr nc, moving_left

;moving_right:
   ld a,(__input_kempston_mouse_x)
   add a,b
   ld b,a                          ; b = new x-coordinate = old x-coordinate + x-displacement
   ld a,(__input_kempston_mouse_x) ; a = old x-coordinate
   cp b
   jr z, mouse_y
   jr nc, right_edge
   ld a,b
   ld (__input_kempston_mouse_x),a
   jr mouse_y
right_edge:
   ld a,255
   ld (__input_kempston_mouse_x),a
   jr mouse_y

moving_left:
   ld a,(__input_kempston_mouse_x)
   add a,b
   ld b,a                          ; b = new x-coordinate
   ld a,(__input_kempston_mouse_x) ; a = old x-coordinate
   cp b
   jr c, left_edge
   ld a,b
   ld (__input_kempston_mouse_x),a
   jr mouse_y
left_edge:
   xor a
   ld (__input_kempston_mouse_x),a

mouse_y:
   ; Read Kempston y-coordinate
   ld a,(__input_kempston_mouse_rawy)
   ld d,a
   ld bc,65503
   in a,(c)
   ld (__input_kempston_mouse_rawy),a
   sub d                           ; a = signed y-displacement
   sra a                           ; scale it down by a factor of 2
   ld b,a                          ; b = scaled and signed y-displacement
   ; Note: Kempston mouse y-axis is flipped, reversed by subtraction below.
   ld a,(__input_kempston_mouse_y)
   sub b                           ; a = new y-coordinate = old y-coordinate - y-displacement
   cp 191
   jr nc, set_vert_edge
   ld (__input_kempston_mouse_y),a
   jr mouse_btn

set_vert_edge:
   ; Mouse up/down?
   ld a,(__input_kempston_mouse_y) ; a = old y-coordinate
   cp 96
   jr c, set_top_edge
;set_bottom_edge:
   ld a,191
   ld (__input_kempston_mouse_y),a
   jr mouse_btn
set_top_edge:
   xor a
   ld (__input_kempston_mouse_y),a

mouse_btn:
   ; Read Kempston button state
   ld bc,64223
   in a,(c)
   xor $07                     ; Invert button bits
   and $07                     ; Only keep button bits
   ; a = buttons = 0000 0MLR active high

   ; Deliver mouse coordinates.
   ld hl,(__input_kempston_mouse_x)
   ex de,hl                    ; de = new x-coordinate
   ld hl,(__input_kempston_mouse_y)
   ld c,l
   ld b,h                      ; bc = new y-coordinate
   ret
