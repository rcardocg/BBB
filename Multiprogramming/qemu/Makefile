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

CPU_OBJS := \
	$(BUILD)/os/cpu_read_cpsr.o \
	$(BUILD)/os/cpu_write_cpsr_c.o \
	$(BUILD)/os/cpu_enable_irq.o \
	$(BUILD)/os/cpu_read_dfsr.o \
	$(BUILD)/os/cpu_read_ifsr.o \
	$(BUILD)/os/cpu_get_user_regs.o \
	$(BUILD)/os/cpu_set_user_regs.o \
	$(BUILD)/os/cpu_switch_to_user_mode.o

OS_OBJS := \
	$(BUILD)/os/root.o \
	$(BUILD)/os/pcb.o \
	$(BUILD)/os/timer.o \
	$(BUILD)/os/intc.o \
	$(BUILD)/os/os_uart_putc.o \
	$(BUILD)/os/os_uart_puts.o \
	$(BUILD)/os/os_wdt_disable.o \
	$(BUILD)/os/os_pcb_system_init.o \
	$(BUILD)/os/os_syscall_dispatcher.o \
	$(BUILD)/os/os_fault_handler.o \
	$(BUILD)/os/os_timer_irq_handler.o \
	$(BUILD)/os/os_trace_mode_switch.o \
	$(BUILD)/os/os_trace_fault.o \
	$(BUILD)/os/os_kmain.o \
	$(CPU_OBJS)

P1_OBJS := \
	$(BUILD)/P1/start.o \
	$(BUILD)/P1/main.o \
	$(BUILD)/P1/print.o \
	$(BUILD)/P1/syscall.o

P2_OBJS := \
	$(BUILD)/P2/start.o \
	$(BUILD)/P2/main.o \
	$(BUILD)/P2/print.o \
	$(BUILD)/P2/syscall.o

all: os_qemu.bin p1_qemu.bin p2_qemu.bin

$(BUILD)/os $(BUILD)/P1 $(BUILD)/P2:
	mkdir -p $@

# CPU Module
$(BUILD)/os/cpu_%.o: $(ROOT)/os/cpu/cpu_%.c | $(BUILD)/os
	$(CC) $(CFLAGS) -c $< -o $@

# OS Core Atomic Files
$(BUILD)/os/os_%.o: $(ROOT)/os/qemu/src/os_%.c | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os/qemu -I$(ROOT)/os/cpu -c $< -o $@

$(BUILD)/os/root.o: $(ROOT)/os/root.s | $(BUILD)/os
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/os/pcb.o: $(ROOT)/os/pcb.c $(ROOT)/os/pcb.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/timer.o: $(ROOT)/os/qemu/timer.c $(ROOT)/os/timer.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

$(BUILD)/os/intc.o: $(ROOT)/os/qemu/intc.c $(ROOT)/os/intc.h | $(BUILD)/os
	$(CC) $(CFLAGS) -I$(ROOT)/os -c $< -o $@

os_qemu.elf: $(OS_OBJS) $(ROOT)/os/qemu/linker.ld
	$(LD) $(CFLAGS) $(OS_OBJS) -T $(ROOT)/os/qemu/linker.ld $(LDFLAGS) -Wl,-Map=os_qemu.map -o $@

os_qemu.bin: os_qemu.elf
	$(OBJCOPY) -O binary $< $@

# P1 & P2
$(BUILD)/P1/start.o: $(ROOT)/P1/qemu/start.s | $(BUILD)/P1
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/P1/main.o: $(ROOT)/P1/main.c $(ROOT)/lib/print.h | $(BUILD)/P1
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P1/print.o: $(ROOT)/lib/qemu/print.c $(ROOT)/lib/print.h | $(BUILD)/P1
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P1/syscall.o: $(ROOT)/lib/qemu/syscall.s | $(BUILD)/P1
	$(CC) $(CFLAGS) -c $< -o $@

p1_qemu.elf: $(P1_OBJS) $(ROOT)/P1/qemu/linker.ld
	$(LD) $(CFLAGS) $(P1_OBJS) -T $(ROOT)/P1/qemu/linker.ld $(LDFLAGS) -Wl,-Map=p1_qemu.map -o $@

p1_qemu.bin: p1_qemu.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/P2/start.o: $(ROOT)/P2/qemu/start.s | $(BUILD)/P2
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/P2/main.o: $(ROOT)/P2/main.c $(ROOT)/lib/print.h | $(BUILD)/P2
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P2/print.o: $(ROOT)/lib/qemu/print.c $(ROOT)/lib/print.h | $(BUILD)/P2
	$(CC) $(CFLAGS) -I$(ROOT)/lib -c $< -o $@

$(BUILD)/P2/syscall.o: $(ROOT)/lib/qemu/syscall.s | $(BUILD)/P2
	$(CC) $(CFLAGS) -c $< -o $@

p2_qemu.elf: $(P2_OBJS) $(ROOT)/P2/qemu/linker.ld
	$(LD) $(CFLAGS) $(P2_OBJS) -T $(ROOT)/P2/qemu/linker.ld $(LDFLAGS) -Wl,-Map=p2_qemu.map -o $@

p2_qemu.bin: p2_qemu.elf
	$(OBJCOPY) -O binary $< $@

run: all
	$(QEMU) -M versatilepb -cpu cortex-a8 -nographic -audio none -kernel os_qemu.bin -device loader,file=p1_qemu.bin,addr=0x00020000 -device loader,file=p2_qemu.bin,addr=0x00030000

disasm: os_qemu.elf
	$(OBJDUMP) -D os_qemu.elf > os_qemu.dis

clean:
	rm -rf $(BUILD) *.elf *.bin *.map *.dis

.PHONY: all clean disasm run
