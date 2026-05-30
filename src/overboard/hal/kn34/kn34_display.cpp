/**
 * @file    kn34_display.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   KN34 hardware display implementation
 */
#include <overboard/hal/kn34/kn34_display.hpp>

namespace ovb::hal::kn34 {

/****************************/
/*          Refresh         */
/****************************/
void KN34_Display::refresh() {
    // TODO: Refresh LCD section via SPI
}

/******************************/
/*        Update Layer        */
/******************************/
void KN34_Display::update_layer() {
    // TODO: Re-render keyboard buttons via LVGL
}

/****************************/
/*          Render          */
/****************************/
void KN34_Display::render() {
    // TODO: Drive LVGL tick/timer handler
}

} // namespace ovb::hal::kn34
