/**
 * @file    usb_hid.cpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   USB HID Keyboard Handler Implementation
 */

#include "usb_hid.hpp"

// Third-Party Libraries
#include "hardware/timer.h"
#include "tusb.h"

// C++ Standard Libraries
#include <cctype>
#include <cstdio>
#include <cstring>

namespace {

// HID keycodes (USB HID Usage Tables)
constexpr uint8_t HID_KEY_A          { 0x04 };
constexpr uint8_t HID_KEY_B          { 0x05 };
constexpr uint8_t HID_KEY_C          { 0x06 };
constexpr uint8_t HID_KEY_D          { 0x07 };
constexpr uint8_t HID_KEY_E          { 0x08 };
constexpr uint8_t HID_KEY_F          { 0x09 };
constexpr uint8_t HID_KEY_G          { 0x0A };
constexpr uint8_t HID_KEY_H          { 0x0B };
constexpr uint8_t HID_KEY_I          { 0x0C };
constexpr uint8_t HID_KEY_J          { 0x0D };
constexpr uint8_t HID_KEY_K          { 0x0E };
constexpr uint8_t HID_KEY_L          { 0x0F };
constexpr uint8_t HID_KEY_M          { 0x10 };
constexpr uint8_t HID_KEY_N          { 0x11 };
constexpr uint8_t HID_KEY_O          { 0x12 };
constexpr uint8_t HID_KEY_P          { 0x13 };
constexpr uint8_t HID_KEY_Q          { 0x14 };
constexpr uint8_t HID_KEY_R          { 0x15 };
constexpr uint8_t HID_KEY_S          { 0x16 };
constexpr uint8_t HID_KEY_T          { 0x17 };
constexpr uint8_t HID_KEY_U          { 0x18 };
constexpr uint8_t HID_KEY_V          { 0x19 };
constexpr uint8_t HID_KEY_W          { 0x1A };
constexpr uint8_t HID_KEY_X          { 0x1B };
constexpr uint8_t HID_KEY_Y          { 0x1C };
constexpr uint8_t HID_KEY_Z          { 0x1D };
constexpr uint8_t HID_KEY_1          { 0x1E };
constexpr uint8_t HID_KEY_2          { 0x1F };
constexpr uint8_t HID_KEY_3          { 0x20 };
constexpr uint8_t HID_KEY_4          { 0x21 };
constexpr uint8_t HID_KEY_5          { 0x22 };
constexpr uint8_t HID_KEY_6          { 0x23 };
constexpr uint8_t HID_KEY_7          { 0x24 };
constexpr uint8_t HID_KEY_8          { 0x25 };
constexpr uint8_t HID_KEY_9          { 0x26 };
constexpr uint8_t HID_KEY_0          { 0x27 };
constexpr uint8_t HID_KEY_ENTER      { 0x28 };
constexpr uint8_t HID_KEY_SPACE      { 0x2C };
constexpr uint8_t HID_KEY_MINUS      { 0x2D };
constexpr uint8_t HID_KEY_EQUAL      { 0x2E };
constexpr uint8_t HID_KEY_LEFTBRACE  { 0x2F };
constexpr uint8_t HID_KEY_RIGHTBRACE { 0x30 };
constexpr uint8_t HID_KEY_BACKSLASH  { 0x31 };
constexpr uint8_t HID_KEY_SEMICOLON  { 0x33 };
constexpr uint8_t HID_KEY_APOSTROPHE { 0x34 };
constexpr uint8_t HID_KEY_GRAVE      { 0x35 };
constexpr uint8_t HID_KEY_COMMA      { 0x36 };
constexpr uint8_t HID_KEY_DOT        { 0x37 };
constexpr uint8_t HID_KEY_SLASH      { 0x38 };

} // anonymous namespace

/***************************/
/*       Constructor       */
/***************************/
USB_HID::USB_HID()
    : m_queue_head(0)
    , m_queue_tail(0)
    , m_key_pressed(false)
    , m_next_action_time_us(0)
{
}

/***************************/
/*         Init            */
/***************************/
void USB_HID::init() {
    // TinyUSB initialization handled by pico_stdlib
    tusb_init();
}

/***************************/
/*     Send Key Press      */
/***************************/
void USB_HID::send_key_press(uint8_t modifiers, uint8_t keycode) {
    // Wait for USB to be ready
    if (!tud_hid_ready()) {
        return;
    }

    // Build keyboard report
    uint8_t keycodes[6] = {keycode, 0, 0, 0, 0, 0};

    // Send report
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycodes);
}

/***************************/
/*    Send Key Release     */
/***************************/
void USB_HID::send_key_release() {
    // Wait for USB to be ready
    if (!tud_hid_ready()) {
        return;
    }

    // Send empty report (all keys released)
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, nullptr);
}

/***************************/
/*       Queue Text        */
/***************************/
std::size_t USB_HID::queue_text(const char* text) {
    std::size_t queued = 0;
    for (std::size_t i = 0; text[i] != '\0' && queued < TEXT_QUEUE_SIZE; i++) {
        uint8_t modifiers = 0;
        uint8_t keycode = char_to_hid_keycode(text[i], modifiers);
        if (keycode != 0) {
            queue_push(keycode, modifiers);
            queued++;
        }
    }
    return queued;
}

/***************************/
/*          Tick           */
/***************************/
void USB_HID::tick() {
    // Don't process if waiting for delay
    if (time_us_32() < m_next_action_time_us) {
        return;
    }

    if (m_key_pressed) {
        // Release the current key
        send_key_release();
        m_key_pressed = false;
        m_next_action_time_us = time_us_32() + CHAR_RELEASE_DELAY_US;
        return;
    }

    // Try to send next character from queue
    Text_Queue_Entry entry;
    if (queue_pop(entry)) {
        send_key_press(entry.modifiers, entry.keycode);
        m_key_pressed = true;
        m_next_action_time_us = time_us_32() + CHAR_PRESS_DELAY_US;
    }
}

/***************************/
/*   Text Queue Empty      */
/***************************/
bool USB_HID::text_queue_empty() const {
    return m_queue_head == m_queue_tail;
}

/***************************/
/*       Queue Push        */
/***************************/
void USB_HID::queue_push(uint8_t keycode, uint8_t modifiers) {
    std::size_t next_tail = (m_queue_tail + 1) % TEXT_QUEUE_SIZE;
    if (next_tail == m_queue_head) {
        // Queue full -- drop character
        return;
    }
    m_text_queue[m_queue_tail].keycode   = keycode;
    m_text_queue[m_queue_tail].modifiers = modifiers;
    m_queue_tail = next_tail;
}

/***************************/
/*        Queue Pop        */
/***************************/
bool USB_HID::queue_pop(Text_Queue_Entry& entry) {
    if (m_queue_head == m_queue_tail) {
        return false;  // Empty
    }
    entry = m_text_queue[m_queue_head];
    m_queue_head = (m_queue_head + 1) % TEXT_QUEUE_SIZE;
    return true;
}

/***************************/
/* Send Consumer Control   */
/***************************/
void USB_HID::send_consumer_control(uint16_t usage_id) {
    // Wait for USB to be ready
    if (!tud_hid_ready()) {
        return;
    }

    // Send consumer control report
    tud_hid_report(REPORT_ID_CONSUMER, &usage_id, sizeof(usage_id));
}

/***************************/
/*  Char to HID Keycode    */
/***************************/
uint8_t USB_HID::char_to_hid_keycode(char c, uint8_t& modifiers) {
    modifiers = 0;

    // Lowercase letters
    if (c >= 'a' && c <= 'z') {
        return HID_KEY_A + (c - 'a');
    }

    // Uppercase letters
    if (c >= 'A' && c <= 'Z') {
        modifiers = 0x02;  // Left Shift
        return HID_KEY_A + (c - 'A');
    }

    // Numbers
    if (c >= '1' && c <= '9') {
        return HID_KEY_1 + (c - '1');
    }
    if (c == '0') {
        return HID_KEY_0;
    }

    // Special characters
    switch (c) {
        case ' ':  return HID_KEY_SPACE;
        case '\n': return HID_KEY_ENTER;
        case '-':  return HID_KEY_MINUS;
        case '=':  return HID_KEY_EQUAL;
        case '[':  return HID_KEY_LEFTBRACE;
        case ']':  return HID_KEY_RIGHTBRACE;
        case '\\': return HID_KEY_BACKSLASH;
        case ';':  return HID_KEY_SEMICOLON;
        case '\'': return HID_KEY_APOSTROPHE;
        case '`':  return HID_KEY_GRAVE;
        case ',':  return HID_KEY_COMMA;
        case '.':  return HID_KEY_DOT;
        case '/':  return HID_KEY_SLASH;

        case '!': modifiers = 0x02; return HID_KEY_1;
        case '@': modifiers = 0x02; return HID_KEY_2;
        case '#': modifiers = 0x02; return HID_KEY_3;
        case '$': modifiers = 0x02; return HID_KEY_4;
        case '%': modifiers = 0x02; return HID_KEY_5;
        case '^': modifiers = 0x02; return HID_KEY_6;
        case '&': modifiers = 0x02; return HID_KEY_7;
        case '*': modifiers = 0x02; return HID_KEY_8;
        case '(': modifiers = 0x02; return HID_KEY_9;
        case ')': modifiers = 0x02; return HID_KEY_0;
        case '_': modifiers = 0x02; return HID_KEY_MINUS;
        case '+': modifiers = 0x02; return HID_KEY_EQUAL;
        case '{': modifiers = 0x02; return HID_KEY_LEFTBRACE;
        case '}': modifiers = 0x02; return HID_KEY_RIGHTBRACE;
        case '|': modifiers = 0x02; return HID_KEY_BACKSLASH;
        case ':': modifiers = 0x02; return HID_KEY_SEMICOLON;
        case '"': modifiers = 0x02; return HID_KEY_APOSTROPHE;
        case '~': modifiers = 0x02; return HID_KEY_GRAVE;
        case '<': modifiers = 0x02; return HID_KEY_COMMA;
        case '>': modifiers = 0x02; return HID_KEY_DOT;
        case '?': modifiers = 0x02; return HID_KEY_SLASH;

        default: return 0;
    }
}
