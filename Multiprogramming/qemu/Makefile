CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump
QEMU ?= qemu-system-arm

ROOT := ..
BUILD := build

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2 -DQEMU
LDFLAGS := -nostdlib -nostartfiles

OS_OBJS := \
	$(BUILD)/os/root.o \
	$(BUILD)/os/os.o \
	$(BUILD)/os/pcb.o \
	$(BUILD)/os/timer.o \
	$(BUILD)/os/intc.o

P1_OBJS := \
	$(BUILD)/P1/start.o \
	$(BUILD)/P1/main.o \
	$(BUILD)/P1/print.o

P2_OBJS := \
	$(BUILD)/P2/start.o \
	$(BUILD)/P2/main.o \
	$(BUILD)/P2/print.o

all: os_qemu.bin p1_qemu.bin p2_qemu.bin

$(BUILD)/os $(BUILD)/P1 $(BUILD)/P2:
	mkdir -p $@

$(BUILD)/os/root.o: $(ROOT)/os/root.s | $(BUILD)/os
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/os/os.o: $(ROOT)/os/os_qemu.c $(ROOT)/os/pcb.h $(ROOT)/os/timer.h $(ROOT)/os/intc.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/pcb.o: $(ROOT)/os/pcb.c $(ROOT)/os/pcb.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/timer.o: $(ROOT)/os/timer_qemu.c $(ROOT)/os/timer.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/intc.o: $(ROOT)/os/intc_qemu.c $(ROOT)/os/intc.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

os_qemu.elf: $(OS_OBJS) $(ROOT)/os/linker_qemu.ld
	$(LD) $(CFLAGS) $(OS_OBJS) -T $(ROOT)/os/linker_qemu.ld $(LDFLAGS) -Wl,-Map=os_qemu.map -o $@

os_qemu.bin: os_qemu.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/P1/start.o: $(ROOT)/P1/start_qemu.s | $(BUILD)/P1
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/P1/main.o: $(ROOT)/P1/main.c $(ROOT)/lib/print.h | $(BUILD)/P1
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P1/print.o: $(ROOT)/lib/print_qemu.c $(ROOT)/lib/print.h | $(BUILD)/P1
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

p1_qemu.elf: $(P1_OBJS) $(ROOT)/P1/linker_qemu.ld
	$(LD) $(CFLAGS) $(P1_OBJS) -T $(ROOT)/P1/linker_qemu.ld $(LDFLAGS) -Wl,-Map=p1_qemu.map -o $@

p1_qemu.bin: p1_qemu.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/P2/start.o: $(ROOT)/P2/start_qemu.s | $(BUILD)/P2
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/P2/main.o: $(ROOT)/P2/main.c $(ROOT)/lib/print.h | $(BUILD)/P2
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P2/print.o: $(ROOT)/lib/print_qemu.c $(ROOT)/lib/print.h | $(BUILD)/P2
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

p2_qemu.elf: $(P2_OBJS) $(ROOT)/P2/linker_qemu.ld
	$(LD) $(CFLAGS) $(P2_OBJS) -T $(ROOT)/P2/linker_qemu.ld $(LDFLAGS) -Wl,-Map=p2_qemu.map -o $@

p2_qemu.bin: p2_qemu.elf
	$(OBJCOPY) -O binary $< $@

run: all
	$(QEMU) -M versatilepb -cpu cortex-a8 -nographic -kernel os_qemu.bin -device loader,file=p1_qemu.bin,addr=0x00020000 -device loader,file=p2_qemu.bin,addr=0x00030000

disasm: os_qemu.elf
	$(OBJDUMP) -D os_qemu.elf > os_qemu.dis

clean:
	rm -rf $(BUILD) *.o *.elf *.bin *.map *.dis

.PHONY: all clean disasm run
