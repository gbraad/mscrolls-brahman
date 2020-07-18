;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Stefan Bylund 2018
;;
;; The stack starts at address STACK_START and grows downwards STACK_SIZE bytes
;; to address STACK_END. This assembly file fills the stack with a predefined
;; data pattern of STACK_PATTERN bytes. This is done in order to be able to
;; detect the peak stack usage or a potential stack overflow.
;;
;; The amount of unused stack memory is determined by iterating over the number
;; of consecutive pattern bytes starting from address STACK_END and going
;; upwards. The peak stack usage is then STACK_START - used stack end + 1.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

defc STACK_START = 0xFFFF
defc STACK_END = 0xFF01
defc STACK_SIZE = STACK_START - STACK_END + 1
defc STACK_PATTERN = 0xED

; fill stack with predefined data pattern

SECTION STACK
org STACK_END

defs STACK_SIZE, STACK_PATTERN

; peak stack usage routine

SECTION code_user

PUBLIC _peak_stack_usage

_peak_stack_usage:

   ; Return peak stack usage in bytes.
   ;
   ; enter : none
   ; exit  : hl = peak stack usage
   ; uses  : af, bc, hl

   ld bc,STACK_END
find_used_stack_end_loop:
   ld a,(bc)
   cp STACK_PATTERN
   jr nz, calc_stack_usage
   ; check if stack start is reached
   ld hl,STACK_START
   xor a
   sbc hl,bc
   ret z ; stop if stack start is reached and return hl = 0
   ; otherwise continue
   inc bc
   jr find_used_stack_end_loop

calc_stack_usage:
   ; bc = used stack end
   ld hl,STACK_START
   xor a
   sbc hl,bc
   inc hl ; hl = used stack size = stack start - used stack end + 1
   ret
