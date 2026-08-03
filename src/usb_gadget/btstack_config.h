/**
 * @file    btstack_config.h
 * @author  Overboard Team
 * @date    2026-07-12
 *
 * @brief   BTstack configuration for Classic Bluetooth HID device on Pico 2 W
 *
 * Minimal config for a BR/EDR HID keyboard gadget.
 * No BLE, no A2DP, no RFCOMM — just L2CAP + SDP + HID device profile.
 */
#pragma once

// ── Classic BT features ──────────────────────────────────────────────────────
#define ENABLE_CLASSIC
#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE

// ── Logging ───────────────────────────────────────────────────────────────────
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_INFO

// ── Buffer / pool sizes ───────────────────────────────────────────────────────
#define HCI_OUTGOING_PRE_BUFFER_SIZE            4
#define HCI_ACL_PAYLOAD_SIZE                    1024
#define HCI_ACL_CHUNK_SIZE_ALIGNMENT            4

// Limit ACL/SCO buffers to avoid CYW43 shared-bus overrun
#define MAX_NR_CONTROLLER_ACL_BUFFERS           3
#define MAX_NR_CONTROLLER_SCO_PACKETS           3

// Enable host-side flow control to prevent CYW43 shared-bus overrun
#define ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL
#define HCI_HOST_ACL_PACKET_LEN                 1024
#define HCI_HOST_ACL_PACKET_NUM                 3
#define HCI_HOST_SCO_PACKET_LEN                 120
#define HCI_HOST_SCO_PACKET_NUM                 3

// Connection / service record pool sizes
#define MAX_NR_HCI_CONNECTIONS                  2
#define MAX_NR_L2CAP_CHANNELS                   4
#define MAX_NR_L2CAP_SERVICES                   2
#define MAX_NR_SERVICE_RECORD_ITEMS             2
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES 2
#define MAX_NR_SM_LOOKUP_ENTRIES                2
#define MAX_NR_WHITELIST_ENTRIES                1

// Persistent storage: link keys in flash via TLV
#define NVM_NUM_LINK_KEYS                       4

// ── Platform / HAL ────────────────────────────────────────────────────────────
#define HAVE_EMBEDDED_TIME_MS
#define HAVE_ASSERT

// Reset timeout — some CYW43 variants need extra time
#define HCI_RESET_RESEND_TIMEOUT_MS             1000
