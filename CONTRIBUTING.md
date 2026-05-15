# Contributing

## C++ Coding Standards

### Naming

| Element | Convention | Example |
|---------|-----------|---------|
| Classes | `Snake_Case` (each word capitalised) | `Layer_Manager`, `Calc_Engine` |
| Acronyms in class names | Stay fully uppercase | `SDL_Display`, `I2C_Driver` |
| Interfaces (pure virtual) | `I_` prefix | `I_Display`, `I_Input` |
| Files | `snake_case` | `sdl_display.cpp`, `calc_engine.hpp` |
| Member variables | `snake_case_` (trailing underscore) | `active_layer_`, `width_` |
| Local variables / parameters | `snake_case` | `key_index`, `cell_w` |
| Constants / `constexpr` | `UPPER_CASE` | `KEY_PAD`, `HDR_H` |
| Enums | `Snake_Case` type, `Snake_Case` values | `Key_Event_Type::Press` |

### Class Layout

Access specifiers at **4 spaces**, members/methods at **8 spaces**:

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

### General Rules

- **Standard**: C++23
- **Build system**: CMake 4.0+
- Core portable logic lives under `src/core/` — no platform-specific headers allowed here
- HAL implementations live under `src/hal/<platform>/`
- Prefer `const` references over copies for non-trivial parameters
- All files end with a newline

## Build

```bash
# Debug (default)
./scripts/build.sh

# Release
./scripts/build.sh -r

# Clean + rebuild
./scripts/build.sh -c -r

# Help
./scripts/build.sh -h
```

## Project Structure

```
src/
├── core/       # Portable business logic (no SDL, no hardware)
├── hal/
│   ├── i_display.hpp   # I_Display interface
│   ├── i_input.hpp     # I_Input interface
│   └── sdl/            # SDL2 simulator HAL
```
