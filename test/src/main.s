.export nmi, loop

.segment "CODE"
.proc nmi
  RTI
.endproc

.proc loop
	JMP loop
.endproc
