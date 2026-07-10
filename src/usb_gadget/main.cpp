/**
 * @file    main.cpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   Pico 2 USB HID Bridge - Main Entry Point
 *
 * This firmware runs on a Raspberry Pi Pico 2 (RP2350) and acts as a USB HID
 * keyboard gadget. It receives commands from a Raspberry Pi Zero via UART and
 * translates them into USB HID keyboard events sent to a connected laptop.
 *
 * Operating Modes:
 * - Inactive: No USB activity (default on boot)
 * - Passthrough: Forward key events directly to laptop
 * - Macro: Execute macro sequences with timing
 */

// Third-Party Libraries
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "hardware/uart.h"
#include "tusb.h"

// Project Libraries
#include <overboard/protocol/uart_protocol.hpp>
#include "mode_controller.hpp"
#include "uart_handler.hpp"
#include "usb_hid.hpp"

// C++ Standard Libraries
#include <cstdio>
#include <cstring>

using namespace ovb::protocol;

/****************************/
/*       Global State       */
/****************************/

static UART_Handler    uart_handler;
static USB_HID         usb_hid;
static Mode_Controller mode_controller;

/****************************/
/*    Init Hardware         */
/****************************/
void init_hardware() {
    // Initialize stdio for debugging (UART1, not UART0 which is used for protocol)
    stdio_init_all();

    printf("\n========================================\n");
    printf("USB HID Bridge v0.1.0\n");
    printf("Raspberry Pi Pico 2 (RP2350)\n");

    // Print unique ID
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);
    printf("Board ID: ");
    for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++) {
        printf("%02X", board_id.id[i]);
    }
    printf("\n========================================\n\n");
}

/****************************/
/*   Process Message        */
/****************************/
void process_message(const Protocol_Message& msg) {
    switch (static_cast<Message_Type>(msg.type)) {
        case Message_Type::Mode_Change: {
            if (msg.payload_length >= 1) {
                auto new_mode = static_cast<Operating_Mode>(msg.payload[0]);
                mode_controller.set_mode(new_mode);
                printf("Mode changed to: %d\n", static_cast<int>(new_mode));
            }
            break;
        }

        case Message_Type::Key_Event: {
            if (msg.payload_length >= 3) {
                uint8_t modifiers = msg.payload[0];
                uint8_t keycode   = msg.payload[1];
                uint8_t action    = msg.payload[2];

                if (mode_controller.current_mode() == Operating_Mode::Passthrough) {
                    if (action == static_cast<uint8_t>(Key_Action::Press)) {
                        usb_hid.send_key_press(modifiers, keycode);
                    } else {
                        usb_hid.send_key_release();
                    }
                }
            }
            break;
        }

        case Message_Type::Macro_Sequence: {
            if (mode_controller.current_mode() == Operating_Mode::Macro) {
                if (msg.payload_length >= 1) {
                    uint8_t count = msg.payload[0];
                    uint8_t* action_data = const_cast<uint8_t*>(msg.payload + 1);

                    // Execute macro actions
                    for (uint8_t i = 0; i < count && (action_data - msg.payload) < msg.payload_length; i++) {
                        uint8_t mod   = action_data[0];
                        uint8_t key   = action_data[1];
                        uint8_t act   = action_data[2];
                        uint8_t delay = action_data[3];

                        if (act == static_cast<uint8_t>(Key_Action::Press)) {
                            usb_hid.send_key_press(mod, key);
                        } else {
                            usb_hid.send_key_release();
                        }

                        if (delay > 0) {
                            sleep_ms(delay);
                        }

                        action_data += 4;
                    }

                    // Final release
                    usb_hid.send_key_release();
                }
            }
            break;
        }

        case Message_Type::Text_String: {
            if (mode_controller.current_mode() == Operating_Mode::Macro) {
                if (msg.payload_length >= 1) {
                    const char* text = reinterpret_cast<const char*>(msg.payload + 1);
                    usb_hid.queue_text(text);
                }
            }
            break;
        }

        case Message_Type::Consumer_Control: {
            if (msg.payload_length >= 3) {
                uint16_t usage_id = (msg.payload[0] << 8) | msg.payload[1];
                uint8_t  action   = msg.payload[2];

                if (action == static_cast<uint8_t>(Key_Action::Press)) {
                    usb_hid.send_consumer_control(usage_id);
                } else {
                    usb_hid.send_consumer_control(0);
                }
            }
            break;
        }

        case Message_Type::Ping: {
            printf("Ping received\n");
            break;
        }

        case Message_Type::Reset: {
            mode_controller.set_mode(Operating_Mode::Inactive);
            usb_hid.send_key_release();
            printf("Reset to inactive mode\n");
            break;
        }

        default:
            printf("Unknown message type: 0x%02X\n", msg.type);
            break;
    }
}

/****************************/
/*       Main Loop          */
/****************************/
int main() {
    // Initialize hardware
    init_hardware();

    // Initialize UART handler
    uart_handler.init();
    printf("UART initialized (GPIO %d/%d @ %d baud)\n",
           UART_RX_PIN, UART_TX_PIN, UART_BAUD_RATE);

    // Initialize USB HID
    usb_hid.init();
    printf("USB HID initialized\n");

    // Initialize mode controller
    mode_controller.init();
    printf("Mode controller initialized\n");

    printf("\nReady. Waiting for commands...\n\n");

    // Main loop
    while (true) {
        // Process USB events
        tud_task();
        usb_hid.tick();

        // Check for incoming UART messages
        Protocol_Message msg;
        if (uart_handler.receive_message(msg)) {
            // Process the message
            process_message(msg);
        }

        // Small yield to prevent busy-wait
        tight_loop_contents();
    }

    return 0;
}
