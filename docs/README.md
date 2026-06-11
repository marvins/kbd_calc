# Documentation

Complete documentation for the kbd_calc calculator project.

## Core Documentation

### [Architecture](architecture.md)
System architecture, layer overview, module descriptions, input architecture (text-first), VIA macropad configuration, and build system.

**Key topics:**
- Source layout and dependency rules
- Input architecture (text-first)
- Event filter routing (printable vs non-printable keys)
- Virtual keyboard label emission
- VIA macropad configuration (F13-F24)
- SDL simulator and hardware targets

### [Custom Fonts](custom_fonts.md)
Font generation and integration for math typesetting, including superscript glyphs for power buttons.

## Project Planning

### [Tasks & Roadmap](tasks.md)
Current development phases, active tasks, and future feature backlog organized by priority.

## Implementation Details

### [Math Canvas Refactor Ideas](math_canvas_refactor_ideas.md)
Proposed improvements for the math rendering system.

## Quick Start

1. **Understanding the system:** Start with [Architecture](architecture.md)
2. **Input handling:** Read the "Input Architecture (Text-First)" section
3. **Building:** See the "Build System" section in architecture.md
4. **VIA configuration:** See the "VIA Macropad Configuration" section
