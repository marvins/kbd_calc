/**
 * @file    input.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   PicoCalc I2C keyboard input driver implementation
 */
#include <overboard/hal/picocalc/input.hpp>

// Third-Party Libraries — Pico SDK (only compiled on TARGET_RP2350)
#ifdef TARGET_RP2350
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/stdlib.h>
#endif

namespace ovb::hal::picocalc {

/****************************/
/*       Constructor        */
/****************************/
PicoCalc_Input::PicoCalc_Input( const core::Grid_Layout& layout )
    : m_layout(layout)
{
    i2c_init_hw();

    // Default scancode map: raw ASCII key codes → key indices.
    // These cover the digit / operator keys the calculator primarily uses.
    // Additional bindings can be added via bind() or loaded from VIA JSON.
    const struct { uint8_t code; int idx; } defaults[] = {
        // Digits
        { '0',  0 }, { '1',  1 }, { '2',  2 }, { '3',  3 },
        { '4',  4 }, { '5',  5 }, { '6',  6 }, { '7',  7 },
        { '8',  8 }, { '9',  9 },
        // Operators
        { '+', 10 }, { '-', 11 }, { '*', 12 }, { '/', 13 },
        { '=', 14 },
        // Editing
        { '\b', 15 },  // Backspace
        { 0xB1, 16 },  // ESC (All Clear) — extended code
        { '.',  17 },
        // Navigation / layer
        { 0xb5, 18 },  // Arrow Up
        { 0xb6, 19 },  // Arrow Down
        { 0xb4, 20 },  // Arrow Left
        { 0xb7, 21 },  // Arrow Right
    };
    for (const auto& d : defaults) {
        m_keymap[d.code] = d.idx;
    }
}

/****************************/
/*         I2C Init         */
/****************************/
void PicoCalc_Input::i2c_init_hw() {
#ifdef TARGET_RP2350
    ::i2c_init(i2c1, I2C_KBD_SPEED);
    gpio_set_function(static_cast<uint>(I2C_KBD_SDA), GPIO_FUNC_I2C);
    gpio_set_function(static_cast<uint>(I2C_KBD_SCL), GPIO_FUNC_I2C);
    gpio_pull_up(static_cast<uint>(I2C_KBD_SDA));
    gpio_pull_up(static_cast<uint>(I2C_KBD_SCL));
    m_initialized = true;
#else
    m_initialized = false;  // No-op on non-embedded builds
#endif
}

/****************************/
/*          Bind            */
/****************************/
void PicoCalc_Input::bind(uint8_t i2c_code, int key_index) {
    m_keymap[i2c_code] = key_index;
}

/****************************/
/*       Read Raw Key       */
/****************************/
int PicoCalc_Input::read_raw_key() const {
#ifdef TARGET_RP2350
    if (!m_initialized) return -1;

    // Write register address 0x09, then read 2-byte response
    const uint8_t reg = I2C_KBD_REG;
    int ret = ::i2c_write_timeout_us(i2c1, I2C_KBD_ADDR, &reg, 1, false, 500'000);
    if (ret < 0) return -1;

    uint8_t buf[2] = {0, 0};
    ret = ::i2c_read_timeout_us(i2c1, I2C_KBD_ADDR, buf, 2, false, 500'000);
    if (ret < 0) return -1;

    // Packet format: buf[1]=keycode, buf[0]=event (0x01 = pressed)
    if (buf[0] == 0x01 && buf[1] != 0) {
        return static_cast<int>(buf[1]);
    }
    return 0;
#else
    return 0;
#endif
}

/****************************/
/*        Translate         */
/****************************/
int PicoCalc_Input::translate(uint8_t raw) const {
    auto it = m_keymap.find(raw);
    if (it != m_keymap.end()) {
        return it->second;
    }
    return -1;  // Unknown key
}

/****************************/
/*           Pump           */
/****************************/
void PicoCalc_Input::pump() {
    int raw = read_raw_key();
    if (raw <= 0) return;

    // Check for quit: ESC (0xB1) with no key index mapped to it acts as quit
    if (raw == 0x1B || raw == 0xB1) {
        // Only quit if ESC is not mapped to a calculator key
        auto it = m_keymap.find(static_cast<uint8_t>(raw));
        if (it == m_keymap.end()) {
            m_quit = true;
            return;
        }
    }

    int key_index = translate(static_cast<uint8_t>(raw));
    if (key_index >= 0) {
        m_queue.push(Key_Event{ Key_Event_Kind::Action, key_index, 0, Key_Event_Type::Press });
        return;
    }

    // Unmapped printable ASCII — emit as a Text event so panels can handle it.
    // Raw bytes from the I2C controller are already Shift-resolved (the STM32
    // firmware handles modifier state), so '+', '&', uppercase letters, etc.
    // arrive as their final character values.
    if (raw >= 0x20 && raw < 0x7F) {
        m_queue.push(Key_Event{ Key_Event_Kind::Text, -1, static_cast<char32_t>(raw), Key_Event_Type::Press });
    }
}

/****************************/
/*           Poll           */
/****************************/
bool PicoCalc_Input::poll(Key_Event& out_event) {
    if (m_queue.empty()) return false;
    out_event = m_queue.front();
    m_queue.pop();
    return true;
}

/****************************/
/*       Should Quit        */
/****************************/
bool PicoCalc_Input::should_quit() const {
    return m_quit;
}

} // namespace ovb::hal::picocalc
