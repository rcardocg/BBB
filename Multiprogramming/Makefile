CROSS ?= arm-none-eabi
QEMU := qemu-system-arm

all: os_all p1 p2

os_all:
	$(MAKE) -C os CROSS=$(CROSS)

p1:
	$(MAKE) -C P1 CROSS=$(CROSS)

p2:
	$(MAKE) -C P2 CROSS=$(CROSS)

runqemu: all
	$(QEMU) -M versatilepb -cpu cortex-a8 -nographic -kernel os/os_qemu.bin -device loader,file=P1/p1_qemu.bin,addr=0x00020000 -device loader,file=P2/p2_qemu.bin,addr=0x00030000

clean:
	$(MAKE) -C os clean
	$(MAKE) -C P1 clean
	$(MAKE) -C P2 clean

.PHONY: all clean os_all p1 p2
