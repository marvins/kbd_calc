/**
 * @file    lv_font_custom.h
 * @author  Marvin Smith
 * @date    2026-08-30
 *
 * @brief   Project custom font declarations for LVGL
 *
 * Included by LVGL via LV_FONT_CUSTOM_INCLUDE when
 * LV_FONT_USE_CUSTOM_INCLUDE is enabled in lv_conf.h.
 */
#pragma once

#define LV_FONT_CUSTOM_DECLARE \
    LV_FONT_DECLARE(lv_font_superscript_regular) \
    LV_FONT_DECLARE(lv_font_superscript_bold)
