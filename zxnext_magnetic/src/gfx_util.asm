;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Allen Albright, Stefan Bylund 2018
;;
;; Graphics utility functions. Only compiled if USE_GFX = 1.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE "zconfig.inc"

SECTION code_user

EXTERN __IO_LAYER_2_CONFIG
EXTERN __IL2C_SHOW_LAYER_2
EXTERN __REG_CLIP_WINDOW_LAYER_2
EXTERN __REG_LAYER_2_OFFSET_Y
EXTERN IOCTL_OTERM_SCROLL_LIMIT
EXTERN asm_ioctl

EXTERN _gfx_on
EXTERN _gfx_window_height
EXTERN _gfx_window_height_adjustment

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; IMAGE_HEIGHT_IN_CHARS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUBLIC image_height_in_chars

image_height_in_chars:

   ; Return image height in characters.
   ;
   ; enter : none
   ; exit  : l = a = image height in characters
   ; uses  : af, l

   ld a,(_gfx_window_height)
   add a,7
   rra
   rra
   rra
   and $1f                     ; a = height of image in characters

   ld l,a
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; _IMAGE_MOUSE_SCROLL
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUBLIC _image_mouse_scroll

_image_mouse_scroll:

   ; Scroll image up/down by the given amount (provided by the mouse).
   ; Updates _gfx_window_height_adjustment and _gfx_window_height variables.
   ;
   ; enter : l = image height change (positive if increased, negative if decreased)
   ; exit  : none
   ; uses  : af, bc, de, hl

   ld a,(_gfx_on)
   or a
   ret z                       ; return if graphics is off

   ; clip image height change if new image height would become < 0 or > MAX_IMAGE_HEIGHT
   ld a,(_gfx_window_height)
   ld b,a                      ; b = image height
   add a,l                     ; a = new image height = image height + image height change
   bit 7,l
   jr z, positive_change
negative_change:
   ; is new image height < 0 (i.e. larger than 127 in 8-bit unsigned arithmetics)?
   bit 7,a
   jr z, valid_change
   ; special care if new image height is in range [128,MAX_IMAGE_HEIGHT], which is ok
   cp MAX_IMAGE_HEIGHT + 1
   jr c, valid_change
   ; set image height change = -(image height)
   ld a,b
   neg
   ld l,a                      ; l = clipped image height change
   jr valid_change
positive_change:
   ; is new image height > MAX_IMAGE_HEIGHT ?
   cp MAX_IMAGE_HEIGHT + 1
   jr c, valid_change
   ; set image height change = MAX_IMAGE_HEIGHT - image height
   ld a,MAX_IMAGE_HEIGHT
   sub b
   ld l,a                      ; l = clipped image height change

valid_change:
   ld a,(_gfx_window_height_adjustment)
   add a,l
   ld d,a                      ; d = new window height adjustment

   ld a,(_gfx_window_height)
   ld e,a                      ; e = old window height
   add a,l                     ; a = new window height

   ; hide layer 2 screen if new window height is 0
   or a
   jr nz,show_layer2_screen
   ld bc,__IO_LAYER_2_CONFIG
   out (c),0
   jr continue

show_layer2_screen:
   ; show layer 2 screen if new window height is growing from 0
   ld b,a
   ld a,e
   or a
   ld a,b
   jr nz,continue
   ld h,__IL2C_SHOW_LAYER_2
   ld bc,__IO_LAYER_2_CONFIG
   out (c),h

continue:
   ; set the new window height adjustment from d
   ld hl,_gfx_window_height_adjustment
   ld (hl),d
   ; jump to adjust_clip_window with a = new window height
   jr adjust_clip_window

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; IMAGE_KEY_SCROLL
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUBLIC image_key_scroll

image_key_scroll:

   ; Scroll image up/down if up/down arrow key is pressed.
   ; Updates _gfx_window_height_adjustment and _gfx_window_height variables.
   ;
   ; enter : a = ascii code
   ; exit  : carry reset if up/down arrow keypress detected
   ; uses  : af, bc, de, hl

   ld hl,_gfx_window_height_adjustment

   cp ASCII_CODE_UP
   jr z, image_smaller

   cp ASCII_CODE_DOWN
   jr z, image_bigger

   scf
   ret

image_smaller:

   ; hl = _gfx_window_height_adjustment

   ld a,(_gfx_on)
   or a
   ret z                       ; if graphics is off, just reject the character

   dec (hl)                    ; indicate window height adjusted by -1

   ld a,(_gfx_window_height)
   dec a                       ; window height decreases by 1

   ld d,0
   jr z, config_layer2_screen  ; hide layer 2 screen when window height decrements to 0

   cp 0xff
   jr nz, adjust_clip_window

   inc (hl)                    ; undo window height adjust
   xor a                       ; window height is 0
   jr config_layer2_screen     ; hide layer 2 screen when window height is 0

image_bigger:

   ; hl = _gfx_window_height_adjustment

   ld a,(_gfx_on)
   or a
   ret z                       ; if graphics is off, just reject the character

   inc (hl)                    ; indicate window height adjusted by +1

   ld a,(_gfx_window_height)
   inc a                       ; window height increases by 1

   cp 1
   ld d,__IL2C_SHOW_LAYER_2
   jr z, config_layer2_screen  ; show layer 2 screen when window height increments from 0

   cp MAX_IMAGE_HEIGHT + 1
   jr c, adjust_clip_window

   dec (hl)                    ; undo window height adjust
   ld a,MAX_IMAGE_HEIGHT       ; window height is MAX_IMAGE_HEIGHT
   jr adjust_clip_window

config_layer2_screen:

   ; d = layer 2 screen config value

   ld bc,__IO_LAYER_2_CONFIG
   out (c),d

   ; continue to adjust_clip_window below

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ADJUST_CLIP_WINDOW
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

adjust_clip_window:

   ; Adjust the layer 2 clip window height, scroll the layer 2 screen vertically
   ; and update the scroll limit of the output terminal.
   ;
   ; Updates _gfx_window_height_adjustment and _gfx_window_height variables.
   ;
   ; enter : a = new window height
   ; exit  : none
   ; uses  : af, bc, hl

   ld (_gfx_window_height),a

   ; set new layer 2 clip window height

   ld b,a                      ; b = stored window height
   or a
   jr z, set_clip_reg          ; y2 = 0
   dec a                       ; y2 = window height - 1
set_clip_reg:
   nextreg __REG_CLIP_WINDOW_LAYER_2,0    ; x1
   nextreg __REG_CLIP_WINDOW_LAYER_2,255  ; x2
   nextreg __REG_CLIP_WINDOW_LAYER_2,0    ; y1
   nextreg __REG_CLIP_WINDOW_LAYER_2,a    ; y2

   ; scroll the layer 2 screen vertically so that the bottom-most part of its
   ; contents is always visible in the bottom part of the layer 2 clip window

   ld a,MAX_IMAGE_HEIGHT
   sub b
   nextreg __REG_LAYER_2_OFFSET_Y,a  ; MAX_IMAGE_HEIGHT - height

   ; update the scroll limit of the output terminal

IF __SDCC_IY
   push iy
   call update_scroll_limit
   pop iy
ELSE
   push ix
   call update_scroll_limit
   pop ix
ENDIF

   or a                        ; reject the character
   ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; UPDATE_SCROLL_LIMIT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

update_scroll_limit:

   ; Update the scroll limit of the output terminal.
   ; Also updates _gfx_window_height_adjustment variable
   ; to remainder after scroll limit update.
   ;
   ; enter : none
   ; exit  : none
   ; uses  : af, bc, hl

   ; if the accumulated graphics window height adjustments change by at least
   ; one character, update the scroll limit of the output terminal accordingly

   ; is abs(_gfx_window_height_adjustment) >= 8 ?
   ld a,(_gfx_window_height_adjustment)
   bit 7,a
   jr z, positive_check
   neg
positive_check:
   cp 8
   jr c, check_image_height_alignment

   ; current scroll limit = ioctl(1, IOCTL_OTERM_SCROLL_LIMIT, -1)
IF __SDCC
   ld hl,-1
   push hl
   ld hl,IOCTL_OTERM_SCROLL_LIMIT
   push hl
   ld hl,1
   push hl
ELSE
   ld hl,1
   push hl
   ld hl,IOCTL_OTERM_SCROLL_LIMIT
   push hl
   ld hl,-1
   push hl
ENDIF
   call asm_ioctl              ; hl = return value
   pop af
   pop af
   pop af
   ld c,l                      ; c = current scroll limit

   ; height adjustment in rows = _gfx_window_height_adjustment / 8
   ld a,(_gfx_window_height_adjustment)
   bit 7,a
   jr z, positive_div
   add a,7
positive_div:
   sra a
   sra a
   sra a
   ld b,a                      ; b = height adjustment in rows

   ; new scroll limit = current scroll limit - height adjustment in rows
   ld a,c
   sub b                       ; a = new scroll limit (set to 0 below if negative)
   bit 7,a
   jr z, set_new_scroll_limit
   xor a

set_new_scroll_limit:
   ; ioctl(1, IOCTL_OTERM_SCROLL_LIMIT, new_scroll_limit)
IF __SDCC
   ld h,0
   ld l,a
   push hl
   ld hl,IOCTL_OTERM_SCROLL_LIMIT
   push hl
   ld hl,1
   push hl
ELSE
   ld hl,1
   push hl
   ld hl,IOCTL_OTERM_SCROLL_LIMIT
   push hl
   ld h,0
   ld l,a
   push hl
ENDIF
   call asm_ioctl
   pop af
   pop af
   pop af

   ; remainder after scroll limit update (_gfx_window_height_adjustment %= 8)
   ld a,(_gfx_window_height_adjustment)
   ld b,a
   bit 7,a
   jr z, positive_mod_1
   neg
positive_mod_1:
   and 0x07
   bit 7,b
   jr z, positive_mod_2
   neg
positive_mod_2:
   ld (_gfx_window_height_adjustment),a

   ; extra safety: reset accumulated graphics window height
   ; adjustments if image height is aligned on a text line boundary

check_image_height_alignment:
   ; is _gfx_window_height % 8 = 0 ?
   ld a,(_gfx_window_height)
   and 0x07
   ret nz
   xor a
   ld (_gfx_window_height_adjustment),a
   ret
