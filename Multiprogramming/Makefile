CROSS ?= arm-none-eabi
QEMU := qemu-system-arm

all: qemu beagle

qemu:
	$(MAKE) -C qemu CROSS=$(CROSS) QEMU=$(QEMU)

beagle:
	$(MAKE) -C Beagle CROSS=$(CROSS)

runqemu:
	$(MAKE) -C qemu CROSS=$(CROSS) QEMU=$(QEMU) run

clean:
	$(MAKE) -C qemu clean
	$(MAKE) -C Beagle clean

.PHONY: all clean qemu beagle runqemu
