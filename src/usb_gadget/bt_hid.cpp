/**
 * @file    bt_hid.cpp
 * @author  Overboard Team
 * @date    2026-07-12
 *
 * @brief   Bluetooth HID Keyboard Handler Implementation
 *
 * Uses BTstack's hid_device API to present a Classic Bluetooth HID keyboard
 * + consumer-control device.  Report format mirrors the USB HID descriptors
 * already defined in usb_descriptors.c so the Pi Zero UART protocol is
 * transport-agnostic.
 */

#include "bt_hid.hpp"

// C++ Standard Libraries
#include <cctype>
#include <cstdio>
#include <cstring>

// Pico SDK
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"

// BTstack (pico-sdk 2.2.0 / BTstack v1.6.2)
#include "btstack.h"
#include "classic/hid_device.h"
#include "gap.h"

// ── HID descriptor ────────────────────────────────────────────────────────────
// Mirrors usb_descriptors.c: keyboard (Report ID 1) + consumer control (ID 2).
// BTstack registers this blob so the host uses the same report layout over BT.
static const uint8_t hid_descriptor[] = {
    // --- Keyboard (Report ID 1) ---
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    // Modifier byte
    0x05, 0x07,        //   Usage Page (Keyboard)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    // Reserved byte
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x03,        //   Input (Constant)
    // Keycodes (6-KRO)
    0x05, 0x07,        //   Usage Page (Keyboard)
    0x19, 0x00,        //   Usage Minimum (0)
    0x29, 0xFF,        //   Usage Maximum (255)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xFF,        //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x06,        //   Report Count (6)
    0x81, 0x00,        //   Input (Data, Array, Absolute)
    // LED output
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (1)
    0x29, 0x05,        //   Usage Maximum (5)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x05,        //   Report Count (5)
    0x91, 0x02,        //   Output (Data, Variable, Absolute)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x03,        //   Report Size (3)
    0x91, 0x03,        //   Output (Constant)
    0xC0,              // End Collection

    // --- Consumer Control (Report ID 2) ---
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x0F,  //   Logical Maximum (4095)
    0x05, 0x0C,        //   Usage Page (Consumer)
    0x19, 0x00,        //   Usage Minimum (0)
    0x2A, 0xFF, 0x0F,  //   Usage Maximum (4095)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x00,        //   Input (Data, Array, Absolute)
    0xC0               // End Collection
};

// ── Static instance pointer for BTstack C callback trampoline ─────────────────
BT_HID* BT_HID::s_instance = nullptr;

// ── BTstack registration storage ─────────────────────────────────────────────
static btstack_packet_callback_registration_t hci_event_callback_reg;

/**************************/
/*       Constructor      */
/**************************/
BT_HID::BT_HID() {
    s_instance = this;
}

/**************************/
/*          Init          */
/**************************/
void BT_HID::init() {
    // Initialise CYW43 (needed for BT hardware access on Pico 2 W)
    if (cyw43_arch_init() != 0) {
        printf("[BT_HID] cyw43_arch_init failed\n");
        return;
    }

    // L2CAP is the base layer for Classic BT profiles
    l2cap_init();

    // SDP (Service Discovery Protocol) — lets hosts find our HID service
    sdp_init();

    // Set local name visible during pairing
    gap_set_local_name("Overboard Calculator");

    // Allow the device to be discoverable and connectable
    gap_discoverable_control(1);
    gap_connectable_control(1);

    // Classic BT security: fixed PIN mode
    // gap_set_security_level(LEVEL_2) implicitly enables authentication for Classic BT.
    // TODO (ICD 0x06): replace with runtime PIN once Pi Zero → Pico PIN message exists
    gap_set_security_level(LEVEL_2);

    // Register HID device profile (SDK 2.2.0: no storage parameter)
    hid_device_init(false,                      // boot_protocol_mode_supported
                    sizeof(hid_descriptor),
                    hid_descriptor);

    // Register our packet handler for HCI + HID events
    hci_event_callback_reg.callback = &BT_HID::packet_handler_cb;
    hci_add_event_handler(&hci_event_callback_reg);
    hid_device_register_packet_handler(&BT_HID::packet_handler_cb);

    // SDP record so hosts discover us as a HID keyboard
    static uint8_t hid_sdp_record_buf[300];
    static const hid_sdp_record_t hid_sdp_params = {
        .hid_device_subclass      = 0x0540,  // keyboard + pointing device subclass
        .hid_country_code         = 33,
        .hid_virtual_cable        = 1,
        .hid_remote_wake          = 0,
        .hid_reconnect_initiate   = 1,
        .hid_normally_connectable = true,
        .hid_boot_device          = false,
        .hid_ssr_host_max_latency = 0xFFFF,
        .hid_ssr_host_min_timeout = 0xFFFF,
        .hid_supervision_timeout  = 0,
        .hid_descriptor           = hid_descriptor,
        .hid_descriptor_size      = sizeof(hid_descriptor),
        .device_name              = "Overboard Calculator",
    };
    hid_create_sdp_record(hid_sdp_record_buf, 0x10001, &hid_sdp_params);
    sdp_register_service(hid_sdp_record_buf);

    // Power on the BT controller
    hci_power_control(HCI_POWER_ON);

    m_bt_ready = true;
    printf("[BT_HID] Initialized. Device name: 'Overboard Calculator', PIN: %s\n",
           BT_FIXED_PIN);
}

/**************************/
/*          Tick          */
/**************************/
void BT_HID::tick() {
    if (!m_bt_ready) {
        return;
    }
    process_text_queue();
}

/**************************/
/*        connected       */
/**************************/
bool BT_HID::connected() const {
    return m_hid_cid != 0;
}

/****************************/
/*     send_key_press       */
/****************************/
void BT_HID::send_key_press(uint8_t modifiers, uint8_t keycode) {
    send_keyboard_report(modifiers, keycode);
}

/****************************/
/*     send_key_release     */
/****************************/
void BT_HID::send_key_release() {
    send_keyboard_report(0, 0);
}

/****************************/
/*   send_consumer_control  */
/****************************/
void BT_HID::send_consumer_control(uint16_t usage_id) {
    send_consumer_report(usage_id);
}

/****************************/
/*        queue_text        */
/****************************/
std::size_t BT_HID::queue_text(const char* text) {
    std::size_t queued = 0;
    for (std::size_t i = 0; text[i] != '\0' && queued < TEXT_QUEUE_SIZE; ++i) {
        uint8_t modifiers = 0;
        uint8_t keycode = char_to_hid_keycode(text[i], modifiers);
        if (keycode != 0) {
            queue_push(keycode, modifiers);
            ++queued;
        }
    }
    return queued;
}

/****************************/
/*     text_queue_empty     */
/****************************/
bool BT_HID::text_queue_empty() const {
    return m_queue_head == m_queue_tail;
}

// ── Private helpers ────────────────────────────────────────────────────────────

/****************************/
/*   send_keyboard_report   */
/****************************/
void BT_HID::send_keyboard_report(uint8_t modifiers, uint8_t keycode) {
    if (!connected()) {
        return;
    }
    // Boot-protocol keyboard report: Report ID + modifiers + reserved + 6 keycodes
    uint8_t report[9];
    report[0] = REPORT_ID_KEYBOARD;
    report[1] = modifiers;
    report[2] = 0x00;  // reserved
    report[3] = keycode;
    report[4] = 0x00;
    report[5] = 0x00;
    report[6] = 0x00;
    report[7] = 0x00;
    report[8] = 0x00;
    hid_device_send_interrupt_message(m_hid_cid, report, sizeof(report));
}

/****************************/
/*   send_consumer_report   */
/****************************/
void BT_HID::send_consumer_report(uint16_t usage_id) {
    if (!connected()) {
        return;
    }
    uint8_t report[3];
    report[0] = REPORT_ID_CONSUMER;
    report[1] = static_cast<uint8_t>(usage_id & 0xFF);
    report[2] = static_cast<uint8_t>((usage_id >> 8) & 0xFF);
    hid_device_send_interrupt_message(m_hid_cid, report, sizeof(report));
}

/****************************/
/*     process_text_queue   */
/****************************/
void BT_HID::process_text_queue() {
    if (time_us_32() < m_next_action_us) {
        return;
    }

    if (m_key_pressed) {
        send_key_release();
        m_key_pressed = false;
        m_next_action_us = time_us_32() + CHAR_RELEASE_DELAY_US;
        return;
    }

    Text_Queue_Entry entry;
    if (queue_pop(entry)) {
        send_key_press(entry.modifiers, entry.keycode);
        m_key_pressed = true;
        m_next_action_us = time_us_32() + CHAR_PRESS_DELAY_US;
    }
}

/****************************/
/*        queue_push        */
/****************************/
void BT_HID::queue_push(uint8_t keycode, uint8_t modifiers) {
    std::size_t next_tail = (m_queue_tail + 1) % TEXT_QUEUE_SIZE;
    if (next_tail == m_queue_head) {
        return;  // queue full, drop
    }
    m_text_queue[m_queue_tail].keycode   = keycode;
    m_text_queue[m_queue_tail].modifiers = modifiers;
    m_queue_tail = next_tail;
}

/****************************/
/*         queue_pop        */
/****************************/
bool BT_HID::queue_pop(Text_Queue_Entry& entry) {
    if (m_queue_head == m_queue_tail) {
        return false;
    }
    entry = m_text_queue[m_queue_head];
    m_queue_head = (m_queue_head + 1) % TEXT_QUEUE_SIZE;
    return true;
}

/****************************/
/*  char_to_hid_keycode     */
/****************************/
uint8_t BT_HID::char_to_hid_keycode(char c, uint8_t& modifiers) {
    modifiers = 0;
    if (c >= 'a' && c <= 'z') { return static_cast<uint8_t>(0x04 + (c - 'a')); }
    if (c >= 'A' && c <= 'Z') { modifiers = 0x02; return static_cast<uint8_t>(0x04 + (c - 'A')); }
    if (c >= '1' && c <= '9') { return static_cast<uint8_t>(0x1E + (c - '1')); }
    if (c == '0') { return 0x27; }
    switch (c) {
        case ' ':  return 0x2C;
        case '\n': return 0x28;
        case '-':  return 0x2D;
        case '=':  return 0x2E;
        case '[':  return 0x2F;
        case ']':  return 0x30;
        case '\\': return 0x31;
        case ';':  return 0x33;
        case '\'': return 0x34;
        case '`':  return 0x35;
        case ',':  return 0x36;
        case '.':  return 0x37;
        case '/':  return 0x38;
        case '!': modifiers = 0x02; return 0x1E;
        case '@': modifiers = 0x02; return 0x1F;
        case '#': modifiers = 0x02; return 0x20;
        case '$': modifiers = 0x02; return 0x21;
        case '%': modifiers = 0x02; return 0x22;
        case '^': modifiers = 0x02; return 0x23;
        case '&': modifiers = 0x02; return 0x24;
        case '*': modifiers = 0x02; return 0x25;
        case '(': modifiers = 0x02; return 0x26;
        case ')': modifiers = 0x02; return 0x27;
        case '_': modifiers = 0x02; return 0x2D;
        case '+': modifiers = 0x02; return 0x2E;
        case '{': modifiers = 0x02; return 0x2F;
        case '}': modifiers = 0x02; return 0x30;
        case '|': modifiers = 0x02; return 0x31;
        case ':': modifiers = 0x02; return 0x33;
        case '"': modifiers = 0x02; return 0x34;
        case '~': modifiers = 0x02; return 0x35;
        case '<': modifiers = 0x02; return 0x36;
        case '>': modifiers = 0x02; return 0x37;
        case '?': modifiers = 0x02; return 0x38;
        default:  return 0;
    }
}

// ── BTstack packet handler ─────────────────────────────────────────────────────

/****************************/
/*   packet_handler_cb      */
/****************************/
void BT_HID::packet_handler_cb(uint8_t type, uint16_t channel,
                                uint8_t* packet, uint16_t size) {
    if (s_instance) {
        s_instance->on_packet(type, channel, packet, size);
    }
}

/****************************/
/*        on_packet         */
/****************************/
void BT_HID::on_packet(uint8_t type, uint16_t channel,
                        uint8_t* packet, [[maybe_unused]] uint16_t size) {
    if (type == HCI_EVENT_PACKET) {
        switch (hci_event_packet_get_type(packet)) {

            case BTSTACK_EVENT_STATE:
                if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                    bd_addr_t local_addr;
                    gap_local_bd_addr(local_addr);
                    printf("[BT_HID] BT ready. Addr: %s\n",
                           bd_addr_to_str(local_addr));
                }
                break;

            case HCI_EVENT_PIN_CODE_REQUEST:
                printf("[BT_HID] PIN request from host — responding with '%s'\n",
                       BT_FIXED_PIN);
                {
                    bd_addr_t event_addr;
                    hci_event_pin_code_request_get_bd_addr(packet, event_addr);
                    hci_send_cmd(&hci_pin_code_request_reply, event_addr,
                                 strlen(BT_FIXED_PIN), BT_FIXED_PIN);
                }
                break;

            case HCI_EVENT_USER_CONFIRMATION_REQUEST: {
                // Accept numeric comparison without user interaction
                bd_addr_t confirm_addr;
                hci_event_user_confirmation_request_get_bd_addr(packet, confirm_addr);
                gap_ssp_confirmation_response(confirm_addr);
                break;
            }

            case HCI_EVENT_HID_META:
                switch (hci_event_hid_meta_get_subevent_code(packet)) {
                    case HID_SUBEVENT_CONNECTION_OPENED:
                        if (hid_subevent_connection_opened_get_status(packet) == ERROR_CODE_SUCCESS) {
                            m_hid_cid = hid_subevent_connection_opened_get_hid_cid(packet);
                            printf("[BT_HID] Connected, cid=0x%04X\n", m_hid_cid);
                        } else {
                            printf("[BT_HID] Connection failed: 0x%02X\n",
                                   hid_subevent_connection_opened_get_status(packet));
                        }
                        break;

                    case HID_SUBEVENT_CONNECTION_CLOSED:
                        printf("[BT_HID] Disconnected\n");
                        m_hid_cid = 0;
                        break;

                    case HID_SUBEVENT_CAN_SEND_NOW:
                        // Nothing queued at interrupt level; text queue drains via tick()
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}
