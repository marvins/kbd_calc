# System Diagrams

This document contains UML diagrams for the kbd_calc system architecture using Mermaid.

## High-Level Architecture

```mermaid
graph TB
    subgraph "Main Application"
        main[main.cpp]
    end

    subgraph "HAL - SDL Simulator"
        kbd[LVGL_Keyboard]
        lcd[LVGL_LCD_Display]
        input[SDL_Input]
        keymap[SDL_Keymap]
    end

    subgraph "Core Logic"
        layers[Layer_Manager]
        layout[Grid_Layout]
        keymap_core[Keymap]
    end

    subgraph "Math Engine"
        engine[Calc_Engine]
        parser[Parser]
        layout_engine[Layout_Engine]
    end

    main --> kbd
    main --> lcd
    main --> input
    main --> layers
    main --> engine

    kbd --> layout
    kbd --> layers
    kbd --> input

    lcd --> engine
    lcd --> layers
    lcd --> layout_engine

    input --> keymap
    input --> layout

    layers --> keymap_core
    engine --> parser
    engine --> layout_engine

    style main fill:#e1f5ff
    style kbd fill:#fff4e1
    style lcd fill:#fff4e1
    style input fill:#fff4e1
    style keymap fill:#fff4e1
    style layers fill:#e1ffe1
    style layout fill:#e1ffe1
    style keymap_core fill:#e1ffe1
    style engine fill:#ffe1f5
    style parser fill:#ffe1f5
    style layout_engine fill:#ffe1f5
```

## HAL Module Breakout

```mermaid
graph TB
    subgraph "HAL - SDL Simulator"
        subgraph "Keyboard Display"
            kbd_mgr[LVGL_Keyboard]
            kbd_disp[LVGL_Keyboard_Display]
            kbd_view[LVGL_Keyboard_View]
        end

        subgraph "LCD Display"
            lcd_disp[LVGL_LCD_Display]
            lcd_impl[LVGL_LCD_Display::Impl]
            lcd_canvas[lv_canvas]
            lcd_table[lv_table]
        end

        subgraph "Input System"
            sdl_input[SDL_Input]
            sdl_keymap[SDL_Keymap]
        end

        subgraph "Theme System"
            theme[lvgl_theme.hpp]
        end
    end

    kbd_mgr --> kbd_disp
    kbd_mgr --> kbd_view
    kbd_view --> theme
    kbd_disp --> theme

    lcd_disp --> lcd_impl
    lcd_impl --> lcd_canvas
    lcd_impl --> lcd_table
    lcd_disp --> theme

    sdl_input --> sdl_keymap
    sdl_input --> kbd_mgr

    style kbd_mgr fill:#fff4e1
    style kbd_disp fill:#fff4e1
    style kbd_view fill:#fff4e1
    style lcd_disp fill:#fff4e1
    style lcd_impl fill:#ffe1f5
    style lcd_canvas fill:#e1f5ff
    style lcd_table fill:#e1f5ff
    style sdl_input fill:#fff4e1
    style sdl_keymap fill:#fff4e1
    style theme fill:#e1ffe1
```

## GUI Module Breakout

```mermaid
graph TB
    subgraph "GUI Layer"
        subgraph "App Management"
            app_view[App_View]
            panel_mgr[Panel_Manager]
            app_registry[App_Registry]
            app_reg[app_registration.hpp]
        end

        subgraph "Panels"
            app_menu[App_Menu]
            calc_app[Calculator_App]
            status_page[Status_Page]
            settings_page[Settings_Page]
            keymap_info[Key_Mapping_Info]
        end

        subgraph "Display Components"
            lcd_section[LCD_Section]
            kbd_display[Keyboard_Display]
            header_bar[Header_Bar]
            footer_bar[Footer_Bar]
        end
    end

    app_view --> panel_mgr
    app_view --> app_registry
    app_view --> lcd_section
    app_view --> kbd_display

    app_registry --> app_reg
    app_registry --> app_menu

    app_menu --> panel_mgr
    app_menu --> calc_app
    app_menu --> status_page
    app_menu --> settings_page

    panel_mgr --> calc_app
    panel_mgr --> status_page
    panel_mgr --> settings_page
    panel_mgr --> app_menu
    panel_mgr --> keymap_info

    lcd_section --> header_bar
    lcd_section --> footer_bar

    style app_view fill:#e1f5ff
    style panel_mgr fill:#ffe1f5
    style app_registry fill:#fff4e1
    style app_reg fill:#e1ffe1
    style app_menu fill:#ffe1f5
    style calc_app fill:#e1ffe1
    style status_page fill:#e1ffe1
    style settings_page fill:#e1ffe1
    style keymap_info fill:#e1ffe1
    style lcd_section fill:#fff4e1
    style kbd_display fill:#fff4e1
    style header_bar fill:#ffe1f5
    style footer_bar fill:#ffe1f5
```

## SDL Simulator Class Diagram

```mermaid
classDiagram
    class LVGL_Keyboard {
        -LVGL_Keyboard_Display m_display
        -LVGL_Keyboard_View m_view
        -Grid_Layout m_layout
        -Layer_Manager m_layers
        +LVGL_Keyboard(title, width, height, layout, layers)
        +uint32_t window_id()
        +void set_pressed(int key)
        +void clear_pressed()
        +void update_layer()
        +void render()
    }

    class LVGL_Keyboard_Display {
        -lv_display_t* m_display
        -SDL_Window* m_sdl_window
        -int m_width
        -int m_height
        +LVGL_Keyboard_Display(title, width, height)
        +~LVGL_Keyboard_Display()
        +int width()
        +int height()
        +uint32_t window_id()
        +lv_obj_t* screen()
        +void clear(Color)
        +void flush()
    }

    class LVGL_Keyboard_View {
        -lv_obj_t* m_container
        -lv_obj_t* m_header
        -vector~lv_obj_t*~ m_buttons
        -vector~lv_obj_t*~ m_labels
        -Grid_Layout m_layout
        -Layer_Manager m_layers
        -int m_width
        -int m_height
        -int m_pressed_key
        +LVGL_Keyboard_View(parent, layout, layers, width, height)
        +void update_layer()
        +void set_pressed(int key)
        +void clear_pressed()
        -void build_buttons(parent)
        -void apply_style(key, pressed)
    }

    class LVGL_LCD_Display {
        -Impl* m_impl
        +LVGL_LCD_Display(title, x, y, width, height, engine, layers)
        +~LVGL_LCD_Display()
        +int width()
        +int height()
        +uint32_t window_id()
        +void refresh()
        +void render()
    }

    class SDL_Input {
        -uint32_t m_kbd_window_id
        -Grid_Layout m_layout
        -queue~Key_Event~ m_event_queue
        -SDL_Keymap m_keymap
        -bool m_quit
        +SDL_Input(window_id, width, height, layout, header, margin_left, margin_top)
        +bool poll(Key_Event)
        +bool should_quit()
        +void pump()
        +int hit_test(x, y)
        +SDL_Keymap keymap()
    }

    class SDL_Keymap {
        -array~int, SDL_NUM_SCANCODES~ m_scancode_map
        +SDL_Keymap()
        +optional~int~ get_key_index(scancode)
        +optional~int~ get_key_index_from_keycode(keycode)
        +void bind(scancode, key_index)
        +void unbind(scancode)
        +void reset_to_defaults()
    }

    class Layer_Manager {
        -Keymap m_keymap
        -int m_current_layer
        +Layer_Manager(keymap)
        +int current_layer_index()
        +Layer current_layer()
        +void set_layer(index)
        +void next_layer()
        +void prev_layer()
    }

    class Calc_Engine {
        -Expression m_expression
        -vector~History_Entry~ m_history
        -bool m_math_layout
        +Calc_Engine()
        +void handle_key(Key_Code)
        +void toggle_math_layout()
        +const Expression& expression()
        +const vector~History_Entry~& history()
    }

    LVGL_Keyboard *-- LVGL_Keyboard_Display
    LVGL_Keyboard *-- LVGL_Keyboard_View
    LVGL_Keyboard_View --> Layer_Manager
    LVGL_Keyboard_View --> Grid_Layout
    LVGL_LCD_Display --> Calc_Engine
    LVGL_LCD_Display --> Layer_Manager
    SDL_Input *-- SDL_Keymap
    SDL_Input --> Grid_Layout
    Layer_Manager --> Keymap
    Calc_Engine --> Expression
```

## Input Handling Sequence Diagram

```mermaid
sequenceDiagram
    participant User
    participant SDL_Input
    participant SDL_Keymap
    participant LVGL_Keyboard
    participant Layer_Manager
    participant Calc_Engine
    participant LVGL_LCD_Display

    User->>SDL_Input: Press physical key (e.g., 'q')
    SDL_Input->>SDL_Keymap: get_key_index(SDL_SCANCODE_Q)
    SDL_Keymap-->>SDL_Input: Optional(0) [key index]
    SDL_Input->>SDL_Input: Push Key_Event{0, Press}

    User->>SDL_Input: pump()
    SDL_Input-->>LVGL_Keyboard: Key_Event{0, Press}
    LVGL_Keyboard->>LVGL_Keyboard: set_pressed(0)
    LVGL_Keyboard->>LVGL_Keyboard_View: set_pressed(0)
    LVGL_Keyboard_View->>LVGL_Keyboard_View: apply_style(0, true)
    LVGL_Keyboard->>LVGL_Keyboard: render()

    User->>SDL_Input: Release physical key
    SDL_Input->>SDL_Keymap: get_key_index(SDL_SCANCODE_Q)
    SDL_Keymap-->>SDL_Input: Optional(0)
    SDL_Input->>SDL_Input: Push Key_Event{0, Release}

    User->>SDL_Input: pump()
    SDL_Input-->>LVGL_Keyboard: Key_Event{0, Release}
    LVGL_Keyboard->>LVGL_Keyboard: clear_pressed()
    LVGL_Keyboard->>LVGL_Keyboard_View: clear_pressed()
    LVGL_Keyboard->>LVGL_Keyboard: render()

    Note over LVGL_Keyboard,Calc_Engine: In main loop, LVGL_Keyboard passes events to Calc_Engine
    LVGL_Keyboard->>Calc_Engine: handle_key(Key_Code::DIGIT_1)
    Calc_Engine->>Calc_Engine: Update expression
    Calc_Engine->>LVGL_LCD_Display: refresh()
    LVGL_LCD_Display->>LVGL_LCD_Display: Update history table
    LVGL_LCD_Display->>LVGL_LCD_Display: Draw math preview
    LVGL_LCD_Display->>LVGL_LCD_Display: render()
```

## LVGL Display Architecture

```mermaid
graph LR
    subgraph "Keyboard Window"
        kbd_win[SDL Window]
        kbd_lv[LVGL Display Driver]
        kbd_screen[LVGL Screen]
        kbd_container[Container]
        kbd_header[Header Label]
        kbd_buttons[Button Grid]
    end

    subgraph "LCD Window"
        lcd_win[SDL Window]
        lcd_lv[LVGL Display Driver]
        lcd_screen[LVGL Screen]
        lcd_bezel[Bezel Container]
        lcd_table[History Table]
        lcd_preview[Preview Area]
        lcd_canvas[Math Canvas]
        lcd_labels[Mode/Layer Labels]
    end

    kbd_win --> kbd_lv
    kbd_lv --> kbd_screen
    kbd_screen --> kbd_container
    kbd_container --> kbd_header
    kbd_container --> kbd_buttons

    lcd_win --> lcd_lv
    lcd_lv --> lcd_screen
    lcd_screen --> lcd_bezel
    lcd_bezel --> lcd_table
    lcd_bezel --> lcd_preview
    lcd_bezel --> lcd_labels
    lcd_preview --> lcd_canvas

    style kbd_win fill:#e1f5ff
    style lcd_win fill:#ffe1f5
    style kbd_lv fill:#fff4e1
    style lcd_lv fill:#fff4e1
```

## Math Typesetting Flow

```mermaid
graph TB
    expr[Expression String]
    parser[Parser]
    ast[AST]
    layout_engine[Layout_Engine]
    box[Layout Box Tree]
    canvas[LVGL Canvas]
    layer[LVGL Layer]
    render[Render to Canvas]

    expr --> parser
    parser --> ast
    ast --> layout_engine
    layout_engine --> box
    box --> canvas
    canvas --> layer
    layer --> render

    style expr fill:#e1f5ff
    style parser fill:#fff4e1
    style ast fill:#e1ffe1
    style layout_engine fill:#fff4e1
    style box fill:#e1ffe1
    style canvas fill:#ffe1f5
    style layer fill:#fff4e1
    style render fill:#e1ffe1
```

## Component Dependencies

```mermaid
graph TB
    subgraph "HAL (hal/sdl)"
        lvgl_kbd[LVGL_Keyboard]
        lvgl_lcd[LVGL_LCD_Display]
        sdl_input[SDL_Input]
        sdl_keymap[SDL_Keymap]
    end

    subgraph "Core (core)"
        layer_mgr[Layer_Manager]
        grid_layout[Grid_Layout]
        keymap_core[Keymap]
    end

    subgraph "Math (math)"
        calc_engine[Calc_Engine]
        parser[Parser]
        layout_engine[Layout_Engine]
    end

    lvgl_kbd --> grid_layout
    lvgl_kbd --> layer_mgr
    lvgl_lcd --> calc_engine
    lvgl_lcd --> layer_mgr
    lvgl_lcd --> layout_engine
    sdl_input --> grid_layout
    sdl_input --> sdl_keymap

    layer_mgr --> keymap_core
    calc_engine --> parser
    calc_engine --> layout_engine

    style lvgl_kbd fill:#fff4e1
    style lvgl_lcd fill:#fff4e1
    style sdl_input fill:#fff4e1
    style sdl_keymap fill:#fff4e1
    style layer_mgr fill:#e1ffe1
    style grid_layout fill:#e1ffe1
    style keymap_core fill:#e1ffe1
    style calc_engine fill:#ffe1f5
    style parser fill:#ffe1f5
    style layout_engine fill:#ffe1f5
```

## Theme System

```mermaid
classDiagram
    class lvgl_theme {
        <<constants>>
        +LVGL_COLOR_BG_SCREEN: 0xF0F0F0
        +LVGL_COLOR_BG_BEZEL: 0xFFFFFF
        +LVGL_COLOR_TEXT_PRIMARY: 0x333333
        +LVGL_COLOR_ACCENT_BLUE: 0x6496FF
        +LVGL_COLOR_ACCENT_GREEN: 0x32C832
        +LVGL_FONT_DEFAULT: &lv_font_montserrat_14
        +LVGL_FONT_SMALL: &lv_font_montserrat_12
        +lvgl_color(hex): lv_color_t
    }

    class LVGL_Keyboard_View {
        +apply_style(key, pressed)
    }

    class LVGL_LCD_Display {
        +refresh()
        +draw_math_to_canvas()
    }

    LVGL_Keyboard_View --> lvgl_theme
    LVGL_LCD_Display --> lvgl_theme

    style lvgl_theme fill:#fff4e1
    style LVGL_Keyboard_View fill:#e1f5ff
    style LVGL_LCD_Display fill:#ffe1f5
```

## Layer Management

```mermaid
stateDiagram-v2
    [*] --> Basic: Initial state
    Basic --> Trig: Press LAYER_TRIG
    Basic --> Constants: Press LAYER_CONST
    Basic --> Programmer: Press LAYER_PROG
    Basic --> Algebra: Press LAYER_ALGEBRA

    Trig --> Basic: Press LAYER_HOME
    Constants --> Basic: Press LAYER_HOME
    Programmer --> Basic: Press LAYER_HOME
    Algebra --> Basic: Press LAYER_HOME

    Trig --> Constants: Press LAYER_CONST
    Constants --> Trig: Press LAYER_TRIG
    Constants --> Programmer: Press LAYER_PROG
    Programmer --> Constants: Press LAYER_CONST

    note right of Basic
        Default layer
        Digits, basic ops
        Cursor keys
    end note

    note right of Trig
        sin, cos, tan
        asin, acos, atan
    end note

    note right of Constants
        π, e, φ, τ
    end note

    note right of Programmer
        Hex A-F
        Bitwise ops
        Shift operators
    end note

    note right of Algebra
        Reciprocal, factorial
        Log, ln, exp
        Power functions
    end note
```
