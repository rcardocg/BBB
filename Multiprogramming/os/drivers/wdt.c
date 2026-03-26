#include "wdt.h"
#include "mmio.h"

#define WDT1_BASE       0x44E35000u
#define WDT_WSPR        0x48u
#define WDT_WWPS        0x34u
#define WDT_WWPS_W_PEND (1u << 4)

void wdt_disable(void) {
    mmio_write(WDT1_BASE + WDT_WSPR, 0xAAAAu);
    while ((mmio_read(WDT1_BASE + WDT_WWPS) & WDT_WWPS_W_PEND) != 0u) {
    }

    mmio_write(WDT1_BASE + WDT_WSPR, 0x5555u);
    while ((mmio_read(WDT1_BASE + WDT_WWPS) & WDT_WWPS_W_PEND) != 0u) {
    }
}
