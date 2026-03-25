CROSS ?= arm-none-eabi
CC := $(CROSS)-gcc
LD := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy

CFLAGS := -mcpu=cortex-a8 -marm -ffreestanding -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2
LDFLAGS := -T linker.ld -nostdlib -nostartfiles -Wl,-Map=p1.map

OBJS := start.o main.o ../lib/print.o

all: p1.bin

start.o: start.s
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

../lib/print.o: ../lib/print.c ../lib/print.h
	$(CC) $(CFLAGS) -c ../lib/print.c -o ../lib/print.o

p1.elf: $(OBJS) linker.ld
	$(LD) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

p1.bin: p1.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.bin *.map ../lib/print.o

.PHONY: all clean
