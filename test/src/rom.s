.import nmi, reset

.segment "HEADER"
.byte "NES", $1a  ; Magical signiature
.byte $01         ; 1x 16KB ROM (PRG)
.byte $01         ; 1x 8KB VROM (CHR)
.byte $01         ; Mapper 0 = NROM (No Mapper)
.byte $00         ; Mirroring 0001 = Vertical Mirroring

.segment "VECTORS"
  .addr nmi
  .addr reset
  .addr reset ; We don't need to call any IRQ subroutine
