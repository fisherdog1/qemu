#ifndef HW_CORTEXM4_MINIMAL_SOC_H
#define HW_CORTEXM4_MINIMAL_SOC_H

#include "hw/arm/armv7m.h"
#include "qom/object.h"

#define TYPE_CORTEXM4_MINIMAL_SOC "cortexm4-minimal-soc"
OBJECT_DECLARE_SIMPLE_TYPE(CORTEXM4_MINIMALState, CORTEXM4_MINIMAL_SOC)

#define FLASH_BASE_ADDRESS 0x00000000
#define FLASH_SIZE (1024 * 1024)
#define SRAM_BASE_ADDRESS 0x20000000
#define SRAM_SIZE (128 * 1024)

struct CORTEXM4_MINIMALState {
    SysBusDevice parent_obj;

    ARMv7MState armv7m;

    MemoryRegion sram;
    MemoryRegion flash;

    Clock *sysclk;
};

#endif
