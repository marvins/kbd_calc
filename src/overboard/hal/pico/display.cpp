/**
 * @file    display.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Raspberry Pi Pico hardware display implementation
 */
#include <overboard/hal/pico/display.hpp>

namespace ovb::hal::pico {

/****************************/
/*          Refresh         */
/****************************/
void Pico_Display::refresh() {
    // TODO: Refresh LCD section via SPI
}

/******************************/
/*        Update Layer        */
/******************************/
void Pico_Display::update_layer() {
    // TODO: Re-render keyboard buttons via LVGL
}

/****************************/
/*          Render          */
/****************************/
void Pico_Display::render() {
    // TODO: Drive LVGL tick/timer handler
}

} // namespace ovb::hal::pico
