CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2
LDFLAGS := -T linker.ld -nostdlib -nostartfiles

# List of object files
OBJS := root.o os.o pcb.o \
        drivers/uart.o drivers/wdt.o drivers/timer.o \
        scheduler/round_robin.o

.SUFFIXES:

all: os.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

os.elf: $(OBJS) linker.ld
	$(LD) $(CFLAGS) $(OBJS) $(LDFLAGS) -Wl,-Map=os.map -o $@

os.bin: os.elf
	$(OBJCOPY) -O binary $< $@

disasm: os.elf
	$(OBJDUMP) -D os.elf > os.dis

clean:
	rm -f *.o *.elf *.bin *.map *.dis
	rm -f drivers/*.o scheduler/*.o

.PHONY: all clean disasm
