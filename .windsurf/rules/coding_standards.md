---
trigger: always_on
---

# C++ Coding Standards

## Naming
- **Classes**: `Snake_Case` with first letter of each word capitalised — e.g. `Layer_Manager`, `Calc_Engine`
- **Acronyms in class names**: stay fully uppercase — e.g. `SDL_Display`, `I2C_Driver`, `USB_Hub`
- **Files**: `snake_case.cpp` / `snake_case.hpp`
- **Member variables**: `snake_case_` (trailing underscore)
- **Local variables / parameters**: `snake_case`
- **Constants / `constexpr`**: `UPPER_CASE`
- **Enums and enum values**: `Snake_Case` class enum, values `Snake_Case` — e.g. `Key_Event_Type::Press`

## Class Layout
Access specifiers indented **4 spaces**, members/methods indented **8 spaces**:

```cpp
class My_Class {
    public:
        My_Class();
        void do_thing();

    private:
        int value_;
        void helper();
};
```

Within `public` sections, order members as follows:
1. **`static constexpr` constants** (before any methods)
2. **Methods** (constructors, then all other methods)

Within `private` and `protected` sections, order members as follows:
1. **Instance methods** (non-static member functions)
2. **Static methods** (static member functions)
3. **Member variables** (non-static data members)
4. **Static member variables / `static constexpr`** (static data members)

## Constant Initializers
Prefer brace-initializer syntax for `constexpr` and `inline constexpr` constants:

```cpp
// Preferred
inline constexpr int    FULL_WIDTH  { 400 };
inline constexpr uint32_t I2C_SPEED { 400'000 };

// Avoid
inline constexpr int FULL_WIDTH = 400;
```

Digit separators (`'`) should be used for numeric literals ≥ 5 digits to aid readability:
```cpp
inline constexpr uint32_t SPI_SPEED { 25'000'000 };
```

## General
- Standard: **C++23**
- Build system: **CMake 4.0+**
- Interfaces (pure virtual base classes) prefixed with `I_` — e.g. `I_Display`, `I_Input`
- HAL implementations live under `src/hal/<platform>/`
- Core portable logic lives under `src/core/` — **no platform headers allowed here**
- Prefer `const` references over copies for non-trivial parameters
- All files end with a newline

## .cpp File Organization
In `.cpp` source files, add a 3-line comment block before each function implementation to aid navigation when scrolling:

```cpp
/***************************/
/*        Window ID        */
/***************************/
uint32_t SDL_Display::window_id() const {
    return m_impl->sdl_window ? SDL_GetWindowID(m_impl->sdl_window) : 0;
}
```

Format rules — follow these exactly:
1. **Center line**: `/*` + 8 spaces + text + 8 spaces + `*/`
2. **Top and bottom lines**: `/*` + asterisks + `*/`, where the total character count of the top/bottom line **must equal** the total character count of the center line (count every character including `/*`, spaces, text, and `*/`)
3. Number of asterisks = (center line length) − 4  (subtracting the 2 chars of `/*` and 2 chars of `*/`)
```
/***************************/
/*        Window ID        */
/***************************/
```

Use descriptive, concise text.

## File Header Format
All header and source files must follow this structure:

```cpp
/**
 * @file    filename.hpp
 * @author  <author>
 * @date    <date>
 *
 * @brief   Brief description of file purpose
 *
 * Optional longer description if needed.
 */
#pragma once

// C++ Standard Libraries
#include <string>
#include <vector>

// Third-Party Libraries (if any)
#include <SDL2/SDL.h>

// Project Libraries
#include <overboard/core/something.hpp>

namespace ovb::module {
// code
}
```

Note that section headers should be omitted entirely if that section has no includes. For example, if a file has no third-party libraries, do not include the "// Third-Party Libraries" header at all.

Within each include section, headers must be **alphabetized** unless there is a technical reason not to (e.g. a header that must be included before another to satisfy a dependency). If order matters, add a brief inline comment explaining why.

For `.cpp` files, the associated header (the `.hpp` with the same base name) must be included **first**, before all other includes and before any section comment. The remaining includes then follow in the normal grouped order:

```cpp
/**
 * @file calc_engine.cpp
 * ...
 */
#include <overboard/math/calc_engine.hpp>

// C++ Standard Libraries
#include <cmath>
#include <string>

// Project Libraries
#include <overboard/math/parser.hpp>
```
