CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2
LDFLAGS := -T linker.ld -nostdlib -nostartfiles

.SUFFIXES:

all: os.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

os.elf: root.o os.o linker.ld
	$(LD) $(CFLAGS) root.o os.o $(LDFLAGS) -Wl,-Map=os.map -o $@

os.bin: os.elf
	$(OBJCOPY) -O binary $< $@

disasm: os.elf
	$(OBJDUMP) -D os.elf > os.dis

clean:
	rm -f *.o *.elf *.bin *.map *.dis

.PHONY: all clean disasm
