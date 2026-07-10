/**
 * @file    tusb_config.h
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   TinyUSB device configuration for USB HID Bridge
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Target MCU and OS (bare-metal, no RTOS)
#define CFG_TUSB_MCU              OPT_MCU_RP2350
#define CFG_TUSB_OS               OPT_OS_NONE

// Device class support — only HID enabled
#define CFG_TUD_HID               1
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

// Endpoint sizes and task scheduling
#define CFG_TUD_HID_EP_BUFSIZE    64
#define CFG_TUD_ENDPOINT0_SIZE    64
#define CFG_TUSB_TASK_INTERVAL    10

#ifdef __cplusplus
}
#endif
