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

// Activate USB device stack on port 0.
// This sets CFG_TUD_ENABLED via the TUD_RHPORT_MODE fallback in tusb_option.h.
// Without this, all tud_* APIs are compiled out.
#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE)

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
