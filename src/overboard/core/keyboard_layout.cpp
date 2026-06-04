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

    // Calculate total size using width/height (supports fractional sizes like 1.25u)
    int total_w = static_cast<int>(pos.width * static_cast<float>(cell_size.x));
    int total_h = static_cast<int>(pos.height * static_cast<float>(cell_size.y));

    // Position is based on start cell, with fractional gap offset
    int gap_x = static_cast<int>(pos.col_gap * static_cast<float>(cell_size.x));
    int gap_y = static_cast<int>(pos.row_gap * static_cast<float>(cell_size.y));

    int x = grid_origin.x + pos.col * cell_size.x + gap_x + padding / 2;
    int y = grid_origin.y + pos.row * cell_size.y + gap_y + padding / 2;

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
    // 8 cols × 8 rows:
    //   Cols 0-2 = left section (3 keys wide)
    //   Col  3   = gap column between left and right sections
    //   Cols 4-7 = right section (4 keys wide, numpad)
    //   Row  1   = gap row (right side only, left section starts at row 2)

    std::vector<Key_Position> positions;
    positions.reserve(34);

    // Layout structure (8 cols × 8 rows with spacing rows):
    //       Cols 0-2       | gap | Cols 4-7
    // Row 0: [0] [1]  [2]  |     | [3]  [4]  [5]  [6]      <- Top row
    // Row 1:                |     |                          <- GAP ROW (right side only)
    // Row 2: [11][12] [13]  |     | [7]  [8]  [9]  [10]     <- Left func row 1 + right extra row
    // Row 3: [18][19] [20]  |     | [14] [15] [16] [17tall] <- Left func row 2 + numpad 7,8,9 + tall Aprx
    // Row 4:                |     | [21] [22] [23] [17tall] <- numpad 4,5,6
    // Row 5:                |     | [24] [25] [26] [27tall] <- numpad 1,2,3 + tall Eval
    // Row 6:      [28]      |     | [29wide]  [30] [27tall] <- blank + wide 0 + .
    // Row 7: [31][32] [33]  |     |                          <- Arrow keys

    // --- Row 0: Top function row ---
    positions.push_back({0, 0, 1, 1});  // 0: Home
    positions.push_back({1, 0, 1, 1});  // 1: _
    positions.push_back({2, 0, 1, 1});  // 2: BSP
    positions.push_back({4, 0, 1, 1});  // 3: right-0
    positions.push_back({5, 0, 1, 1});  // 4: right-1
    positions.push_back({6, 0, 1, 1});  // 5: right-2
    positions.push_back({7, 0, 1, 1});  // 6: right-3

    // --- Row 1: GAP right side only (no keys placed) ---

    // --- Row 2: Left function row 1 + right extra row ---
    positions.push_back({4, 2, 1, 1});  // 7: right extra 1
    positions.push_back({5, 2, 1, 1});  // 8: right extra 2
    positions.push_back({6, 2, 1, 1});  // 9: right extra 3
    positions.push_back({7, 2, 1, 1});  // 10: right extra 4
    positions.push_back({0, 2, 1, 1});  // 11: left func 1
    positions.push_back({1, 2, 1, 1});  // 12: left func 2
    positions.push_back({2, 2, 1, 1});  // 13: left func 3

    // --- Row 3: Left function row 2 + numpad 7,8,9 + tall Aprx ---
    positions.push_back({4, 3, 1, 1});  // 14: 7
    positions.push_back({5, 3, 1, 1});  // 15: 8
    positions.push_back({6, 3, 1, 1});  // 16: 9
    positions.push_back({7, 3, 1, 2});  // 17: TALL Aprx (spans rows 3-4)
    positions.push_back({0, 3, 1, 1});  // 18: left func 4
    positions.push_back({1, 3, 1, 1});  // 19: left func 5
    positions.push_back({2, 3, 1, 1});  // 20: left func 6

    // --- Row 4: Numpad 4,5,6 (Aprx continues) ---
    positions.push_back({4, 4, 1, 1});  // 21: 4
    positions.push_back({5, 4, 1, 1});  // 22: 5
    positions.push_back({6, 4, 1, 1});  // 23: 6
    // (key 17 continues here)

    // --- Row 5: Numpad 1,2,3 + tall Eval ---
    positions.push_back({4, 5, 1, 1});  // 24: 1
    positions.push_back({5, 5, 1, 1});  // 25: 2
    positions.push_back({6, 5, 1, 1});  // 26: 3
    positions.push_back({7, 5, 1, 2});  // 27: TALL Eval (spans rows 5-6)

    // --- Row 6: blank + wide 0 + . ---
    positions.push_back({1, 6, 1, 1});  // 28: blank
    positions.push_back({4, 6, 2, 1});  // 29: WIDE 0 (spans cols 4-5)
    positions.push_back({6, 6, 1, 1});  // 30: .
    // (key 27 continues here)

    // --- Row 7: Arrow keys ---
    positions.push_back({0, 7, 1, 1});  // 31: ↑
    positions.push_back({1, 7, 1, 1});  // 32: ←
    positions.push_back({2, 7, 1, 1});  // 33: ↓

    return Grid_Layout(8, 8, std::move(positions));
}

} // namespace ovb::core
