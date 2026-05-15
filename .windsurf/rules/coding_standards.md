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
