// Project Libraries
#include <overboard/hal/sdl/sdl_input.hpp>

namespace ovb::hal::sdl {

SDL_Input::SDL_Input(uint32_t kbd_window_id,
                     int kbd_width,
                     int kbd_height,
                     const ovb::core::Grid_Layout& layout,
                     int header_height, int margin_left, int margin_top)
    : m_kbd_window_id(kbd_window_id), m_kbd_width(kbd_width), m_kbd_height(kbd_height),
      m_layout(layout), m_header_height(header_height),
      m_margin_left(margin_left), m_margin_top(margin_top) {}

bool SDL_Input::should_quit() const { return m_quit; }

int SDL_Input::hit_test(int mx, int my) const {
    const int grid_y = m_header_height + m_margin_top;
    if (my < grid_y) return -1;
    if (mx < m_margin_left) return -1;

    // Mirror draw_key geometry exactly:
    //   grid origin = (m_margin_left, m_header_height + m_margin_top)
    //   cell_w = (width - m_margin_left - KEY_PAD) / layout.cols()
    //   cell_h = (height - grid_y - KEY_PAD) / layout.rows()
    int grid_w = m_kbd_width  - m_margin_left - KEY_PAD;
    int grid_h = m_kbd_height - grid_y         - KEY_PAD;
    int cell_w = grid_w / m_layout.cols();
    int cell_h = grid_h / m_layout.rows();

    int rel_x = mx - m_margin_left;
    int rel_y = my - grid_y;

    int col = rel_x / cell_w;
    int row = rel_y / cell_h;

    if (col < 0 || col >= m_layout.cols() || row < 0 || row >= m_layout.rows()) return -1;

    // Use Grid_Layout to map grid cell to actual key index
    return m_layout.get_cell_owner(col, row);
}

void SDL_Input::pump() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            m_quit = true;
            return;
        }

        // Only handle events on the keyboard window
        if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.windowID == m_kbd_window_id) {
            int idx = hit_test(ev.button.x, ev.button.y);
            if (idx >= 0)
                m_event_queue.push({idx, Key_Event_Type::Press});
        }
        if (ev.type == SDL_MOUSEBUTTONUP && ev.button.windowID == m_kbd_window_id) {
            int idx = hit_test(ev.button.x, ev.button.y);
            if (idx >= 0)
                m_event_queue.push({idx, Key_Event_Type::Release});
        }
    }
}

bool SDL_Input::poll(Key_Event& out_event) {
    if (m_event_queue.empty()) return false;
    out_event = m_event_queue.front();
    m_event_queue.pop();
    return true;
}

} // namespace ovb::hal::sdl
