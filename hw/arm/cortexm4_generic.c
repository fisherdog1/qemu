/*
 * Adapted from Olimex SoC
 * Minimal Cortex M4 platform
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "qemu/error-report.h"
#include "hw/arm/cortexm4_minimal_soc.h"
#include "hw/arm/boot.h"

/* Extremely generic Cortex M4 machine for development */

/* Main frequency in Hz (1MHz) */
#define SYSCLK_FRQ 1000000ULL

static void cortex_m4_init(MachineState *machine)
{
    DeviceState *dev;
    Clock *sysclk;

    /* This clock doesn't need migration because it is fixed-frequency */
    sysclk = clock_new(OBJECT(machine), "CPUCLK");
    clock_set_hz(sysclk, SYSCLK_FRQ);

    /* Ram added here??? */
    /* Key thing that adds SOC and peripherals */
    dev = qdev_new(TYPE_CORTEXM4_MINIMAL_SOC);
    object_property_add_child(OBJECT(machine), "soc", OBJECT(dev));

    qdev_connect_clock_in(dev, "sysclk", sysclk);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    armv7m_load_kernel(CORTEXM4_MINIMAL_SOC(dev)->armv7m.cpu,
                       machine->kernel_filename,
                       0, FLASH_SIZE);
}

static void cortex_m4_machine_init(MachineClass *mc)
{
    static const char * const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m4"),
        NULL
    };

    mc->desc = "Generic Cortex M4";
    mc->init = cortex_m4_init;
    mc->valid_cpu_types = valid_cpu_types;

    /* SRAM pre-allocated as part of the SoC instantiation */
    mc->default_ram_size = 0;
}

DEFINE_MACHINE("cortex-m4-generic", cortex_m4_machine_init)
