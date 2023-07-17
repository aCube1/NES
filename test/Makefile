ROM := rom.nes

BUILD_DIR := build
SRCS := rom.s
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

ASMFLAGS := --cpu 6502
LINKFLAGS := --config ines.cfg

$(ROM): $(OBJS)
	ld65 $^ -o $@ $(LINKFLAGS)
	cat bin/hdr.bin bin/prg.bin bin/chr.bin > $@

$(OBJS): $(SRCS)
	ca65 $(ASMFLAGS) $^ -o $@

.PHONY: clean
.DELETE_ON_ERROR: clean
clean:
	rm *.o
	rm bin/*.bin
