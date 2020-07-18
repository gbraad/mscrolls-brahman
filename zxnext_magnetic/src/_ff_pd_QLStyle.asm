;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Phoebus Dokos 2018
;;
;; Proportional 7pt FZX font inspired by the Sinclair QL system font.
;;
;; Note: The Magnetic interpreter uses standard ASCII so the ZX Spectrum
;; character codes 94 and 96 representing up-arrow and £ are replaced with
;; their ASCII counterparts ^ and BACKQUOTE.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION rodata_font
SECTION rodata_font_fzx

PUBLIC _ff_pd_QLStyle

_ff_pd_QLStyle:

BINARY "QLStyle.fzx"
