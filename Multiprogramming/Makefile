CROSS ?= arm-none-eabi

all: os_all p1 p2

os_all:
	$(MAKE) -C os CROSS=$(CROSS)

p1:
	$(MAKE) -C P1 CROSS=$(CROSS)

p2:
	$(MAKE) -C P2 CROSS=$(CROSS)

clean:
	$(MAKE) -C os clean
	$(MAKE) -C P1 clean
	$(MAKE) -C P2 clean

.PHONY: all clean os_all p1 p2
