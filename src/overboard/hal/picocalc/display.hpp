/**
 * @file    display.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   PicoCalc ILI9488 SPI display driver
 *
 * Creates the LVGL display handle backed by the ILI9488 320x320 SPI LCD
 * on the ClockworkPi PicoCalc. Exposes screen() for GUI layer attachment.
 * All SPI communication uses the Pico SDK hardware/spi API on SPI1.
 *
 * Pin assignments (fixed PCB layout):
 *   SCK=10, MOSI=11, MISO=12, CS=13, DC=14, RST=15
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::hal::picocalc {

/// Physical LCD resolution
inline constexpr int LCD_PHYS_WIDTH  = 320;
inline constexpr int LCD_PHYS_HEIGHT = 320;

/**
 * @brief PicoCalc ILI9488 display driver
 *
 * Owns the LVGL display handle and SPI peripheral. Initialises
 * the ILI9488 controller, registers the LVGL flush callback, and
 * sets up a partial-render DMA-friendly buffer (10 rows).
 */
class PicoCalc_Display {

    public:

        /**
         * @brief Constructor
         */
        PicoCalc_Display();

        /**
         * @brief Destructor
         */
        virtual ~PicoCalc_Display();

        /**
         * @brief LVGL active screen — attach GUI widgets here
         */
        lv_obj_t* screen() const;

    private:

        /**
         * @brief LVGL flush callback — copy dirty area to LCD
         */
        static void flush_cb( lv_display_t* disp,
                              const lv_area_t* area,
                              uint8_t* px_map );

        /**
         * @brief Initialise SPI1 and GPIO pins
         */
        static void spi_init();

        /**
         * @brief Initialise ILI9488 controller
         */
        static void lcd_init();

        /**
         * @brief Send command to LCD
         */
        static void lcd_send_cmd( uint8_t cmd );

        /**
         * @brief Send data to LCD
         */
        static void lcd_send_data( const uint8_t* data, size_t len );

        /**
         * @brief Set LCD window area
         */
        static void lcd_set_window( uint16_t x1, uint16_t y1,
                                    uint16_t x2, uint16_t y2 );

        lv_display_t* m_lv_display = nullptr;
};

} // namespace ovb::hal::picocalc
