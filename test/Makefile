ROM := rom.nes

BUILD_DIR := build
SRCS := rom.s
OBJS := $(SRCS:%.s=$(BUILD_DIR)/%.o)

ASMFLAGS := -t nes
LINKFLAGS := --config ines.cfg

.PHONY: default clean
.DELETE_ON_ERROR: clean

default: all
all: $(ROM)

$(BUILD_DIR):
	-mkdir $(BUILD_DIR)

$(OBJS): $(SRCS) $(BUILD_DIR)
	ca65 $(ASMFLAGS) $< -o $@

$(ROM): $(OBJS)
	ld65 $^ -o $@ $(LINKFLAGS)

clean:
	rm -r $(OBJS)
