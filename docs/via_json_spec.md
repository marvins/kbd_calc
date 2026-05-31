# VIA JSON Specification

This document describes the VIA JSON format for keyboard layout definitions, as used by the VIA configurator.

## Overview

VIA requires a definition of both the physical arrangement of keys and the mapping of those keys to the switch matrix. It optionally requires definition of layout options - alternative physical arrangements of keys, such as different bottom rows, split right shift, etc.

### Key Properties

- **`keymap`**: Contains KLE JSON data defining the physical layout and switch matrix coordinates
- **`labels`**: Optional array naming the layout options

The `keymap` property contains KLE JSON data that can be edited using Keyboard Layout Editor. This contains the switch matrix coordinates for each key, and optionally information about layout options.

## KLE JSON Rules

The KLE JSON should follow these rules:

- Horizontal and vertical gaps are allowed (for separation of rows, columns, blockers, etc.)
- Do not use stepped keys
- Rotated keys are allowed but layout options for rotated keys are not supported
- Use key color `#cccccc` for alphas, `#aaaaaa` for modifier keys, and `#777777` for accents (Esc, Enter, arrows, etc.)
- Use legend color `#000000` for all legends

### Key Color Guidelines

| Color | Usage |
|-------|-------|
| `#cccccc` | Alpha keys |
| `#aaaaaa` | Modifier keys |
| `#777777` | Accent keys (Esc, Enter, arrows, etc.) |
| `#000000` | All legends |

Note: The key colors used in the KLE will be used by VIA to map alphas/modifiers/accents to a color theme. All keys which are the "modifier" color in a standard keycap set should be set to `#aaaaaa`.

## Switch Matrix Coordinates

The mapping from physical layout to switch matrix layout is defined by the **top-left legend** of the key, using `row,col` format.

```
row,col
```

This replaces the mapping that is usually done by a `LAYOUT_*()` macro in QMK. Most `LAYOUT_*()` macros in QMK use some row,col naming convention, so translation into the KLE JSON should be straightforward.

### Example

**KLE Example:**
```json
{
  "label": "0,0",
  ...
}
```

**QMK Layout Macro Example:**
```c
#define LAYOUT_60_all( \
    K00, K01, K02, K03, K04, ... \
    K10, K11, K12, K13, K14, ... \
    K20, K21, K22, K23, K24, ... \
    ...
)
```

## Rotary Encoders

You can set up your VIA layout to display rotary encoders in the UI.

### Required Setup in QMK

- Enable encoder maps as per QMK
- Add `ENCODER_ENABLE = yes` to `rules.mk`
- Add `ENCODER_MAP_ENABLE = yes` to `keymaps/via/rules.mk`
- Add an encoder map to `keymaps/via/keymap.c`
- The encoder map should be defined for the same number of layers as configured for VIA (default 4)
- Add the encoder to the VIA keyboard definition

### Encoder Types

#### Encoders Without Push Switch (Just Twist, No Push)

Add a "key" to the KLE JSON with `e0`, `e1`, etc. as the center label. The number will match the encoder ID used in the encoder map.

#### Rotary Encoder With Push Switch

Define the switch matrix coordinates like other switches and add `e0`, `e1`, etc. to the center label of the switch.

#### Optional Rotary Encoder (Combined Switch/Rotary Encoder Footprint)

Use VIA Layout Options like other switches. VIA can render either a knob, switch, or empty space.

## Layout Options

If a keyboard supports multiple physical layouts of keys, the KLE JSON definition will contain the "default" layout as well as all the layout options.

### Layout Option Format

Layout options are defined by the **bottom-right legend** of the key, using `option,choice` format:

```
option,choice
```

- **Layout option**: An area of the keyboard which can change into more than one arrangement
- **Layout option choice**: One arrangement of a layout option

Each layout option is defined by a number (the first number in the bottom-right legend). The second number defines which "choice" it belongs to within that layout option.

### Default Layout

The "default layout" (what is presented in VIA by default) is defined as:
- All keys without the `option,choice` in the bottom-right label
- All keys which have `option,0` in the bottom-right label (the "default" choice for all layout options)

VIA will use the bounding box of all keys in the "default layout" as the extent of the "keyboard", so layout option choices can be positioned above, below, left, or right of this bounding box.

### Layout Option Requirements

- Layout option choices must all have the same coverage (overlap exactly in shape)
- They can vary in keycap sizes and arrangement
- Layout option choices must be aligned either vertically or horizontally with the "default" layout option
- All choices must contain the switch matrix coordinates in the top-left label
- Setting switch matrix coordinates may require testing real hardware (e.g., non-split center switch of a split shift may be connected in parallel to the switch to the left or right)

### Layout Option Guidelines

- Layout options should be at the finest granularity possible
- For example, ANSI/ISO should be defined as three separate layout options: ANSI/ISO Enter, Split Left Shift, Split Right Shift
- Layout options can use "decal" keys to represent optional blockers (e.g., HHKB or WKL)
- VIA will present the layout options to the user and store the state of the choices in the device

### Example: Split Backspace

This example defines the layout option "Split Backspace":

- The 2U backspace legend is `0,0` (layout option #0, choice #0)
- The 2U backspace is the default (because it is `-`,0`)
- The 2U backspace is positioned as it should be relative to other constant keys (because it is the default)
- The split backspace keys have legends `0,1` (layout option #0, choice #1)
- The split backspace keys have the same bounding box as the 2U backspace
- The split backspace keys bounding box is aligned vertically with the 2U backspace
- All keys have switch matrix coordinates in the top-left legends

## Layout Option Labels

Layout options are assigned labels from the `labels` property.

### Labels Format

The `labels` property is an optional array of `string` or `string[]` that defines the labels for the layout controls.

```json
{
  "layouts": {
    "labels": [
      "Split Backspace",
      "ISO Enter",
      "Split Left Shift",
      "Split Right Shift",
      ["Bottom Row", "ANSI", "7U", "HHKB", "WKL"]
    ]
  }
}
```

### Label Types

#### String Labels (Toggle)

If an item in the labels array is a `string`, it is presented as a toggle button:
- Off state maps to layout option choice #0 (the default)
- On state maps to layout option choice #1

Example: `"Split Backspace"` corresponds to layout option #0

#### String Array Labels (Select)

If an item in the labels array is a `string[]`, it maps to a select control:
- The first item is the label for the control
- Following items are labels for layout option choices #0, #1, #2, etc.

Example: `["Bottom Row", "ANSI", "7U", "HHKB", "WKL"]`
- "Bottom Row" is the control label
- "ANSI" maps to layout option choice #0
- "7U" maps to layout option choice #1
- etc.

### Label Order

The order of the labels is important as the implicit index is used to map to the group number:
- Index 0 → Layout option #0
- Index 1 → Layout option #1
- etc.

## File Structure

The complete VIA JSON file structure:

```json
{
  "keyboard_name": "Keyboard Name",
  "layouts": {
    "keymap": [
      // KLE JSON array defining the layout
    ],
    "labels": [
      // Optional layout option labels
    ]
  }
}
```

## Working with Keyboard Layout Editor (KLE)

- Keyboard Layout Editor will not load the `<keyboard_name>.json` that VIA uses directly
- Only the KLE JSON within the `keymap` property can be loaded
- Copy the value of the `keymap` property into the "Raw data" tab of Keyboard Layout Editor (everything inside the outer `[` and `]`)
- Copying and pasting the other way does not work (KLE's "Raw data" tab contains JSON with missing quotes around property names)
- In KLE, save as a JSON file and then copy the contents of the JSON file into the `keymap` property of the `<keyboard_name>.json`
