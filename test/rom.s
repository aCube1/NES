.include "ines_header.i"

.CODE
LDA #$0a ; Loop limit
LDX #$00 ; Loop counter
STX $00ff ; Store counter

CMP $00ff ; Compare memory location
BEQ loop ; Is memory location equals 0x00?

loop:
	INC $00ff ; Increment counter

	CMP $00ff ; Compare memory location with X
	BNE loop ; Return to loop if not equals

BRK
