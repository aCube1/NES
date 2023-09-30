BIN_DIR := bin
SRC_DIR := src

ROM := game.nes
EMU := fceux

SRCS := rom.s boot.s main.s
OBJS := $(SRCS:%.s=$(BIN_DIR)/%.o)

ASSEMBLER := ca65
LINKER    := ld65
ASMFLAGS  := -I include -t nes
LINKFLAGS := -C ines.cfg -m $(ROM:%.nes=%.map)

.PHONY: all debug run clean

.DEFAULT: all
.DELETE_ON_ERROR: clean

all: $(BIN_DIR) $(ROM)

debug: ASMFLAGS += -g
debug: clean all

$(BIN_DIR):
	mkdir -p $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.s
	$(ASSEMBLER) $(ASMFLAGS) $? -o $@

$(ROM): $(OBJS)
	$(LINKER) $(LINKFLAGS) $^ -o $@

run: $(ROM)
	$(EMU) $(ROM)

clean:
	@rm -rv $(OBJS) $(ROM)
