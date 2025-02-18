/*
 * Adapted from STM32F405 SoC
 * Minimal Cortex M4 platform
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "exec/address-spaces.h"
#include "hw/arm/cortexm4_minimal_soc.h"
#include "hw/qdev-clock.h"
#include "hw/misc/unimp.h"

static void cortexm4_minimal_soc_initfn(Object *obj)
{
    CORTEXM4_MINIMALState *s = CORTEXM4_MINIMAL_SOC(obj);

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
}

static void cortexm4_minimal_soc_realize(DeviceState *dev_soc, Error **errp)
{
    CORTEXM4_MINIMALState *s = CORTEXM4_MINIMAL_SOC(dev_soc);
    MemoryRegion *system_memory = get_system_memory();
    DeviceState *armv7m;
    Error *err = NULL;

    memory_region_init_rom(&s->flash, OBJECT(dev_soc), "generic.flash",
                           FLASH_SIZE, &err);
    if (err != NULL) {
        error_propagate(errp, err);
        return;
    }

    memory_region_add_subregion(system_memory, FLASH_BASE_ADDRESS, &s->flash);

    memory_region_init_ram(&s->sram, NULL, "generic.sram", SRAM_SIZE,
                           &err);
    if (err != NULL) {
        error_propagate(errp, err);
        return;
    }

    memory_region_add_subregion(system_memory, SRAM_BASE_ADDRESS, &s->sram);

    armv7m = DEVICE(&s->armv7m);

    qdev_prop_set_uint32(armv7m, "num-irq", 96);
    qdev_prop_set_uint8(armv7m, "num-prio-bits", 4);
    qdev_prop_set_string(armv7m, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m4"));
    qdev_prop_set_bit(armv7m, "enable-bitband", true);

    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->sysclk);

    object_property_set_link(OBJECT(&s->armv7m), "memory",
                             OBJECT(system_memory), &error_abort);

    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }

    create_unimplemented_device("dummy", 0x40000000, 0x400);
}

static void cortexm4_minimal_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = cortexm4_minimal_soc_realize;
}

static const TypeInfo cortexm4_minimal_soc_info = {
    .name          = TYPE_CORTEXM4_MINIMAL_SOC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(CORTEXM4_MINIMALState),
    .instance_init = cortexm4_minimal_soc_initfn,
    .class_init    = cortexm4_minimal_soc_class_init,
};

static void cortexm4_minimal_soc_types(void)
{
    type_register_static(&cortexm4_minimal_soc_info);
}

type_init(cortexm4_minimal_soc_types)
