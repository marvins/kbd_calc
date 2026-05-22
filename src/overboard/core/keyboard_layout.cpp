// C++ Standard Libraries
#include <algorithm>
#include <cstring>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::core {

/****************************/
/*     Constructors         */
/****************************/

Grid_Layout::Grid_Layout(int cols, int rows)
    : m_cols(cols), m_rows(rows),
      m_cell_owners(static_cast<std::size_t>(cols) * static_cast<std::size_t>(rows), -1) {}

Grid_Layout::Grid_Layout(int cols, int rows, std::vector<Key_Position> key_positions)
    : m_cols(cols), m_rows(rows),
      m_key_positions(std::move(key_positions)),
      m_cell_owners(static_cast<std::size_t>(cols) * static_cast<std::size_t>(rows), -1) {
    rebuild_cell_map();
}

/****************************/
/*     Key Definition       */
/****************************/

bool Grid_Layout::define_key(int key_index, Key_Position pos) {
    // Validate position is within grid
    if (!is_valid_cell(pos.col, pos.row)) {
        return false;
    }
    if (pos.col + pos.col_span > m_cols || pos.row + pos.row_span > m_rows) {
        return false;
    }
    if (pos.col_span < 1 || pos.row_span < 1) {
        return false;
    }

    // Ensure key_index can be stored
    if (key_index < 0) {
        return false;
    }
    if (static_cast<std::size_t>(key_index) >= m_key_positions.size()) {
        m_key_positions.resize(static_cast<std::size_t>(key_index) + 1);
    }

    // Check for conflicts with existing keys
    for (int r = pos.row; r < pos.row + pos.row_span; ++r) {
        for (int c = pos.col; c < pos.col + pos.col_span; ++c) {
            int owner = m_cell_owners[static_cast<std::size_t>(cell_index(c, r))];
            if (owner >= 0 && owner != key_index) {
                return false; // Cell already occupied by another key
            }
        }
    }

    // Remove old cells owned by this key index
    for (auto& owner : m_cell_owners) {
        if (owner == key_index) {
            owner = -1;
        }
    }

    // Set new position
    m_key_positions[static_cast<std::size_t>(key_index)] = pos;

    // Mark cells as owned by this key
    for (int r = pos.row; r < pos.row + pos.row_span; ++r) {
        for (int c = pos.col; c < pos.col + pos.col_span; ++c) {
            m_cell_owners[static_cast<std::size_t>(cell_index(c, r))] = key_index;
        }
    }

    return true;
}

std::optional<Key_Position> Grid_Layout::get_key_position(int key_index) const {
    if (key_index < 0 || static_cast<std::size_t>(key_index) >= m_key_positions.size()) {
        return std::nullopt;
    }
    return m_key_positions[static_cast<std::size_t>(key_index)];
}

/****************************/
/*     Cell Queries         */
/****************************/
Cell_Type Grid_Layout::get_cell_type(int col, int row) const {
    if (!is_valid_cell(col, row)) {
        return Cell_Type::EMPTY;
    }

    int owner = m_cell_owners[static_cast<std::size_t>(cell_index(col, row))];
    if (owner < 0) {
        return Cell_Type::EMPTY;
    }

    // Check if this is the start cell for the key
    auto pos_opt = get_key_position(owner);
    if (!pos_opt) {
        return Cell_Type::EMPTY;
    }

    const auto& pos = *pos_opt;
    if (col == pos.col && row == pos.row) {
        return Cell_Type::KEY_START;
    }
    return Cell_Type::KEY_CONT;
}

int Grid_Layout::get_cell_owner(int col, int row) const {
    if (!is_valid_cell(col, row)) {
        return -1;
    }
    return m_cell_owners[static_cast<std::size_t>(cell_index(col, row))];
}

/****************************/
/*     Rectangle Calc       */
/****************************/
std::optional<core::Rect<int>> Grid_Layout::get_key_rect(
    int key_index,
    core::Point<int> grid_origin,
    core::Point<int> cell_size,
    int padding) const {

    auto pos_opt = get_key_position(key_index);
    if (!pos_opt) {
        return std::nullopt;
    }

    const auto& pos = *pos_opt;

    // Calculate total size including spans
    int total_w = pos.col_span * cell_size.x;
    int total_h = pos.row_span * cell_size.y;

    // Position is based on start cell
    int x = grid_origin.x + pos.col * cell_size.x + padding / 2;
    int y = grid_origin.y + pos.row * cell_size.y + padding / 2;

    // Adjust for padding (only apply padding between keys, not inside)
    int w = total_w - padding;
    int h = total_h - padding;

    return core::Rect<int>{x, y, w, h};
}

core::Rect<int> Grid_Layout::get_cell_rect(
    int col, int row,
    core::Point<int> grid_origin,
    core::Point<int> cell_size) const {

    int x = grid_origin.x + col * cell_size.x;
    int y = grid_origin.y + row * cell_size.y;
    return core::Rect<int>{x, y, cell_size.x, cell_size.y};
}

/****************************/
/*     Helpers              */
/****************************/

void Grid_Layout::rebuild_cell_map() {
    std::fill(m_cell_owners.begin(), m_cell_owners.end(), -1);

    for (size_t key_idx = 0; key_idx < m_key_positions.size(); ++key_idx) {
        const auto& pos = m_key_positions[key_idx];
        // Skip uninitialized positions
        if (pos.col_span == 0 && pos.row_span == 0) {
            continue;
        }

        for (int r = pos.row; r < pos.row + pos.row_span; ++r) {
            for (int c = pos.col; c < pos.col + pos.col_span; ++c) {
                if (is_valid_cell(c, r)) {
                    m_cell_owners[static_cast<std::size_t>(cell_index(c, r))] = static_cast<int>(key_idx);
                }
            }
        }
    }
}

/****************************/
/*     Factory Methods      */
/****************************/

Grid_Layout Grid_Layout::standard_5x6() {
    std::vector<Key_Position> positions;
    positions.reserve(30);

    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 5; ++col) {
            positions.push_back({col, row, 1, 1});
        }
    }

    return Grid_Layout(5, 6, std::move(positions));
}

/**
 * @brief KISNT KN34-style layout
 *
 * Visual layout matching user's design:
 * - Top: 3 keys (left), gap (1u), 4 keys (right/numpad top)
 * - Middle: arrow keys in upside-down T on left, numpad middle on right
 * - Bottom: more keys, wide 0, tall Enter
 *
 * Grid: 8 columns × 6 rows (3 left + 1 gap + 4 right)
 */
Grid_Layout Grid_Layout::kn34() {
    // 9×7 grid for better spacing:
    //   Cols 0-2 = left section (3 keys wide)
    //   Col 3 = gap between left function and arrows
    //   Col 4 = arrows column (centered)
    //   Col 5 = gap to right section
    //   Cols 6-9 = right section (4 keys wide, numpad)
    //
    // Actually simpler: use 8 cols but add empty rows for spacing

    std::vector<Key_Position> positions;
    positions.reserve(30);

    // Layout structure (8 cols × 7 rows with spacing rows):
    //       Cols 0-2     | gap | Cols 4-7
    // Row 0: [0] [1] [2]  |     | [3] [4] [5] [6]     <- Top row (no gap in grid, just empty col 3)
    // Row 1:               |     |                     <- GAP ROW (empty)
    // Row 2: [←] [↑] [→]  |     | [7] [8] [9] [10tall] <- Arrows (upside-down T start) + numpad
    // Row 3:      [↓]     |     | [11][12][13][10tall] <- Down arrow (centered) + numpad
    // Row 4:               |     |                     <- GAP ROW (empty)
    // Row 5: [14][15][16] |     | [17wide]   [18][19tall] <- Bottom left + wide 0 + Enter
    // Row 6:               |     |                     <- Row 5 continued for tall keys
    //
    // 10tall = + (spans rows 2-3), 19tall = Enter (spans rows 5-6... need to check)

    // Let's use 8 cols × 6 rows with proper key placement:

    // --- Row 0: Top function row ---
    positions.push_back({0, 0, 1, 1});  // 0
    positions.push_back({1, 0, 1, 1});  // 1
    positions.push_back({2, 0, 1, 1});  // 2
    positions.push_back({4, 0, 1, 1});  // 3
    positions.push_back({5, 0, 1, 1});  // 4
    positions.push_back({6, 0, 1, 1});  // 5
    positions.push_back({7, 0, 1, 1});  // 6

    // --- Row 1: 4 blank buttons above numpad ---
    positions.push_back({4, 1, 1, 1});  // 7: Extra 1
    positions.push_back({5, 1, 1, 1});  // 8: Extra 2
    positions.push_back({6, 1, 1, 1});  // 9: Extra 3
    positions.push_back({7, 1, 1, 1});  // 10: Extra 4

    // --- Row 2: Left section + Numpad 7,8,9 + tall + ---
    positions.push_back({0, 2, 1, 1});  // 11: (was r2c0)
    positions.push_back({1, 2, 1, 1});  // 12: (was r2c1)
    positions.push_back({2, 2, 1, 1});  // 13: (was r2c2)
    positions.push_back({4, 2, 1, 1});  // 14: 7
    positions.push_back({5, 2, 1, 1});  // 15: 8
    positions.push_back({6, 2, 1, 1});  // 16: 9
    positions.push_back({7, 2, 1, 2});  // 17: TALL PLUS (spans rows 2-3)

    // --- Row 3: Left section (was r3) + numpad 4,5,6 ---
    positions.push_back({0, 3, 1, 1});  // 18: (was r3c0)
    positions.push_back({1, 3, 1, 1});  // 19: (was r3c1)
    positions.push_back({2, 3, 1, 1});  // 20: (was r3c2)
    positions.push_back({4, 3, 1, 1});  // 21: 4
    positions.push_back({5, 3, 1, 1});  // 22: 5
    positions.push_back({6, 3, 1, 1});  // 23: 6
    // (key 17 continues here)

    // --- Row 4: Empty left + numpad 1,2,3 + Enter ---
    positions.push_back({4, 4, 1, 1});  // 24: 1
    positions.push_back({5, 4, 1, 1});  // 25: 2
    positions.push_back({6, 4, 1, 1});  // 26: 3
    positions.push_back({7, 4, 1, 2});  // 27: TALL ENTER (spans rows 4-5)

    // --- Row 5: [blank] + wide 0 (c4-5) + [→] at c6 + Enter cont ---
    positions.push_back({1, 5, 1, 1});  // Blank
    positions.push_back({4, 5, 2, 1});  // WIDE 0 (spans cols 4-5, moved right)
    positions.push_back({6, 5, 1, 1});  // Right arrow (moved to c6)
    // (key 27 continues here)

    // --- Row 6: [↑][←][↓] arrows ---
    positions.push_back({0, 6, 1, 1});  // Up arrow
    positions.push_back({1, 6, 1, 1});  // Left arrow (moved down and right)
    positions.push_back({2, 6, 1, 1});  // Down arrow

    return Grid_Layout(8, 7, std::move(positions));
}

} // namespace ovb::core
