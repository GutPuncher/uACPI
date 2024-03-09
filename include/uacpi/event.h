#pragma once

#include <uacpi/types.h>
#include <uacpi/uacpi.h>
#include <uacpi/acpi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum uacpi_fixed_event {
    UACPI_FIXED_EVENT_TIMER_STATUS = 1,
    UACPI_FIXED_EVENT_POWER_BUTTON,
    UACPI_FIXED_EVENT_SLEEP_BUTTON,
    UACPI_FIXED_EVENT_RTC,
    UACPI_FIXED_EVENT_MAX = UACPI_FIXED_EVENT_RTC,
} uacpi_fixed_event;

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_install_fixed_event_handler(
    uacpi_fixed_event event, uacpi_interrupt_handler handler, uacpi_handle user
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_uninstall_fixed_event_handler(
    uacpi_fixed_event event
))

/*
 * Enable/disable a fixed event. Note that the event is automatically enabled
 * upon installing a handler to it.
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
    uacpi_status uacpi_enable_fixed_event(uacpi_fixed_event event)
)
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
    uacpi_status uacpi_disable_fixed_event(uacpi_fixed_event event)
)

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
    uacpi_status uacpi_clear_fixed_event(uacpi_fixed_event event)
)

typedef enum uacpi_event_info {
    // Event is enabled in software
    UACPI_EVENT_INFO_ENABLED = (1 << 0),

    // Event is enabled in software (only for wake)
    UACPI_EVENT_INFO_ENABLED_FOR_WAKE = (1 << 1),

    // Event is masked
    UACPI_EVENT_INFO_MASKED = (1 << 2),

    // Event has a handler attached
    UACPI_EVENT_INFO_HAS_HANDLER = (1 << 3),

    // Hardware enable bit is set
    UACPI_EVENT_INFO_HW_ENABLED = (1 << 4),

    // Hardware status bit is set
    UACPI_EVENT_INFO_HW_STATUS = (1 << 5),
} uacpi_event_info;

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_fixed_event_info(
    uacpi_fixed_event event, uacpi_event_info *out_info
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_info(
   uacpi_namespace_node *gpe_devicem, uacpi_u16 idx,
   uacpi_event_info *out_info
))

// Set if the handler wishes to reenable the GPE it just handled
#define UACPI_GPE_REENABLE (1 << 7)

typedef uacpi_interrupt_ret (*uacpi_gpe_handler)(
    uacpi_handle ctx, uacpi_namespace_node *gpe_device, uacpi_u16 idx
);

typedef enum uacpi_gpe_triggering {
    UACPI_GPE_TRIGGERING_LEVEL = 0,
    UACPI_GPE_TRIGGERING_EDGE = 1,
    UACPI_GPE_TRIGGERING_MAX = UACPI_GPE_TRIGGERING_EDGE,
} uacpi_gpe_triggering;

const uacpi_char *uacpi_gpe_triggering_to_string(
    uacpi_gpe_triggering triggering
);

/*
 * Installs a handler to the provided GPE at 'idx' controlled by device
 * 'gpe_device'. The GPE is automatically disabled & cleared according to the
 * configured triggering upon invoking the handler. The event is optionally
 * re-enabled (by returning UACPI_GPE_REENABLE from the handler)
 *
 * NOTE: 'gpe_device' may be null for GPEs managed by \_GPE
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_install_gpe_handler(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx,
    uacpi_gpe_triggering triggering, uacpi_gpe_handler handler, uacpi_handle ctx
))

/*
 * Installs a raw handler to the provided GPE at 'idx' controlled by device
 * 'gpe_device'. The handler is dispatched immediately after the event is
 * received, status & enable bits are untouched.
 *
 * NOTE: 'gpe_device' may be null for GPEs managed by \_GPE
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
    uacpi_status uacpi_install_gpe_handler_raw(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx,
    uacpi_gpe_triggering triggering, uacpi_gpe_handler handler, uacpi_handle ctx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_uninstall_gpe_handler(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx, uacpi_gpe_handler handler
))

/*
 * Marks the GPE 'idx' managed by 'gpe_device' as wake-capable. 'wake_device' is
 * optional and configures the GPE to generate an implicit notification whenever
 * an even occures.
 *
 * NOTE: 'gpe_device' may be null for GPEs managed by \_GPE
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_setup_for_wake(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx,
    uacpi_namespace_node *wake_device
))

/*
 * Mark a GPE managed by 'gpe_device' as enabled/disabled for wake. The GPE must
 * have previously been marked by calling uacpi_gpe_setup_for_wake. This
 * function only affects the GPE enable register state following the call to
 * uacpi_gpe_enable_all_for_wake.
 *
 * NOTE: 'gpe_device' may be null for GPEs managed by \_GPE
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_enable_for_wake(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_disable_for_wake(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))


/*
 * Finalize GPE initialization by enabling all GPEs not configured for wake and
 * having a matching AML handler detected.
 *
 * This should be called after the kernel power managment subsystem has
 * enumerated all of the devices, executing their _PRW methods etc., and
 * marking those it wishes to use for wake by calling uacpi_setup_gpe_for_wake
 * or uacpi_mark_gpe_for_wake.
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
    uacpi_status uacpi_finalize_gpe_initialization(void)
)

/*
 * Enable/disable a general purpose event managed by 'gpe_device'. Internally
 * this uses reference counting to make sure a GPE is not disabled until all
 * possible users of it do so. GPEs not marked for wake are enabled
 * automatically so this API is only needed for wake events or those that don't
 * have a corresponding AML handler.
 *
 * NOTE: 'gpe_device' may be null for GPEs managed by \_GPE
 */
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_enable_gpe(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_disable_gpe(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_clear_gpe(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_suspend_gpe(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_resume_gpe(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_finish_handling_gpe(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_mask(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_unmask(
    uacpi_namespace_node *gpe_device, uacpi_u16 idx
))

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_disable_all(void)
)

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_enable_all_for_runtime(void)
)

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_enable_all_for_wake(void)
)

UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_install_block(
    uacpi_namespace_node *gpe_device, uacpi_u64 address,
    uacpi_address_space address_space, uacpi_u16 num_registers,
    uacpi_u32 irq
))
UACPI_ALWAYS_ERROR_FOR_REDUCED_HARDWARE(
uacpi_status uacpi_gpe_uninstall_block(
    uacpi_namespace_node *gpe_device
))

#ifdef __cplusplus
}
#endif
