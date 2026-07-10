/**
 * @file    uart_handler.cpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   UART Message Handler Implementation
 */

#include "uart_handler.hpp"

// Third-Party Libraries
#include "hardware/gpio.h"
#include "hardware/timer.h"

// C++ Standard Libraries
#include <cstdio>
#include <cstring>

using namespace ovb::protocol;

/***************************/
/*       Constructor       */
/***************************/
UART_Handler::UART_Handler()
    : m_rx_index(0)
    , m_receiving(false)
    , m_expected_length(0)
    , m_last_byte_time_us(0)
{
    memset(m_rx_buffer, 0, sizeof(m_rx_buffer));
}

/***************************/
/*          Init           */
/***************************/
void UART_Handler::init() {
    // Initialize UART0 at specified baud rate
    uart_init(uart0, UART_BAUD_RATE);

    // Set TX and RX pins
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Set UART format: 8 data bits, 1 stop bit, no parity
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    // Enable UART FIFO
    uart_set_fifo_enabled(uart0, true);

    reset_receiver();
}

/***************************/
/*     Reset Receiver      */
/***************************/
void UART_Handler::reset_receiver() {
    m_rx_index = 0;
    m_receiving = false;
    m_expected_length = 0;
    m_last_byte_time_us = 0;
}

/***************************/
/*    Receive Message      */
/***************************/
bool UART_Handler::receive_message(Protocol_Message& msg) {
    // Check for timed-out partial messages first
    check_timeout();

    // Check if data is available
    while (uart_is_readable(uart0)) {
        uint8_t byte = uart_getc(uart0);
        m_last_byte_time_us = time_us_32();
        process_byte(byte);

        // Check if we have a complete message
        if (m_receiving && m_rx_index == m_expected_length) {
            // Validate message
            if (validate_message(m_rx_buffer, m_rx_index)) {
                // Extract message type and payload
                msg.type = m_rx_buffer[2];
                msg.payload_length = m_expected_length - 5;  // Subtract overhead
                memcpy(msg.payload, m_rx_buffer + 3, msg.payload_length);

                reset_receiver();
                return true;
            } else {
                // CRC failed, discard message
                printf("CRC validation failed\n");
                reset_receiver();
            }
        }
    }

    return false;
}

/******************************************************************************/
/*                        Check Timeout                                       */
/******************************************************************************/
bool UART_Handler::check_timeout() {
    if (!m_receiving) return false;

    uint32_t elapsed = time_us_32() - m_last_byte_time_us;
    if (elapsed > MESSAGE_TIMEOUT_US) {
        printf("Message timeout: %u bytes received, %u expected\n",
               m_rx_index, m_expected_length);
        reset_receiver();
        return true;
    }

    return false;
}

/***************************/
/*      Process Byte       */
/***************************/
void UART_Handler::process_byte(uint8_t byte) {
    // Look for SOF
    if (!m_receiving) {
        if (byte == SOF) {
            m_rx_buffer[0] = byte;
            m_rx_index = 1;
            m_receiving = true;
            m_expected_length = 0;
        }
        return;
    }

    // Store byte
    if (m_rx_index < RX_BUFFER_SIZE) {
        m_rx_buffer[m_rx_index++] = byte;

        // After receiving LEN byte, validate and set expected length
        if (m_rx_index == 2) {
            m_expected_length = byte;

            // Sanity check
            if (m_expected_length < MIN_MESSAGE_SIZE || m_expected_length > RX_BUFFER_SIZE) {
                printf("Invalid message length: %d\n", m_expected_length);
                reset_receiver();
            }
        }
    } else {
        // Buffer overflow
        printf("RX buffer overflow\n");
        reset_receiver();
    }
}
