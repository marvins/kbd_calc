/**
 * @file    app_menu.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Application menu panel implementation
 */
#include <overboard/gui/app_menu.hpp>

// C++ Standard Libraries
#include <string_view>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/gui/panel_manager.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*          Constructor        */
/*******************************/
App_Menu::App_Menu( std::vector<std::shared_ptr<I_App>> a,
                    Panel_Manager& panels,
                    std::vector<Menu_Item> items,
                    Select_Cb cb,
                    int init)
    : apps(std::move(a)),
      menu_items(std::move(items)),
      on_select(std::move(cb)),
      initial_selection(init) {

    // Register all apps with Panel_Manager
    panel_indices.reserve(apps.size());
    for (auto& app : apps) {
        panel_indices.push_back(panels.register_panel(app));
    }
}

/*******************************/
/*          Destructor         */
/*******************************/
App_Menu::~App_Menu() = default;

/*******************************/
/*           Activate          */
/*******************************/
void App_Menu::activate(lv_obj_t* parent) {
    LOG_DEBUG("App_Menu: activating, menu_items.size=", std::to_string(menu_items.size()));
    selected = initial_selection;

    container = lv_obj_create(parent);
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_align(container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    // Use explicit dimensions - parent may not be laid out yet
    const int width = hal::LCD_WIDTH;
    const int height = hal::LCD_HEIGHT;
    LOG_DEBUG("App_Menu: using LCD size ", std::to_string(width), "x", std::to_string(height));

    // Header bar
    header = std::make_unique<Header_Bar>(container, width);
    header->set_app_name("Menu");

    // Menu list (centered between header and footer)
    const int list_h = height - Header_Bar::HEIGHT - Footer_Bar::HEIGHT;
    list = lv_list_create(container);
    lv_obj_set_size(list, width - 32, list_h - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, Header_Bar::HEIGHT + 16);
    lv_obj_set_style_bg_color(list, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(list, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(list, 8, LV_PART_MAIN);

    // Add menu items
    LOG_DEBUG("App_Menu: adding ", std::to_string(menu_items.size()), " menu items");
    for (size_t i = 0; i < menu_items.size(); ++i) {
        const auto& item = menu_items[i];
        LOG_DEBUG("App_Menu: adding item '", item.name, "' with icon '", item.icon, "'");
        lv_obj_t* btn = lv_list_add_btn(list, item.icon.c_str(), item.name.c_str());
        // Store menu index in user data for selection callback
        lv_obj_set_user_data(btn, reinterpret_cast<void*>(static_cast<intptr_t>(i)));
    }

    // Highlight selected item after buttons are created
    update_selection();

    // Footer bar — decorative
    footer = std::make_unique<Footer_Bar>(container, width);
    LOG_DEBUG("App_Menu: activate complete");
}

/*******************************/
/*          Deactivate         */
/*******************************/
void App_Menu::deactivate() {
    LOG_DEBUG("App_Menu: deactivating");
    footer.reset();
    header.reset();
    if (container) {
        lv_obj_del(container);
        container = nullptr;
    }
    list = nullptr;
    LOG_DEBUG("App_Menu: deactivate complete");
}

/*******************************/
/*        Handle Input         */
/*******************************/
bool App_Menu::handle_input(core::Action_Code action) {
    LOG_DEBUG("App_Menu::handle_input: action_code=", std::to_string(static_cast<int>(action)), " (", core::action_code_to_display(action), ")");
    switch (action) {
        case core::Action_Code::ESCAPE:
            // ESCAPE from menu pops back to previous panel
            return false; // Let Panel_Manager handle pop
        case core::Action_Code::CURSOR_DOWN:
            if (selected < static_cast<int>(menu_items.size()) - 1) {
                selected++;
                update_selection();
            }
            return true;
        case core::Action_Code::CURSOR_UP:
            if (selected > 0) {
                selected--;
                update_selection();
            }
            return true;
        case core::Action_Code::EVAL:
            select_current();
            return true;
        default:
            return false;
    }
}

/*******************************/
/*        Handle Text          */
/*******************************/
bool App_Menu::handle_text(char32_t codepoint) {

    // Look up hotkey in menu items (case-insensitive)
    const char key = static_cast<char>(codepoint);
    for (size_t i = 0; i < menu_items.size(); ++i) {

        const char hotkey = menu_items[i].hotkey;

        if (hotkey != '\0' && (key == hotkey || key == std::toupper(hotkey))) {
            selected = static_cast<int>(i);
            select_current();
            return true;
        }
    }
    return false;
}

/*******************************/
/*      Handle Input Key       */
/*******************************/
bool App_Menu::handle_input_key(core::Input_Key key) {

    switch (key) {
        case core::Input_Key::UP:
            if (selected > 0) {
                selected--;
                update_selection();
            }
            return true;
        case core::Input_Key::DOWN:
            if (selected < static_cast<int>(menu_items.size()) - 1) {
                selected++;
                update_selection();
            }
            return true;
        case core::Input_Key::RETURN:
        case core::Input_Key::NUMPAD_ENTER:
            // Enter/Return selects current menu item
            select_current();
            return true;
        default:
            return false;
    }
}

/*******************************/
/*           Refresh           */
/*******************************/
void App_Menu::refresh() {
    // Static menu — nothing to update
}

/*******************************/
/*            Name             */
/*******************************/
std::string App_Menu::name() const {
    return "Menu";
}

/*******************************/
/*       Update Selection      */
/*******************************/
void App_Menu::update_selection() {
    if (!list) {
        return;
    }

    // Iterate through list children to find buttons
    uint32_t child_count = lv_obj_get_child_cnt(list);
    int button_idx = 0;
    for (uint32_t i = 0; i < child_count && button_idx < static_cast<int>(menu_items.size()); ++i) {
        lv_obj_t* child = lv_obj_get_child(list, static_cast<int32_t>(i));
        if (child && lv_obj_has_class(child, &lv_button_class)) {
            if (button_idx == selected) {
                lv_obj_add_state(child, LV_STATE_FOCUSED);
                lv_obj_add_state(child, LV_STATE_PRESSED);
            } else {
                lv_obj_clear_state(child, LV_STATE_FOCUSED);
                lv_obj_clear_state(child, LV_STATE_PRESSED);
            }
            button_idx++;
        }
    }
}

/*******************************/
/*       Select Current        */
/*******************************/
void App_Menu::select_current() {
    if (!list || !on_select) {
        return;
    }

    if (selected >= 0 && selected < static_cast<int>(menu_items.size())) {
        const auto& selected_item = menu_items[static_cast<size_t>(selected)];
        const int panel_idx = (selected_item.index >= 0 && static_cast<size_t>(selected_item.index) < panel_indices.size())
                             ? panel_indices[static_cast<size_t>(selected_item.index)] : -1;
        LOG_DEBUG("App_Menu: selecting app '", std::string(selected_item.name), "' (panel_index=", std::to_string(panel_idx), ")");
        on_select(panel_idx);
    }
}

/*******************************/
/*      Get Custom Label       */
/*******************************/
std::string App_Menu::get_custom_label(int key_index) const {
    // In menu context, key 27 (TH33 Enter/Eval key) acts as "Select"
    if (key_index == 27) {
        return "Sel";
    }
    return ""; // Use default from keyboard.json
}

} // namespace ovb::gui
