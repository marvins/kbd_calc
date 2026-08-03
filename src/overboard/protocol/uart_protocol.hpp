/**
 * @file    uart_protocol.hpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   UART Protocol Definitions (Pi Zero <-> Pico 2)
 *
 * Shared protocol definitions for communication between Pi Zero calculator
 * and Pico 2 USB HID gadget. Both projects include this header to ensure
 * protocol compatibility.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>

namespace ovb::protocol {

inline constexpr uint8_t  SOF               { 0xAA };
inline constexpr uint8_t  MAX_PAYLOAD_SIZE  { 251 };
inline constexpr uint8_t  MIN_MESSAGE_SIZE  { 5 };  // SOF + LEN + TYPE + CRC

enum class Message_Type : uint8_t {
    Mode_Change      = 0x01,
    Key_Event        = 0x02,
    Macro_Sequence   = 0x03,
    Text_String      = 0x04,
    Consumer_Control = 0x05,
    Ping             = 0xF0,
    Reset            = 0xFF
};

enum class Key_Action : uint8_t {
    Release = 0,
    Press   = 1
};

enum class Operating_Mode : uint8_t {
    Inactive    = 0,
    Passthrough = 1,
    Macro       = 2
};

enum class Modifier_Bits : uint8_t {
    Left_Ctrl   = (1 << 0),
    Left_Shift  = (1 << 1),
    Left_Alt    = (1 << 2),
    Left_Gui    = (1 << 3),
    Right_Ctrl  = (1 << 4),
    Right_Shift = (1 << 5),
    Right_Alt   = (1 << 6),
    Right_Gui   = (1 << 7)
};

struct Protocol_Message {
    uint8_t type;
    uint8_t payload_length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
};

inline uint8_t calculate_crc8(const uint8_t* data, uint8_t length) {
    uint8_t crc { 0x00 };

    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

inline bool validate_message(const uint8_t* buffer, uint8_t length) {
    if (length < MIN_MESSAGE_SIZE) {
        return false;
    }

    if (buffer[0] != SOF) {
        return false;
    }

    uint8_t declared_length = buffer[1];
    if (declared_length != length) {
        return false;
    }

    uint8_t calculated_crc = calculate_crc8(buffer + 1, length - 2);
    uint8_t received_crc   = buffer[length - 1];

    return calculated_crc == received_crc;
}

} // namespace ovb::protocol
