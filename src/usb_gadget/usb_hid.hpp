/**
 * @file    usb_hid.hpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   USB HID Keyboard Handler
 *
 * Manages USB HID keyboard reports using TinyUSB.
 * Provides non-blocking text typing via a character queue.
 */
#pragma once

// C++ Standard Libraries
#include <cstddef>
#include <cstdint>

// Third-Party Libraries
#include "tusb.h"

class USB_HID {
    public:

        /**
         * @brief Construct a new USB HID object
         */
        USB_HID();

        /**
         * @brief Initialize the USB HID device
         */
        void init();

        /**
         * @brief Send a key press report
         */
        void send_key_press(uint8_t modifiers, uint8_t keycode);

        /**
         * @brief Send a key release report
         */
        void send_key_release();

        /**
         * @brief Send a consumer control report
         */
        void send_consumer_control(uint16_t usage_id);

        /**
         * @brief Queue text for typing
         */
        std::size_t queue_text(const char* text);

        /**
         * @brief Process the text queue
         */
        void tick();

        /**
         * @brief Check if the text queue is empty
         */
        bool text_queue_empty() const;

    private:

        /**
         * @brief Text queue entry
         */
        struct Text_Queue_Entry {
            uint8_t keycode;
            uint8_t modifiers;
        };

        /// @brief Text queue size
        static constexpr std::size_t TEXT_QUEUE_SIZE      { 128 };

        /// @brief Character press delay in microseconds
        static constexpr uint32_t    CHAR_PRESS_DELAY_US  { 10'000 };

        /// @brief Character release delay in microseconds
        static constexpr uint32_t    CHAR_RELEASE_DELAY_US { 10'000 };

        /// @brief Report ID for keyboard reports
        static constexpr uint8_t     REPORT_ID_KEYBOARD   { 1 };

        /// @brief Report ID for consumer control reports
        static constexpr uint8_t     REPORT_ID_CONSUMER   { 2 };

        /// @brief Text queue
        Text_Queue_Entry m_text_queue[TEXT_QUEUE_SIZE];

        /// @brief Queue head
        std::size_t      m_queue_head;

        /// @brief Queue tail
        std::size_t      m_queue_tail;

        /// @brief Key pressed flag
        bool             m_key_pressed;

        /// @brief Next action time in microseconds
        uint32_t         m_next_action_time_us;

        /// @brief Convert a character to a HID keycode and modifiers
        uint8_t char_to_hid_keycode(char c, uint8_t& modifiers);

        /// @brief Push a keycode and modifiers to the text queue
        void    queue_push(uint8_t keycode, uint8_t modifiers);

        /// @brief Pop a keycode and modifiers from the text queue
        bool    queue_pop(Text_Queue_Entry& entry);
};
