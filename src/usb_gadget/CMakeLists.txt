cmake_minimum_required(VERSION 3.13)

# Pico SDK must be included before project()
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable not set. "
                        "Please set it to the path of the Raspberry Pi Pico SDK.")
endif()

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2)

# Enable UART stdio for debugging
set(PICO_STDIO_UART ON)
set(PICO_STDIO_USB OFF)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb_hid_bridge
        LANGUAGES C CXX ASM
        VERSION 0.1.0
        DESCRIPTION "Pico 2 USB HID Keyboard Gadget for Calculator")

# Initialize Pico SDK
pico_sdk_init()

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Executable
add_executable(usb_hid_bridge
    main.cpp
    uart_handler.cpp
    usb_hid.cpp
    usb_descriptors.c
    mode_controller.cpp
)

# Include path for shared protocol header
target_include_directories(usb_hid_bridge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# TinyUSB configuration
target_compile_definitions(usb_hid_bridge PRIVATE
    # Enable TinyUSB HID device support
    CFG_TUD_HID=1
    CFG_TUD_CDC=0
    CFG_TUD_MSC=0
    CFG_TUD_MIDI=0
    CFG_TUD_VENDOR=0

    # USB device configuration
    USB_VID=0x2E8A
    USB_PID=0x1234
    USB_MANUFACTURER="Overboard"
    USB_PRODUCT="Calculator HID Bridge"

    # UART configuration
    UART_ID=uart0
    UART_TX_PIN=0
    UART_RX_PIN=1
    UART_BAUD_RATE=115200
)

# Link against Pico SDK libraries
target_link_libraries(usb_hid_bridge
    pico_stdlib
    pico_unique_id
    hardware_uart
    hardware_gpio
    hardware_timer
    tinyusb_device
    tinyusb_board
)

# Enable USB output, disable UART output for main binary
pico_enable_stdio_uart(usb_hid_bridge 1)
pico_enable_stdio_usb(usb_hid_bridge 0)

# Create map/bin/hex/uf2 file
pico_add_extra_outputs(usb_hid_bridge)

# Print memory usage
add_custom_command(TARGET usb_hid_bridge POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} $<TARGET_FILE:usb_hid_bridge>
    COMMENT "Memory usage:"
)
