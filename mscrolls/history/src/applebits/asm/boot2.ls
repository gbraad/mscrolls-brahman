   1                   
   2                   
   3                   ;#########################################################################
   4                   ;#
   5                   ;#       %W%
   6                   ;#       %G%
   7                   ;#
   8                   ;#########################################################################
   9                   
  10                   
  11                   ;***************************************
  12                   ;* BOOT2 - continues boot process by   *
  13                   ;* using RWTS loaded by BOOT1 to grab  *
  14                   ;* the rest of the runtime code.       *
  15                   ;***************************************
  16                   
  17         00:BD     RWTS_ENTRY =  $BD00               ;This is in the file rwtsboot.asm
  18                   
  19         46:BF             *=    $BF46
  20                   
  21         46:BF     BOOT2
  22  46:BF  8E 85 BF          STX   IOBOOT+1    ;Works from any slot
  23  49:BF  8E 93 BF          STX   IOBOOT+15
  24  4C:BF  86 FF             STX   $FF                 ;Save slot index
  25         4E:BF     REBOOT
  26  4E:BF  A9 BF             LDA   #IOBOOT/256
  27  50:BF  A0 84             LDY   #IOBOOT%256
  28  52:BF  20 99 BF          JSR   ENTER_RWTS
  29  55:BF  B0 F7             BCS   REBOOT
  30  57:BF  EE 8D BF          INC   BOOPAGE
  31  5A:BF  AD 89 BF          LDA   BOOSECT
  32  5D:BF  C9 0F             CMP   #15
  33  5F:BF  F0 06             BEQ   UPTRACK
  34  61:BF  EE 89 BF          INC   BOOSECT
  35  64:BF  4C 4E BF          JMP   REBOOT
  36         67:BF     UPTRACK
  37  67:BF  EE 88 BF          INC   BOOTRACK
  38  6A:BF  AD 88 BF          LDA   BOOTRACK
  39  6D:BF  C9 04             CMP   #$04
  40  6F:BF  F0 08             BEQ   BOOT_DONE
  41  71:BF  A9 00             LDA   #$00
  42  73:BF  8D 89 BF          STA   BOOSECT
  43  76:BF  4C 4E BF          JMP   REBOOT
  44         79:BF     BOOT_DONE
  45  79:BF  A6 FF             LDX   $FF
  46  7B:BF  BD 8A C0          LDA   $C08A,X             ;Engage drive 1
  47  7E:BF  BD 89 C0          LDA   $C089,X             ;Switch it on
  48  81:BF  4C 00 60          JMP   $6000               ;Go unhuff emu
  49                   
  50                   
  51         84:BF     IOBOOT
  52  84:BF  01 60 01                .byte      1,$60,1,$68
      87:BF  68        
  53  88:BF  00        BOOTRACK .byte 0
  54  89:BF  00        BOOSECT       .byte      0
  55  8A:BF  95                      .byte      BOODCT%256
  56  8B:BF  BF                      .byte      BOODCT/256
  57  8C:BF  00                      .byte      0
  58  8D:BF  60        BOOPAGE       .byte      $60
  59  8E:BF  00 00                   .byte      0,0
  60  90:BF  01                      .byte      1
  61  91:BF  00                      .byte      0
  62  92:BF  68 60 01                .byte      $68,$60,1
  63  95:BF  00 01 EF  BOODCT        .byte      0,1,$EF,$D8
      98:BF  D8        
  64                   
  65         99:BF     ENTER_RWTS
  66  99:BF  08                PHP
  67  9A:BF  78                SEI
  68  9B:BF  20 00 BD          JSR   RWTS_ENTRY
  69  9E:BF  B0 03             BCS   BADBOOT
  70  A0:BF  28                PLP
  71  A1:BF  18                CLC
  72  A2:BF  60                RTS
  73         A3:BF     BADBOOT
  74  A3:BF  28                PLP
  75  A4:BF  38                SEC
  76  A5:BF  60                RTS
  77                   ;* Do NOT add anything after this *
