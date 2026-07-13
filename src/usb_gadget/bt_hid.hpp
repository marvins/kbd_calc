/**
 * @file    bt_hid.hpp
 * @author  Overboard Team
 * @date    2026-07-12
 *
 * @brief   Bluetooth HID Keyboard Handler
 *
 * Presents the same keyboard + consumer-control interface as USB_HID but
 * over a Classic Bluetooth (BR/EDR) HID device connection using BTstack.
 *
 * Pairing uses a fixed PIN ("0000").
 *
 * TODO: Extend the UART ICD (docs/pico_usb_gadget.md) with a
 *       BT_PIN_CHANGE message (type 0x06) so the Pi Zero can push a
 *       user-configured PIN at runtime instead of the fixed default.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <cstddef>

// NOTE: btstack.h is intentionally NOT included here.
// TinyUSB (tusb.h) and BTstack (btstack.h) both define hid_report_type_t and
// HID_REPORT_TYPE_* — including both in the same translation unit causes a
// compile error. btstack.h is included only in bt_hid.cpp.

class BT_HID {
    public:
        /**
         * @brief Construct a new BT HID object
         */
        BT_HID();

        /**
         * @brief Initialize BTstack and register the HID device profile.
         *        Must be called once before the main loop.
         */
        void init();

        /**
         * @brief Drive BTstack event loop — call every main-loop iteration.
         */
        void tick();

        /**
         * @brief Returns true when a host is connected and the HID channel is open.
         */
        bool connected() const;

        /**
         * @brief Send a key press report
         * @param modifiers  USB HID modifier byte
         * @param keycode    USB HID keycode
         */
        void send_key_press(uint8_t modifiers, uint8_t keycode);

        /**
         * @brief Send a key release report (all keys up)
         */
        void send_key_release();

        /**
         * @brief Send a consumer control report
         * @param usage_id  USB HID Consumer Page usage ID (0 = release)
         */
        void send_consumer_control(uint16_t usage_id);

        /**
         * @brief Queue text for character-by-character typing
         * @param text  Null-terminated ASCII string
         * @return      Number of characters successfully queued
         */
        std::size_t queue_text(const char* text);

        /**
         * @brief Returns true when the text queue is empty
         */
        bool text_queue_empty() const;

    private:
        // ── HID report IDs (must match desc_hid_report in usb_descriptors.c) ──
        static constexpr uint8_t REPORT_ID_KEYBOARD { 1 };
        static constexpr uint8_t REPORT_ID_CONSUMER { 2 };

        // ── Pairing ───────────────────────────────────────────────────────────
        // TODO (ICD 0x06): replace with runtime-configurable PIN from Pi Zero
        static constexpr const char* BT_FIXED_PIN { "0000" };

        // ── Text queue ────────────────────────────────────────────────────────
        static constexpr std::size_t TEXT_QUEUE_SIZE       { 128 };
        static constexpr uint32_t    CHAR_PRESS_DELAY_US   { 10'000 };
        static constexpr uint32_t    CHAR_RELEASE_DELAY_US { 10'000 };

        struct Text_Queue_Entry {
            uint8_t keycode;
            uint8_t modifiers;
        };

        Text_Queue_Entry m_text_queue[TEXT_QUEUE_SIZE];
        std::size_t      m_queue_head      { 0 };
        std::size_t      m_queue_tail      { 0 };
        bool             m_key_pressed     { false };
        uint32_t         m_next_action_us  { 0 };

        // ── BTstack state ─────────────────────────────────────────────────────
        uint16_t         m_hid_cid         { 0 };   // 0 = not connected
        bool             m_bt_ready        { false };

        // BTstack packet handler (static trampoline → instance method)
        static BT_HID*   s_instance;
        static void      packet_handler_cb(uint8_t type, uint16_t channel,
                                           uint8_t* packet, uint16_t size);
        void             on_packet(uint8_t type, uint16_t channel,
                                   uint8_t* packet, uint16_t size);

        // ── Helpers ───────────────────────────────────────────────────────────
        void        send_keyboard_report(uint8_t modifiers, uint8_t keycode);
        void        send_consumer_report(uint16_t usage_id);
        void        queue_push(uint8_t keycode, uint8_t modifiers);
        bool        queue_pop(Text_Queue_Entry& entry);
        uint8_t     char_to_hid_keycode(char c, uint8_t& modifiers);
        void        process_text_queue();
};
