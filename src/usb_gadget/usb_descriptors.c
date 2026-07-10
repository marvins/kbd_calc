/**
 * @file    usb_descriptors.c
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   USB HID descriptors for composite keyboard + consumer control device
 *
 * TinyUSB callback implementations for device, configuration, and HID
 * report descriptors. Presents two interfaces:
 *   - Interface 0: Boot keyboard (Report ID 1)
 *   - Interface 1: Consumer control (Report ID 2)
 */

#include "tusb.h"
#include "pico/unique_id.h"

// USB descriptor IDs
#define USB_VID           0x2E8A
#define USB_PID           0x1234
#define USB_MANUFACTURER  "Overboard"
#define USB_PRODUCT       "Calculator HID Bridge"

// HID report IDs
#define REPORT_ID_KEYBOARD  1
#define REPORT_ID_CONSUMER  2

// String descriptor indices
enum {
    STRID_LANGID    = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};

// --- Device descriptor ---
static tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,
    .iManufacturer      = STRID_MANUFACTURER,
    .iProduct           = STRID_PRODUCT,
    .iSerialNumber      = STRID_SERIAL,
    .bNumConfigurations = 0x01,
};

// --- HID report descriptor: keyboard + consumer control ---
uint8_t const desc_hid_report[] = {
    // --- Keyboard (Report ID 1) ---
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP        ),
    HID_USAGE       ( HID_USAGE_DESKTOP_KEYBOARD    ),
    HID_COLLECTION  ( HID_COLLECTION_APPLICATION    ),
        HID_REPORT_ID    ( REPORT_ID_KEYBOARD       ),

        // Modifier byte (8 bits, one per modifier key)
        HID_USAGE_PAGE    ( HID_USAGE_PAGE_KEYBOARD ),
        HID_USAGE_MIN     ( 0xE0                    ),
        HID_USAGE_MAX     ( 0xE7                    ),
        HID_LOGICAL_MIN   ( 0x00                    ),
        HID_LOGICAL_MAX   ( 0x01                    ),
        HID_REPORT_COUNT  ( 8                       ),
        HID_REPORT_SIZE   ( 1                       ),
        HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

        // Reserved byte
        HID_REPORT_COUNT  ( 1                       ),
        HID_REPORT_SIZE   ( 8                       ),
        HID_INPUT         ( HID_CONSTANT            ),

        // Keycodes (up to 6 simultaneous)
        HID_USAGE_PAGE    ( HID_USAGE_PAGE_KEYBOARD ),
        HID_USAGE_MIN     ( 0x00                    ),
        HID_USAGE_MAX     ( 0xFF                    ),
        HID_LOGICAL_MIN   ( 0x00                    ),
        HID_LOGICAL_MAX   ( 0xFF                    ),
        HID_REPORT_COUNT  ( 6                       ),
        HID_REPORT_SIZE   ( 8                       ),
        HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ),

        // LED output report (Num/Caps/Scroll Lock)
        HID_REPORT_COUNT  ( 5                       ),
        HID_REPORT_SIZE   ( 1                       ),
        HID_USAGE_PAGE    ( HID_USAGE_PAGE_LED      ),
        HID_USAGE_MIN     ( 0x01                    ),
        HID_USAGE_MAX     ( 0x05                    ),
        HID_OUTPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

        // LED padding
        HID_REPORT_COUNT  ( 1                       ),
        HID_REPORT_SIZE   ( 3                       ),
        HID_OUTPUT        ( HID_CONSTANT            ),
    HID_COLLECTION_END,

    // --- Consumer Control (Report ID 2) ---
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_CONSUMER       ),
    HID_USAGE       ( HID_USAGE_CONSUMER_CONTROL    ),
    HID_COLLECTION  ( HID_COLLECTION_APPLICATION    ),
        HID_REPORT_ID    ( REPORT_ID_CONSUMER       ),

        HID_LOGICAL_MIN   ( 0x00                    ),
        HID_LOGICAL_MAX   ( 0x0FFF                  ),
        HID_USAGE_PAGE    ( HID_USAGE_PAGE_CONSUMER ),
        HID_USAGE_MIN     ( 0x00                    ),
        HID_USAGE_MAX     ( 0x0FFF                  ),
        HID_REPORT_COUNT  ( 1                       ),
        HID_REPORT_SIZE   ( 16                      ),
        HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ),
    HID_COLLECTION_END,
};

// --- Configuration descriptor (with two interfaces) ---
#define CONFIG_TOTAL_LEN  ( \
    TUD_CONFIG_DESC_LEN + \
    TUD_HID_INOUT_DESC_LEN + \
    TUD_HID_INOUT_DESC_LEN \
)

#define EPNUM_HID_KBD   0x81  // EP1 IN, keyboard
#define EPNUM_HID_CON   0x82  // EP2 IN, consumer control

uint8_t const desc_configuration[] = {
    // Configuration descriptor
    TUD_CONFIG_DESCRIPTOR(
        1,                  // config number
        2,                  // interface count
        0,                  // string index
        CONFIG_TOTAL_LEN,
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
        100                 // max power (mA)
    ),

    // Interface 0: Keyboard HID
    TUD_HID_DESCRIPTOR(
        0,                          // interface number
        0,                          // string index
        HID_ITF_PROTOCOL_KEYBOARD,  // boot protocol
        sizeof(desc_hid_report),    // report descriptor length
        EPNUM_HID_KBD,              // EP IN
        EPNUM_HID_KBD | 0x80,       // EP OUT (same number, OUT direction)
        CFG_TUD_HID_EP_BUFSIZE      // EP size
    ),

    // Interface 1: Consumer Control HID
    TUD_HID_DESCRIPTOR(
        1,                          // interface number
        0,                          // string index
        HID_ITF_PROTOCOL_NONE,      // no boot protocol
        sizeof(desc_hid_report),    // report descriptor length
        EPNUM_HID_CON,              // EP IN
        0,                          // no EP OUT
        CFG_TUD_HID_EP_BUFSIZE      // EP size
    ),
};

// --- String descriptors ---
static char const *string_desc_arr[] = {
    [STRID_LANGID]       = (const char[]) { 0x09, 0x04 },  // English
    [STRID_MANUFACTURER] = USB_MANUFACTURER,
    [STRID_PRODUCT]      = USB_PRODUCT,
};

// Serial number from board unique ID
static char serial_str[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

// --- TinyUSB descriptor callbacks ---

uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

uint8_t const *tud_descriptor_hid_report_cb(uint8_t instance) {
    (void) instance;
    return desc_hid_report;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;

    // Build serial number from unique board ID on first request
    if (index == STRID_SERIAL) {
        pico_unique_board_id_t board_id;
        pico_get_unique_board_id(&board_id);
        for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++) {
            snprintf(serial_str + i * 2, 3, "%02X", board_id.id[i]);
        }

        // Return as UTF-16 string descriptor
        static uint16_t serial_desc[1 + 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES];
        serial_desc[0] = (TUSB_DESC_STRING << 8) | (2 + 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES);
        for (int i = 0; i < 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++) {
            serial_desc[1 + i] = serial_str[i];
        }
        return serial_desc;
    }

    if (index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) {
        return NULL;
    }

    // Convert ASCII string to UTF-16 string descriptor
    static uint16_t _desc_str[32];
    const char *str = string_desc_arr[index];

    // First byte is length in bytes, second is descriptor type
    uint8_t chr_count = (uint8_t) strlen(str);
    if (chr_count > 31) chr_count = 31;

    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 + chr_count * 2);

    for (uint8_t i = 0; i < chr_count; i++) {
        _desc_str[1 + i] = str[i];
    }

    return _desc_str;
}
