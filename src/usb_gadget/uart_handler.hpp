/**
 * @file    uart_handler.hpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   UART Message Handler
 *
 * Handles UART reception, message framing, and protocol parsing.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>

// Third-Party Libraries
#include "hardware/uart.h"

// Project Libraries
#include <overboard/protocol/uart_protocol.hpp>

class UART_Handler {
    public:
        UART_Handler();

        void init();
        bool receive_message(ovb::protocol::Protocol_Message& msg);
        bool check_timeout();

    private:
        static constexpr uint16_t RX_BUFFER_SIZE     { 256 };
        static constexpr uint32_t MESSAGE_TIMEOUT_US { 100'000 };

        uint8_t  m_rx_buffer[RX_BUFFER_SIZE];
        uint8_t  m_rx_index;
        bool     m_receiving;
        uint8_t  m_expected_length;
        uint32_t m_last_byte_time_us;

        void reset_receiver();
        void process_byte(uint8_t byte);
};
