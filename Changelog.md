# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.1.0] - 2026-05-29

### Added
- Initial PicoCalc HAL implementation (`hal/picocalc/`)
  - ILI9488 SPI display driver (320×320)
  - I2C keyboard driver (STM32 controller)
  - `PicoCalc_App` lifecycle
- Renamed `hal/kn34/` to `hal/pico/` (RP2350 stub)
- Added `BOARD` selection under `TARGET_RP2350` (picocalc/pico)
- Added `TARGET_DEVICE=PICOCALC` convenience option
- Moved MF34 configs to `data/configs/mf/`
- Added PicoCalc VIA layout configs to `data/configs/picocalc/`
- Converted `display_config.hpp` to CMake template (`templates/display_config.hpp.in`)
- Per-target display dimensions (SDL: 400×800, PicoCalc: 320×320)
- Updated architecture documentation with Mermaid diagrams
- Added brace-init style for `constexpr` constants to coding standards
