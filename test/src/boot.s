.export reset

.import loop

.include "ppu.inc"

.macro wait_vblank
  BIT PPU_STATUS

: BIT PPU_STATUS
  BPL :-
.endmacro

.macro clear_ram
  LDA #$00
  LDX #$00

: STA $000, X
  STA $100, X
  STA $200, X
  STA $300, X
  STA $400, X
  STA $500, X
  STA $600, X
  STA $700, X
  INX
  BNE :-
.endmacro

.segment "CODE"
.proc reset
; Disable IRQs and Decimal Mode
  SEI
  CLD ; Not used in NES

; Disable NMI and Rendering
  LDX #$00
  STX PPU_CTRL
  STX PPU_MASK

; Reset stack pointer
  DEX ; X = #$ff
  TXS

  wait_vblank
  clear_ram
  wait_vblank

  JMP loop
.endproc
