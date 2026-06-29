/**
 * @file    display.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   PicoCalc ILI9488 SPI display driver implementation
 */
#include <overboard/hal/picocalc/display.hpp>

// C++ Standard Libraries
#include <cstring>

// Third-Party Libraries — Pico SDK (only compiled on TARGET_RP2350)
#ifdef TARGET_RP2350
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>
#endif

#include <lvgl.h>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::picocalc {

// ---------------------------------------------------------------------------
// SPI / GPIO pin assignments (ClockworkPi PicoCalc PCB V2.0)
// ---------------------------------------------------------------------------
static constexpr uint LCD_SCK  = 10;
static constexpr uint LCD_MOSI = 11;
static constexpr uint LCD_MISO = 12;
static constexpr uint LCD_CS   = 13;
static constexpr uint LCD_DC   = 14;
static constexpr uint LCD_RST  = 15;

static constexpr uint32_t LCD_SPI_SPEED = 25'000'000;

// ILI9488 command codes
static constexpr uint8_t ILI9488_NOP      = 0x00;
static constexpr uint8_t ILI9488_SWRESET  = 0x01;
static constexpr uint8_t ILI9488_SLPOUT   = 0x11;
static constexpr uint8_t ILI9488_INVOFF   = 0x20;
static constexpr uint8_t ILI9488_DISPON   = 0x29;
static constexpr uint8_t ILI9488_CASET    = 0x2A;
static constexpr uint8_t ILI9488_PASET    = 0x2B;
static constexpr uint8_t ILI9488_RAMWR    = 0x2C;
static constexpr uint8_t ILI9488_MADCTL   = 0x36;
static constexpr uint8_t ILI9488_PIXFMT   = 0x3A;

// Partial render buffer: 10 rows × 320 px × 3 bytes (RGB888 wire format for ILI9488 18-bit mode)
static constexpr int BYTES_PER_PIXEL = 3;
static constexpr int BUF_ROWS        = 10;
static uint8_t s_buf[LCD_PHYS_WIDTH * BUF_ROWS * 2];   // LVGL uses RGB565 (2 bytes) internally
static uint8_t s_out[LCD_PHYS_WIDTH * BUF_ROWS * BYTES_PER_PIXEL]; // expanded to 3-byte wire format

/****************************/
/*       SPI Init           */
/****************************/
void PicoCalc_Display::spi_init() {
    LOG_INFO("Initializing ILI9488 SPI display interface");

#ifdef TARGET_RP2350
    LOG_DEBUG("Configuring SPI1 at ", LCD_SPI_SPEED, " Hz");
    ::spi_init(spi1, LCD_SPI_SPEED);
    gpio_set_function(LCD_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);
    gpio_set_drive_strength(LCD_SCK,  GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LCD_MOSI, GPIO_DRIVE_STRENGTH_12MA);
    LOG_DEBUG("SPI pins configured - SCK:", LCD_SCK, " MOSI:", LCD_MOSI, " MISO:", LCD_MISO);

    LOG_DEBUG("Initializing GPIO pins for display control");
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_put(LCD_CS, 1);
    LOG_DEBUG("CS pin (", LCD_CS, ") initialized and set high");

    gpio_init(LCD_DC);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_put(LCD_DC, 1);
    LOG_DEBUG("DC pin (", LCD_DC, ") initialized and set high");

    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_put(LCD_RST, 1);
    LOG_DEBUG("RST pin (", LCD_RST, ") initialized and set high");

    LOG_DEBUG("Performing ILI9488 reset sequence");
    sleep_ms(10);
    gpio_put(LCD_RST, 0);
    LOG_DEBUG("Reset asserted (low)");
    sleep_ms(10);
    gpio_put(LCD_RST, 1);
    LOG_DEBUG("Reset de-asserted (high), waiting for display to initialize");
    sleep_ms(120);

    LOG_INFO("SPI initialization completed successfully");
#else
    LOG_WARN("SPI initialization skipped - not running on RP2350");
#endif
}

/****************************/
/*     LCD Send Command     */
/****************************/
void PicoCalc_Display::lcd_send_cmd(uint8_t cmd) {
#ifdef TARGET_RP2350
    LOG_TRACE("Sending ILI9488 command: 0x", std::hex, static_cast<int>(cmd));
    gpio_put(LCD_DC, 0);
    gpio_put(LCD_CS, 0);
    ::spi_write_blocking(spi1, &cmd, 1);
    gpio_put(LCD_CS, 1);
    gpio_put(LCD_DC, 1);
    LOG_TRACE("Command 0x", std::hex, static_cast<int>(cmd), " sent successfully");
#else
    LOG_TRACE("Skipping ILI9488 command 0x", std::hex, static_cast<int>(cmd), " - not on RP2350");
    (void)cmd;
#endif
}

/****************************/
/*      LCD Send Data       */
/****************************/
void PicoCalc_Display::lcd_send_data(const uint8_t* data, size_t len) {
#ifdef TARGET_RP2350
    LOG_TRACE("Sending ", len, " bytes of data to ILI9488");
    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);
    ::spi_write_blocking(spi1, data, len);
    gpio_put(LCD_CS, 1);
    LOG_TRACE("Data transmission completed");
#else
    LOG_TRACE("Skipping data transmission - not on RP2350");
    (void)data; (void)len;
#endif
}

/****************************/
/*      LCD Set Window      */
/****************************/
void PicoCalc_Display::lcd_set_window(uint16_t x1, uint16_t y1,
                                       uint16_t x2, uint16_t y2) {
    uint8_t col_data[4] = {
        static_cast<uint8_t>(x1 >> 8), static_cast<uint8_t>(x1),
        static_cast<uint8_t>(x2 >> 8), static_cast<uint8_t>(x2)
    };
    uint8_t row_data[4] = {
        static_cast<uint8_t>(y1 >> 8), static_cast<uint8_t>(y1),
        static_cast<uint8_t>(y2 >> 8), static_cast<uint8_t>(y2)
    };
    lcd_send_cmd(ILI9488_CASET);
    lcd_send_data(col_data, 4);
    lcd_send_cmd(ILI9488_PASET);
    lcd_send_data(row_data, 4);
    lcd_send_cmd(ILI9488_RAMWR);
}

/****************************/
/*        LCD Init          */
/****************************/
void PicoCalc_Display::lcd_init() {
    LOG_INFO("Initializing ILI9488 LCD controller");

    // Software reset + wake
    LOG_DEBUG("Sending software reset command (0x01)");
    lcd_send_cmd(ILI9488_SWRESET);
#ifdef TARGET_RP2350
    sleep_ms(120);
    LOG_DEBUG("Software reset delay completed");
#endif

    LOG_DEBUG("Sending sleep out command (0x11)");
    lcd_send_cmd(ILI9488_SLPOUT);
#ifdef TARGET_RP2350
    sleep_ms(120);
    LOG_DEBUG("Sleep out delay completed");
#endif

    // Pixel format: 18-bit (3 bytes per pixel on the wire) — ILI9488 requires 0x66 for SPI
    LOG_DEBUG("Setting pixel format to 18-bit (0x66)");
    uint8_t pixfmt = 0x66;
    lcd_send_cmd(ILI9488_PIXFMT);
    lcd_send_data(&pixfmt, 1);

    // Memory access control: MX + BGR for portrait orientation
    LOG_DEBUG("Setting memory access control (0x48)");
    uint8_t madctl = 0x48;
    lcd_send_cmd(ILI9488_MADCTL);
    lcd_send_data(&madctl, 1);

    LOG_DEBUG("Disabling inversion (0x20)");
    lcd_send_cmd(ILI9488_INVOFF);

    LOG_DEBUG("Turning display on (0x29)");
    lcd_send_cmd(ILI9488_DISPON);
#ifdef TARGET_RP2350
    sleep_ms(50);
    LOG_INFO("ILI9488 LCD initialization completed successfully");
#else
    LOG_WARN("LCD initialization completed on non-RP2350 platform");
#endif
}

/****************************/
/*        Flush CB          */
/****************************/
void PicoCalc_Display::flush_cb( lv_display_t* disp,
                                  const lv_area_t* area,
                                  uint8_t* px_map ) {
    lcd_set_window(static_cast<uint16_t>(area->x1),
                   static_cast<uint16_t>(area->y1),
                   static_cast<uint16_t>(area->x2),
                   static_cast<uint16_t>(area->y2));

    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    int32_t n_pixels = w * h;

    // LVGL renders RGB565 (2 bytes/pixel); ILI9488 in 18-bit mode needs 3 bytes/pixel.
    // Expand in-place into the wire buffer.
    const uint16_t* src = reinterpret_cast<const uint16_t*>(px_map);
    uint8_t* dst = s_out;
    for (int32_t i = 0; i < n_pixels; ++i) {
        uint16_t c = src[i];
        dst[0] = (c & 0xF800) >> 8;  // R: bits 15-11 → top byte
        dst[1] = (c & 0x07E0) >> 3;  // G: bits 10-5
        dst[2] = (c & 0x001F) << 3;  // B: bits 4-0
        dst += 3;
    }

    lcd_send_data(s_out, static_cast<size_t>(n_pixels * BYTES_PER_PIXEL));

    lv_display_flush_ready(disp);
}

/****************************/
/*       Constructor        */
/****************************/
PicoCalc_Display::PicoCalc_Display() {
    spi_init();
    lcd_init();

    m_lv_display = lv_display_create(LCD_PHYS_WIDTH, LCD_PHYS_HEIGHT);
    if (!m_lv_display) {
#ifdef TARGET_RP2350
        panic("PicoCalc_Display: lv_display_create failed");
#else
        return;
#endif
    }

    lv_display_set_flush_cb(m_lv_display, flush_cb);
    lv_display_set_buffers(m_lv_display,
                           s_buf, nullptr,
                           sizeof(s_buf),   // LVGL RGB565 buffer size
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_default(m_lv_display);
}

/****************************/
/*        Destructor        */
/****************************/
PicoCalc_Display::~PicoCalc_Display() {
    if (m_lv_display) {
        lv_display_delete(m_lv_display);
    }
}

/****************************/
/*          Screen          */
/****************************/
lv_obj_t* PicoCalc_Display::screen() const {
    return lv_screen_active();
}

} // namespace ovb::hal::picocalc
