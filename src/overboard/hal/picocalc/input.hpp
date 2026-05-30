/**
 * @file    input.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   PicoCalc I2C keyboard input driver
 *
 * Polls the STM32-based keyboard controller over I2C1 (SDA=6, SCL=7,
 * addr=0x1F). Raw key codes (ASCII + extended) are mapped to logical
 * key indices via the Grid_Layout configuration. Queues Key_Events
 * for consumption by PicoCalc_App.
 *
 * I2C register 0x09 returns a 2-byte packet:
 *   byte[0] = key code, byte[1] = event type (0x01 = press)
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <queue>
#include <unordered_map>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_input.hpp>

namespace ovb::hal::picocalc {

/// I2C peripheral constants (fixed PCB layout)
inline constexpr uint8_t  I2C_KBD_ADDR  { 0x1F };
inline constexpr uint8_t  I2C_KBD_REG   { 0x09 };
inline constexpr int      I2C_KBD_SDA   { 6 };
inline constexpr int      I2C_KBD_SCL   { 7 };
inline constexpr uint32_t I2C_KBD_SPEED { 400'000 };

/**
 * @brief PicoCalc I2C keyboard input driver
 *
 * Polls the keyboard controller each pump() call. Raw ASCII/extended
 * key codes are mapped to key indices using a configurable scancode map
 * that is loaded from the VIA layout JSON at startup.
 */
class PicoCalc_Input : public I_Input {

    public:

        explicit PicoCalc_Input( const core::Grid_Layout& layout );

        /**
         * @brief Bind a raw I2C key code to a key index
         *
         * @param i2c_code  Raw byte returned by the keyboard controller
         * @param key_index Logical key index in Grid_Layout
         */
        void bind( uint8_t i2c_code, int key_index );

        bool poll( Key_Event& out_event ) override;
        bool should_quit() const override;
        void pump() override;

    private:

        void i2c_init_hw();
        int  read_raw_key() const;
        int  translate( uint8_t raw ) const;

        const core::Grid_Layout&          m_layout;
        std::unordered_map<uint8_t, int>  m_keymap;
        std::queue<Key_Event>             m_queue;
        bool                              m_initialized = false;
        bool                              m_quit        = false;
};

} // namespace ovb::hal::picocalc
