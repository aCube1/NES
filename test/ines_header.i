.SEGMENT "HEADER"
.BYTE "NES", $1a

.BYTE $01       ; 1x 16KB ROM (PRG)
.BYTE $01       ; 1x 8KB VROM (CHR)
.BYTE %00000001 ; Mapper nibble 0000 == No Mapper
                ; Mirroring nibble 0001 == Vertical Mirroring
