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
#include <stdexcept>

// Third-Party Libraries — Pico SDK (only compiled on TARGET_RP2350)
#ifdef TARGET_RP2350
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>
#endif

#include <lvgl.h>

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

// Partial render buffer: 10 rows × 320 px × 2 bytes (RGB565)
static constexpr int BYTES_PER_PIXEL = 2;
static constexpr int BUF_ROWS        = 10;
static uint8_t s_buf[LCD_PHYS_WIDTH * BUF_ROWS * BYTES_PER_PIXEL];

/****************************/
/*       SPI Init           */
/****************************/
void PicoCalc_Display::spi_init() {
#ifdef TARGET_RP2350
    ::spi_init(spi1, LCD_SPI_SPEED);
    gpio_set_function(LCD_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);

    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_put(LCD_CS, 1);

    gpio_init(LCD_DC);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_put(LCD_DC, 1);

    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_put(LCD_RST, 1);
    sleep_ms(10);
    gpio_put(LCD_RST, 0);
    sleep_ms(10);
    gpio_put(LCD_RST, 1);
    sleep_ms(120);
#endif
}

/****************************/
/*     LCD Send Command     */
/****************************/
void PicoCalc_Display::lcd_send_cmd(uint8_t cmd) {
#ifdef TARGET_RP2350
    gpio_put(LCD_DC, 0);
    gpio_put(LCD_CS, 0);
    ::spi_write_blocking(spi1, &cmd, 1);
    gpio_put(LCD_CS, 1);
    gpio_put(LCD_DC, 1);
#else
    (void)cmd;
#endif
}

/****************************/
/*      LCD Send Data       */
/****************************/
void PicoCalc_Display::lcd_send_data(const uint8_t* data, size_t len) {
#ifdef TARGET_RP2350
    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);
    ::spi_write_blocking(spi1, data, len);
    gpio_put(LCD_CS, 1);
#else
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
    // Software reset + wake
    lcd_send_cmd(ILI9488_SWRESET);
#ifdef TARGET_RP2350
    sleep_ms(120);
#endif
    lcd_send_cmd(ILI9488_SLPOUT);
#ifdef TARGET_RP2350
    sleep_ms(120);
#endif

    // Pixel format: 16-bit RGB565
    uint8_t pixfmt = 0x55;
    lcd_send_cmd(ILI9488_PIXFMT);
    lcd_send_data(&pixfmt, 1);

    // Memory access control: MX + BGR for portrait orientation
    uint8_t madctl = 0x48;
    lcd_send_cmd(ILI9488_MADCTL);
    lcd_send_data(&madctl, 1);

    lcd_send_cmd(ILI9488_INVOFF);
    lcd_send_cmd(ILI9488_DISPON);
#ifdef TARGET_RP2350
    sleep_ms(50);
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
    lcd_send_data(px_map, static_cast<size_t>(w * h * BYTES_PER_PIXEL));

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
        throw std::runtime_error("PicoCalc_Display: lv_display_create failed");
    }

    lv_display_set_flush_cb(m_lv_display, flush_cb);
    lv_display_set_buffers(m_lv_display,
                           s_buf, nullptr,
                           sizeof(s_buf),
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
