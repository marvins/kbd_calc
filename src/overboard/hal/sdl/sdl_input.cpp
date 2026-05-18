// Project Libraries
#include <overboard/hal/sdl/sdl_input.hpp>

namespace ovb::hal::sdl {

SDL_Input::SDL_Input(SDL_Display& kbd_display, int cols, int rows,
                     int header_height, int margin_left, int margin_top)
    : m_kbd_display(kbd_display), m_cols(cols), m_rows(rows),
      m_header_height(header_height), m_margin_left(margin_left), m_margin_top(margin_top) {}

bool SDL_Input::should_quit() const { return m_quit; }

int SDL_Input::hit_test(int mx, int my) const {
    const int grid_y = m_header_height + m_margin_top;
    if (my < grid_y) return -1;
    if (mx < m_margin_left) return -1;

    // Mirror draw_key geometry exactly:
    //   grid origin = (m_margin_left, m_header_height + m_margin_top)
    //   cell_w = (width - m_margin_left - KEY_PAD) / m_cols
    //   cell_h = (height - grid_y - KEY_PAD) / m_rows
    int grid_w = m_kbd_display.width()  - m_margin_left - KEY_PAD;
    int grid_h = m_kbd_display.height() - grid_y         - KEY_PAD;
    int cell_w = grid_w / m_cols;
    int cell_h = grid_h / m_rows;

    int rel_x = mx - m_margin_left;
    int rel_y = my - grid_y;

    int col = rel_x / cell_w;
    int row = rel_y / cell_h;

    if (col < 0 || col >= m_cols || row < 0 || row >= m_rows) return -1;
    return row * m_cols + col;
}

void SDL_Input::pump() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            m_quit = true;
            return;
        }

        // Only handle events on the keyboard window
        Uint32 kbd_id = SDL_GetWindowID(m_kbd_display.window());

        if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.windowID == kbd_id) {
            int idx = hit_test(ev.button.x, ev.button.y);
            if (idx >= 0)
                m_event_queue.push({idx, Key_Event_Type::Press});
        }
        if (ev.type == SDL_MOUSEBUTTONUP && ev.button.windowID == kbd_id) {
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
