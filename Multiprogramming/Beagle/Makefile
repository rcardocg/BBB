CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump

ROOT := ..
BUILD := build

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2 -DBEAGLE
LDFLAGS := -nostdlib -nostartfiles

OS_OBJS := \
	$(BUILD)/os/root.o \
	$(BUILD)/os/os.o \
	$(BUILD)/os/pcb.o \
	$(BUILD)/os/uart.o \
	$(BUILD)/os/timer.o \
	$(BUILD)/os/wdt.o \
	$(BUILD)/os/round_robin.o

P1_OBJS := \
	$(BUILD)/P1/start.o \
	$(BUILD)/P1/main.o \
	$(BUILD)/P1/print.o

P2_OBJS := \
	$(BUILD)/P2/start.o \
	$(BUILD)/P2/main.o \
	$(BUILD)/P2/print.o

all: os_beagle.bin p1_beagle.bin p2_beagle.bin

$(BUILD)/os $(BUILD)/P1 $(BUILD)/P2:
	mkdir -p $@

$(BUILD)/os/root.o: $(ROOT)/os/root.s | $(BUILD)/os
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/os/os.o: $(ROOT)/os/os.c $(ROOT)/os/pcb.h $(ROOT)/os/cpu.h $(ROOT)/os/drivers/uart.h $(ROOT)/os/drivers/wdt.h $(ROOT)/os/drivers/timer.h $(ROOT)/os/scheduler/scheduler.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/pcb.o: $(ROOT)/os/pcb.c $(ROOT)/os/pcb.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/uart.o: $(ROOT)/os/drivers/uart.c $(ROOT)/os/drivers/uart.h $(ROOT)/os/drivers/mmio.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/timer.o: $(ROOT)/os/drivers/timer.c $(ROOT)/os/drivers/timer.h $(ROOT)/os/drivers/mmio.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/wdt.o: $(ROOT)/os/drivers/wdt.c $(ROOT)/os/drivers/wdt.h $(ROOT)/os/drivers/mmio.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/round_robin.o: $(ROOT)/os/scheduler/round_robin.c $(ROOT)/os/scheduler/scheduler.h $(ROOT)/os/pcb.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

os_beagle.elf: $(OS_OBJS) $(ROOT)/os/linker.ld
	$(LD) $(CFLAGS) $(OS_OBJS) -T $(ROOT)/os/linker.ld $(LDFLAGS) -Wl,-Map=os_beagle.map -o $@

os_beagle.bin: os_beagle.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/P1/start.o: $(ROOT)/P1/start.s | $(BUILD)/P1
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/P1/main.o: $(ROOT)/P1/main.c $(ROOT)/lib/print.h | $(BUILD)/P1
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P1/print.o: $(ROOT)/lib/print.c $(ROOT)/lib/print.h | $(BUILD)/P1
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

p1_beagle.elf: $(P1_OBJS) $(ROOT)/P1/linker.ld
	$(LD) $(CFLAGS) $(P1_OBJS) -T $(ROOT)/P1/linker.ld $(LDFLAGS) -Wl,-Map=p1_beagle.map -o $@

p1_beagle.bin: p1_beagle.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/P2/start.o: $(ROOT)/P2/start.s | $(BUILD)/P2
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/P2/main.o: $(ROOT)/P2/main.c $(ROOT)/lib/print.h | $(BUILD)/P2
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P2/print.o: $(ROOT)/lib/print.c $(ROOT)/lib/print.h | $(BUILD)/P2
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

p2_beagle.elf: $(P2_OBJS) $(ROOT)/P2/linker.ld
	$(LD) $(CFLAGS) $(P2_OBJS) -T $(ROOT)/P2/linker.ld $(LDFLAGS) -Wl,-Map=p2_beagle.map -o $@

p2_beagle.bin: p2_beagle.elf
	$(OBJCOPY) -O binary $< $@

disasm: os_beagle.elf
	$(OBJDUMP) -D os_beagle.elf > os_beagle.dis

clean:
	rm -rf $(BUILD) *.o *.elf *.bin *.map *.dis

.PHONY: all clean disasm
