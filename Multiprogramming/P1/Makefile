CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy

QEMU := qemu-system-arm

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2

# Flags separados
CFLAGS_BBB := $(CFLAGS) -DBEAGLE
CFLAGS_QEMU := $(CFLAGS) -DQEMU

LDFLAGS_BBB := -T linker.ld -nostdlib -nostartfiles -Wl,-Map=p1_beagle.map
LDFLAGS_QEMU := -T linker_qemu.ld -nostdlib -nostartfiles -Wl,-Map=p1_qemu.map

# Objetos separados pq no me quiero pelear con esto jaja
OBJS_BBB := start_bbb.o main_bbb.o print_bbb.o
OBJS_QEMU := start_qemu.o main_qemu.o print_qemu.o

# ========================
# TARGET PRINCIPAL
# ========================
all: p1_beagle.bin p1_qemu.bin

# ========================
# COMPILACIÓN BBB
# ========================
start_bbb.o: start.s
	$(CC) $(CFLAGS_BBB) -c $< -o $@

main_bbb.o: main.c
	$(CC) $(CFLAGS_BBB) -c $< -o $@

print_bbb.o: ../lib/print.c ../lib/print.h
	$(CC) $(CFLAGS_BBB) -c ../lib/print.c -o $@

p1_beagle.elf: $(OBJS_BBB) linker.ld
	$(LD) $(CFLAGS_BBB) $(OBJS_BBB) $(LDFLAGS_BBB) -o $@

p1_beagle.bin: p1_beagle.elf
	$(OBJCOPY) -O binary $< $@

# ========================
# COMPILACIÓN QEMU
# ========================
start_qemu.o: start_qemu.s
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

main_qemu.o: main.c
	$(CC) $(CFLAGS_QEMU) -c $< -o $@

print_qemu.o: ../lib/print_qemu.c ../lib/print.h
	$(CC) $(CFLAGS_QEMU) -c ../lib/print_qemu.c -o $@

p1_qemu.elf: $(OBJS_QEMU) linker_qemu.ld
	$(LD) $(CFLAGS_QEMU) $(OBJS_QEMU) $(LDFLAGS_QEMU) -o $@

p1_qemu.bin: p1_qemu.elf
	$(OBJCOPY) -O binary $< $@

# ========================
# CLEAN
# ========================
clean:
	rm -f *.o *.elf *.bin *.map

.PHONY: all clean runqemu