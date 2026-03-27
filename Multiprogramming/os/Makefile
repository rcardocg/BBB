CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2

CFLAGS_BBB := $(CFLAGS) -DBEAGLE
CFLAGS_QEMU := $(CFLAGS) -DQEMU

LDFLAGS_BBB := -T linker.ld -nostdlib -nostartfiles -Wl,-Map=os_beagle.map
LDFLAGS_QEMU := -T linker_qemu.ld -nostdlib -nostartfiles -Wl,-Map=os_qemu.map

# Objetos separados
OBJS_BBB := \
root_bbb.o \
os_bbb.o \
pcb_bbb.o \
drivers/uart_bbb.o \
drivers/timer_bbb.o \
drivers/wdt_bbb.o \
scheduler/round_robin_bbb.o
OBJS_QEMU := root_qemu.o os_qemu.o pcb_qemu.o timer_qemu.o intc_qemu.o

.SUFFIXES:

all: os_beagle.bin os_qemu.bin

# ========================
# BBB
# ========================
root_bbb.o: root.s
	$(CC) $(CFLAGS_BBB) -c $< -o $@

os_bbb.o: os.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

pcb_bbb.o: pcb.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

drivers/uart_bbb.o: drivers/uart.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

drivers/timer_bbb.o: drivers/timer.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

drivers/wdt_bbb.o: drivers/wdt.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

scheduler/round_robin_bbb.o: scheduler/round_robin.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

os_beagle.elf: $(OBJS_BBB) linker.ld
	$(LD) $(CFLAGS_BBB) $(OBJS_BBB) $(LDFLAGS_BBB) -o $@

os_beagle.bin: os_beagle.elf
	$(OBJCOPY) -O binary $< $@

# ========================
# QEMU
# ========================
root_qemu.o: root.s
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

os_qemu.o: os_qemu.c
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

pcb_qemu.o: pcb.c
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

timer_qemu.o: timer_qemu.c
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

intc_qemu.o: intc_qemu.c
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

os_qemu.elf: $(OBJS_QEMU) linker_qemu.ld
	$(LD) $(CFLAGS_QEMU) $(OBJS_QEMU) $(LDFLAGS_QEMU) -o $@

os_qemu.bin: os_qemu.elf
	$(OBJCOPY) -O binary $< $@

# ========================
# UTILIDADES
# ========================
disasm: os_qemu.elf
	$(OBJDUMP) -D os_qemu.elf > os.dis

disasm_beagle: os_beagle.elf
	$(OBJDUMP) -D os_beagle.elf > os_beagle.dis

clean:
	rm -f *.o *.elf *.bin *.map *.dis

.PHONY: all clean disasm