.include "ines_header.i"

.CODE
LDX #$00 ; Loop counter
CPX #$00 ; is X == 00
BEQ loop

loop:
	INX ; Increment counter

	CPX #$0a ; is X == 0a
	BNE loop

BRK
