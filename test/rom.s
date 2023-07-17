.include "ines_header.i"

.CODE
LDA #$10 ; Loop counter
LDX #$00 ; Loop limit
STX $00ff ; Store limit

CMP $00ff ; Compare memory location
BEQ loop ; Is memory location equals 0x00?

loop:
	LSR ; Increment counter

	CMP $00ff ; Compare memory location with X
	BNE loop ; Return to loop if not equals

BRK
