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

## General
- Standard: **C++23**
- Build system: **CMake 4.0+**
- Interfaces (pure virtual base classes) prefixed with `I_` — e.g. `I_Display`, `I_Input`
- HAL implementations live under `src/hal/<platform>/`
- Core portable logic lives under `src/core/` — **no platform headers allowed here**
- Prefer `const` references over copies for non-trivial parameters
- All files end with a newline

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
