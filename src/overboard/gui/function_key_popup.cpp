/**
 * @file    function_key_popup.cpp
 * @author  Marvin Smith
 * @date    2026-06-03
 *
 * @brief   Function key popup implementation
 */

// Project Libraries
#include <overboard/gui/function_key_popup.hpp>

namespace ovb::gui {

/***************************/
/*  Function Key Popup     */
/***************************/
Function_Key_Popup::Function_Key_Popup(int func_index)
    : m_func_index(func_index), m_visible(false)
{
}

/***************************/
/*         Show            */
/***************************/
void Function_Key_Popup::show()
{
    m_visible = true;
}

/***************************/
/*         Hide            */
/***************************/
void Function_Key_Popup::hide()
{
    m_visible = false;
}

/***************************/
/*      Handle Input       */
/***************************/
bool Function_Key_Popup::handle_input(core::Input_Key key)
{
    // For now, dismiss popup on any non-F-key press
    // TODO: Implement proper popup input handling
    (void)key;
    return false;
}

/***************************/
/*        Render           */
/***************************/
void Function_Key_Popup::render()
{
    // TODO: Implement popup rendering
    // For now, popup is blank as requested
}

} // namespace ovb::gui
