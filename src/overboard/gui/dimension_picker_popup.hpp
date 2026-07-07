/**
 * @file    dimension_picker_popup.hpp
 * @author  Marvin Smith
 * @date    2026-07-05
 *
 * @brief   Two-axis dimension spinner popup for matrix/vector construction
 *
 * Presents two numeric spinners (rows and columns) navigated with arrow keys.
 * Confirming with Enter fires a callback with the chosen dimensions.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/gui/i_popup.hpp>

namespace ovb::gui {

/**
 * @brief Two-axis dimension spinner popup
 *
 * Displays two rows of spinners — Rows (1–9) and Cols (1–9).
 * Up/Down navigates between the two fields; Left/Right changes the value.
 * Digit keys 1–9 directly set the focused field.
 * Enter confirms; Escape cancels.
 */
class Dimension_Picker_Popup : public I_Popup {

    public:

        /**
         * @brief Callback fired on confirmation
         * @param rows Selected row count
         * @param cols Selected column count
         */
        using Confirm_Cb = std::function<void(int rows, int cols)>;

        /**
         * @brief Construct a dimension picker popup
         * @param parent   Parent LVGL object
         * @param title    Popup title (e.g. "New Matrix")
         * @param on_confirm Callback invoked when user confirms
         * @param show_cols  If false, only the Rows spinner is shown (vector mode)
         */
        Dimension_Picker_Popup( lv_obj_t*    parent,
                                std::string  title,
                                Confirm_Cb   on_confirm,
                                bool         show_cols = true );

        /**
         * @brief Destructor
         */
        ~Dimension_Picker_Popup() override;

        /**
         * @brief Show the popup
         */
        void show() override;

        /**
         * @brief Hide and destroy the popup
         */
        void hide() override;

        /**
         * @brief Handle input key
         */
        bool handle_input(core::Input_Key key) override;

        /**
         * @brief Handle action code (unused — returns false)
         */
        bool handle_input(core::Action_Code action) override;

        /**
         * @brief Redraw the popup contents
         */
        void render() override;

        /**
         * @brief Returns true if the popup is currently visible
         */
        bool is_visible() const;

    private:

        /// @brief Parent LVGL object
        lv_obj_t*   m_parent;

        /// @brief Container object
        lv_obj_t*   m_container  = nullptr;

        /// @brief Rows label
        lv_obj_t*   m_rows_label = nullptr;

        /// @brief Columns label
        lv_obj_t*   m_cols_label = nullptr;

        /// @brief Hint label
        lv_obj_t*   m_hint_label = nullptr;

        /// @brief Popup title
        std::string m_title;

        /// @brief Confirm callback
        Confirm_Cb  m_on_confirm;

        /// @brief Show columns spinner
        bool        m_show_cols;

        /// @brief Rows value
        int         m_rows    = 2;

        /// @brief Columns value
        int         m_cols    = 2;

        /// @brief Focused field (0 = rows, 1 = cols)
        int         m_focused = 0;

        /// @brief Visibility flag
        bool        m_visible = false;
};

} // namespace ovb::gui
