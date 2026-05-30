#!/usr/bin/env python
#
#    File:    key_mapper.py
#    Author:  Marvin Smith
#    Date:    2026-05-22
#
"""
Key Mapper GUI Tool

A PyQt-based tool for visualizing and editing keyboard key mappings.
Loads VIA layout JSON and a mapping JSON, allowing interactive key assignment.
"""

#  Python Standard Libraries
import json
import sys
from pathlib import Path
from typing import Dict, List, Optional

#  PyQt6
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QLabel, QLineEdit, QFileDialog, QMessageBox, QScrollArea, QDialog, QComboBox, QTabWidget, QCompleter
)
from PyQt6.QtCore import Qt, QSize
from PyQt6.QtGui import QFont, QKeyEvent

# Mapping from Qt key codes to SDL scancode name strings
QT_KEY_TO_SDL_SCANCODE = {
    # Function keys
    Qt.Key.Key_F1:  "SDL_SCANCODE_F1",   Qt.Key.Key_F2:  "SDL_SCANCODE_F2",
    Qt.Key.Key_F3:  "SDL_SCANCODE_F3",   Qt.Key.Key_F4:  "SDL_SCANCODE_F4",
    Qt.Key.Key_F5:  "SDL_SCANCODE_F5",   Qt.Key.Key_F6:  "SDL_SCANCODE_F6",
    Qt.Key.Key_F7:  "SDL_SCANCODE_F7",   Qt.Key.Key_F8:  "SDL_SCANCODE_F8",
    Qt.Key.Key_F9:  "SDL_SCANCODE_F9",   Qt.Key.Key_F10: "SDL_SCANCODE_F10",
    Qt.Key.Key_F11: "SDL_SCANCODE_F11",  Qt.Key.Key_F12: "SDL_SCANCODE_F12",
    Qt.Key.Key_F13: "SDL_SCANCODE_F13",  Qt.Key.Key_F14: "SDL_SCANCODE_F14",
    Qt.Key.Key_F15: "SDL_SCANCODE_F15",  Qt.Key.Key_F16: "SDL_SCANCODE_F16",
    Qt.Key.Key_F17: "SDL_SCANCODE_F17",  Qt.Key.Key_F18: "SDL_SCANCODE_F18",
    Qt.Key.Key_F19: "SDL_SCANCODE_F19",  Qt.Key.Key_F20: "SDL_SCANCODE_F20",
    # Numpad digits
    Qt.Key.Key_0: "SDL_SCANCODE_0",  Qt.Key.Key_1: "SDL_SCANCODE_1",
    Qt.Key.Key_2: "SDL_SCANCODE_2",  Qt.Key.Key_3: "SDL_SCANCODE_3",
    Qt.Key.Key_4: "SDL_SCANCODE_4",  Qt.Key.Key_5: "SDL_SCANCODE_5",
    Qt.Key.Key_6: "SDL_SCANCODE_6",  Qt.Key.Key_7: "SDL_SCANCODE_7",
    Qt.Key.Key_8: "SDL_SCANCODE_8",  Qt.Key.Key_9: "SDL_SCANCODE_9",
    # Navigation
    Qt.Key.Key_Left:     "SDL_SCANCODE_LEFT",
    Qt.Key.Key_Right:    "SDL_SCANCODE_RIGHT",
    Qt.Key.Key_Up:       "SDL_SCANCODE_UP",
    Qt.Key.Key_Down:     "SDL_SCANCODE_DOWN",
    Qt.Key.Key_Home:     "SDL_SCANCODE_HOME",
    Qt.Key.Key_End:      "SDL_SCANCODE_END",
    Qt.Key.Key_PageUp:   "SDL_SCANCODE_PAGEUP",
    Qt.Key.Key_PageDown: "SDL_SCANCODE_PAGEDOWN",
    Qt.Key.Key_Insert:   "SDL_SCANCODE_INSERT",
    Qt.Key.Key_Delete:   "SDL_SCANCODE_DELETE",
    # Editing
    Qt.Key.Key_Return:    "SDL_SCANCODE_RETURN",
    Qt.Key.Key_Backspace: "SDL_SCANCODE_BACKSPACE",
    Qt.Key.Key_Tab:       "SDL_SCANCODE_TAB",
    Qt.Key.Key_Escape:    "SDL_SCANCODE_ESCAPE",
    Qt.Key.Key_Space:     "SDL_SCANCODE_SPACE",
    # Operators / punctuation
    Qt.Key.Key_Minus:        "SDL_SCANCODE_MINUS",
    Qt.Key.Key_Equal:        "SDL_SCANCODE_EQUALS",
    Qt.Key.Key_Slash:        "SDL_SCANCODE_SLASH",
    Qt.Key.Key_Backslash:    "SDL_SCANCODE_BACKSLASH",
    Qt.Key.Key_Period:       "SDL_SCANCODE_PERIOD",
    Qt.Key.Key_Comma:        "SDL_SCANCODE_COMMA",
    Qt.Key.Key_Semicolon:    "SDL_SCANCODE_SEMICOLON",
    Qt.Key.Key_Apostrophe:   "SDL_SCANCODE_APOSTROPHE",
    Qt.Key.Key_BracketLeft:  "SDL_SCANCODE_LEFTBRACKET",
    Qt.Key.Key_BracketRight: "SDL_SCANCODE_RIGHTBRACKET",
    Qt.Key.Key_QuoteLeft:    "SDL_SCANCODE_GRAVE",
    # Numpad
    Qt.Key.Key_division:  "SDL_SCANCODE_KP_DIVIDE",
    Qt.Key.Key_multiply:  "SDL_SCANCODE_KP_MULTIPLY",
    Qt.Key.Key_Plus:      "SDL_SCANCODE_KP_PLUS",
    Qt.Key.Key_NumLock:   "SDL_SCANCODE_NUMLOCKCLEAR",
    # Modifiers (usually not useful as trigger keys, but map them anyway)
    Qt.Key.Key_Shift:   "SDL_SCANCODE_LSHIFT",
    Qt.Key.Key_Control: "SDL_SCANCODE_LCTRL",
    Qt.Key.Key_Alt:     "SDL_SCANCODE_LALT",
    Qt.Key.Key_Meta:    "SDL_SCANCODE_LGUI",
    Qt.Key.Key_CapsLock:   "SDL_SCANCODE_CAPSLOCK",
    Qt.Key.Key_ScrollLock: "SDL_SCANCODE_SCROLLLOCK",
    Qt.Key.Key_Print:      "SDL_SCANCODE_PRINTSCREEN",
    Qt.Key.Key_Pause:      "SDL_SCANCODE_PAUSE",
}


def qt_key_to_sdl_scancode(key: Qt.Key, text: str) -> str:
    """Convert a Qt key event to an SDL_SCANCODE_* name string."""
    # Check direct mapping first
    if key in QT_KEY_TO_SDL_SCANCODE:
        return QT_KEY_TO_SDL_SCANCODE[key]
    # Fall back to letter keys A-Z
    if text and len(text) == 1 and text.upper().isalpha():
        return f"SDL_SCANCODE_{text.upper()}"
    return ""


# Available key codes (from C++ keymap.hpp Key_Code enum)
AVAILABLE_KEY_CODES = [
    "NONE",
    # Digits
    "DIGIT_0", "DIGIT_1", "DIGIT_2", "DIGIT_3", "DIGIT_4",
    "DIGIT_5", "DIGIT_6", "DIGIT_7", "DIGIT_8", "DIGIT_9",
    # Basic ops
    "ADD", "SUBTRACT", "MULTIPLY", "DIVIDE", "EQUALS", "EVAL", "APPROX",
    "DECIMAL", "CLEAR", "ALL_CLEAR", "BACKSPACE",
    # Grouping / extra basic
    "PAREN_OPEN", "PAREN_CLOSE", "PERCENT", "NEGATE",
    # Memory
    "MEM_STORE", "MEM_RECALL", "MEM_ADD", "MEM_CLEAR",
    # Scientific
    "SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN",
    "LOG", "LN", "EXP", "SQRT", "POWER_2", "POWER_3", "POWER_N",
    "FACTORIAL", "RECIPROCAL", "PI", "EULER",
    # Programmer
    "BIT_AND", "BIT_OR", "BIT_XOR", "BIT_NOT", "SHIFT_LEFT", "SHIFT_RIGHT",
    "HEX_A", "HEX_B", "HEX_C", "HEX_D", "HEX_E", "HEX_F",
    # Constants
    "PHI", "TAU",
    # Meta / control
    "LAYER_NEXT", "LAYER_PREV", "LAYER_CONST", "LAYER_ALG", "LAYER_TRIG",
    "LAYER_VAR", "LAYER_HOME",
    # Display mode
    "TOGGLE_MATH_LAYOUT",
    # Navigation
    "CURSOR_LEFT", "CURSOR_RIGHT", "CURSOR_UP", "CURSOR_DOWN",
    "PAGE_UP", "PAGE_DOWN",
]

# Mapping from key codes to display labels (from C++ keymap.hpp and label_string)
KEY_CODE_TO_LABEL = {
    "NONE": "",
    # Digits
    "DIGIT_0": "0", "DIGIT_1": "1", "DIGIT_2": "2", "DIGIT_3": "3", "DIGIT_4": "4",
    "DIGIT_5": "5", "DIGIT_6": "6", "DIGIT_7": "7", "DIGIT_8": "8", "DIGIT_9": "9",
    # Basic ops
    "ADD": "+", "SUBTRACT": "-", "MULTIPLY": "×", "DIVIDE": "÷", "EQUALS": "=",
    "EVAL": "Eval", "APPROX": "Aprx", "DECIMAL": ".", "CLEAR": "C",
    "ALL_CLEAR": "AC", "BACKSPACE": "BSP",
    # Grouping / extra basic
    "PAREN_OPEN": "(", "PAREN_CLOSE": ")", "PERCENT": "%", "NEGATE": "+/-",
    # Memory
    "MEM_STORE": "M+", "MEM_RECALL": "MR", "MEM_ADD": "M+", "MEM_CLEAR": "MC",
    # Scientific
    "SIN": "sin", "COS": "cos", "TAN": "tan", "ASIN": "asin", "ACOS": "acos", "ATAN": "atan",
    "LOG": "log", "LN": "ln", "EXP": "e^x", "SQRT": "√", "POWER_2": "x²", "POWER_3": "x³", "POWER_N": "x^y",
    "FACTORIAL": "n!", "RECIPROCAL": "1/x", "PI": "π", "EULER": "e",
    # Programmer
    "BIT_AND": "&", "BIT_OR": "|", "BIT_XOR": "^", "BIT_NOT": "~",
    "SHIFT_LEFT": "<<", "SHIFT_RIGHT": ">>",
    "HEX_A": "A", "HEX_B": "B", "HEX_C": "C", "HEX_D": "D", "HEX_E": "E", "HEX_F": "F",
    # Constants
    "PHI": "φ", "TAU": "τ",
    # Meta / control
    "LAYER_NEXT": "LAYER+", "LAYER_PREV": "LAYER-", "LAYER_CONST": "CST",
    "LAYER_ALG": "ALG", "LAYER_TRIG": "TRG", "LAYER_VAR": "Var", "LAYER_HOME": "Home",
    # Display mode
    "TOGGLE_MATH_LAYOUT": "MATH",
    # Navigation
    "CURSOR_LEFT": "←", "CURSOR_RIGHT": "→", "CURSOR_UP": "↑", "CURSOR_DOWN": "↓",
    "PAGE_UP": "PgUp", "PAGE_DOWN": "PgDn",
}


class KeyButton(QPushButton):
    """A button representing a keyboard key with its label and code."""

    def __init__(self, key_data: Dict, parent=None):
        super().__init__(parent)
        self.key_data = key_data
        self.layer_index = key_data.get("layer_index", 0)
        self.position_index = key_data.get("position_index", 0)
        self.index = key_data.get("index", 0)
        self.label = key_data.get("label", "")
        self.code = key_data.get("code", "NONE")
        self.scancode = key_data.get("scancode", "")

        self.update_display()
        self.clicked.connect(self.on_click)

    def update_display(self):
        """Update button appearance based on current label/code."""
        # Always use KEY_CODE_TO_LABEL for display, not the JSON label
        display_text = KEY_CODE_TO_LABEL.get(self.code, "")
        if not display_text or self.code == "NONE":
            display_text = f"{self.index}"
        self.setText(display_text)

        # Style based on whether key is assigned
        if self.code and self.code != "NONE":
            self.setStyleSheet("""
                QPushButton {
                    background-color: #4a6fa5;
                    color: white;
                    border: 2px solid #2c4a7a;
                    border-radius: 4px;
                    font-weight: bold;
                    font-size: 12px;
                }
                QPushButton:hover {
                    background-color: #5a7fb5;
                }
            """)
        else:
            self.setStyleSheet("""
                QPushButton {
                    background-color: #555;
                    color: #aaa;
                    border: 2px solid #333;
                    border-radius: 4px;
                    font-size: 12px;
                }
                QPushButton:hover {
                    background-color: #666;
                }
            """)

    def on_click(self):
        """Handle button click - open code editor dialog."""
        # Get assigned codes from window
        assigned_codes = self.window().get_assigned_codes()

        dialog = KeyEditDialog(self, self.label, self.code, self.scancode, assigned_codes)
        if dialog.exec():
            old_code = self.code
            self.label = dialog.label_text
            self.code = dialog.code_text
            self.scancode = dialog.scancode_text
            self.key_data["label"] = self.label
            self.key_data["code"] = self.code
            self.key_data["scancode"] = self.scancode
            self.update_display()
            self.window().mapping_changed = True
            self.window().update_assigned_codes(old_code, self.code)
            self.window().update_scancode( self.key_data.get("row"),
                                           self.key_data.get("col"),
                                           self.scancode )


class KeyEditDialog(QDialog):
    """Dialog for editing key label and code."""

    def __init__(self, parent, current_label: str, current_code: str, current_scancode: str, assigned_codes: set):
        super().__init__(parent)
        self.setWindowTitle(f"Edit Key {parent.index} (row {parent.key_data.get('row')}, col {parent.key_data.get('col')})")
        self.setFixedSize(500, 310)
        self._recording = False

        layout = QVBoxLayout()

        layout.addWidget(QLabel("Select Key Code:"))
        self.code_input = QComboBox()
        self.code_input.setEditable(True)  # Enable search/filtering

        # Add available codes with labels (excluding currently assigned ones, but include current)
        available = [code for code in AVAILABLE_KEY_CODES if code not in assigned_codes or code == current_code]
        for code in available:
            label = KEY_CODE_TO_LABEL.get(code, code)
            display_text = f"{code} ({label})" if label else code
            self.code_input.addItem(display_text, code)  # Store code as user data

        # Set up completer for search/filtering
        completer = QCompleter([self.code_input.itemText(i) for i in range(self.code_input.count())])
        completer.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        completer.setFilterMode(Qt.MatchFlag.MatchContains)
        self.code_input.setCompleter(completer)

        # Set current selection
        for i in range(self.code_input.count()):
            if self.code_input.itemData(i) == current_code:
                self.code_input.setCurrentIndex(i)
                break

        layout.addWidget(self.code_input)

        # Preview button
        layout.addWidget(QLabel("Preview:"))
        self.preview_btn = QPushButton()
        self.preview_btn.setFixedSize(150, 50)
        self.preview_btn.setEnabled(False)  # Read-only preview
        self.update_preview()
        layout.addWidget(self.preview_btn, 0, Qt.AlignmentFlag.AlignCenter)

        # Connect code change to update preview
        self.code_input.currentIndexChanged.connect(self.update_preview)

        # Scancode field
        layout.addWidget(QLabel("Keyboard Scancode:"))
        scancode_row = QHBoxLayout()
        self.scancode_input = QLineEdit(current_scancode or "")
        self.scancode_input.setPlaceholderText("SDL_SCANCODE_...  or click Record and press a key")
        self.record_btn = QPushButton("Record")
        self.record_btn.setFixedWidth(80)
        self.record_btn.setFixedHeight(30)
        self.record_btn.setCheckable(True)
        self.record_btn.clicked.connect(self.toggle_record)
        scancode_row.addWidget(self.scancode_input)
        scancode_row.addWidget(self.record_btn)
        layout.addLayout(scancode_row)

        buttons = QHBoxLayout()
        ok_btn = QPushButton("OK")
        ok_btn.setFixedHeight(40)
        ok_btn.clicked.connect(self.accept)
        cancel_btn = QPushButton("Cancel")
        cancel_btn.setFixedHeight(40)
        cancel_btn.clicked.connect(self.reject)
        remove_btn = QPushButton("Remove")
        remove_btn.setFixedHeight(40)
        remove_btn.clicked.connect(self.remove_key)
        buttons.addWidget(ok_btn)
        buttons.addWidget(cancel_btn)
        buttons.addWidget(remove_btn)
        layout.addLayout(buttons)

        self.setLayout(layout)

    def update_preview(self):
        """Update the preview button to show the selected key label."""
        code = self.code_input.currentData()
        label = KEY_CODE_TO_LABEL.get(code, "")
        display_text = label if label else code
        if not display_text or display_text == "NONE":
            display_text = "NONE"

        self.preview_btn.setText(display_text)

        # Style based on whether key is assigned
        if code and code != "NONE":
            self.preview_btn.setStyleSheet("""
                QPushButton {
                    background-color: #4a6fa5;
                    color: white;
                    border: 2px solid #2c4a7a;
                    border-radius: 4px;
                    font-weight: bold;
                    font-size: 12px;
                }
            """)
        else:
            self.preview_btn.setStyleSheet("""
                QPushButton {
                    background-color: #555;
                    color: #aaa;
                    border: 2px solid #333;
                    border-radius: 4px;
                    font-size: 12px;
                }
            """)

    def toggle_record(self, checked: bool):
        """Enter or exit key-recording mode."""
        self._recording = checked
        if checked:
            self.record_btn.setText("Listening...")
            self.record_btn.setStyleSheet(
                "QPushButton { background-color: #a03030; color: white; border-radius: 4px; font-weight: bold; }"
            )
            self.scancode_input.setPlaceholderText("Press a key...")
            self.grabKeyboard()
        else:
            self.record_btn.setText("Record")
            self.record_btn.setStyleSheet("")
            self.scancode_input.setPlaceholderText("SDL_SCANCODE_...  or click Record and press a key")
            self.releaseKeyboard()

    def keyPressEvent(self, event: QKeyEvent):
        """Capture a keypress when in recording mode."""
        if self._recording:
            scancode = qt_key_to_sdl_scancode(Qt.Key(event.key()), event.text())
            if scancode:
                self.scancode_input.setText(scancode)
            self.record_btn.setChecked(False)
            self.toggle_record(False)
            event.accept()
            return
        super().keyPressEvent(event)

    def accept(self):
        if self._recording:
            self.toggle_record(False)
        super().accept()

    def reject(self):
        if self._recording:
            self.toggle_record(False)
        super().reject()

    def remove_key(self):
        """Set key to NONE and close dialog."""
        self.code_input.setCurrentIndex(0)  # Assume NONE is first
        self.accept()

    @property
    def label_text(self) -> str:
        """Get the label for the selected code."""
        code = self.code_input.currentData()
        return KEY_CODE_TO_LABEL.get(code, "")

    @property
    def scancode_text(self) -> str:
        """Get the entered scancode string."""
        return self.scancode_input.text().strip()

    @property
    def code_text(self) -> str:
        """Get the selected code."""
        return self.code_input.currentData()


class KeyMapperWindow(QMainWindow):
    """Main window for the key mapper tool."""

    def __init__(self, layout_path: str, keymap_path: str, layers_path: str):
        super().__init__()
        self.layout_path = Path(layout_path)
        self.keymap_path = Path(keymap_path)
        self.layers_path = Path(layers_path)
        self.mapping_changed = False
        self.assigned_codes = set()
        self.current_layer_index = 0

        self.layout_data = self.load_layout()
        self.keymap_data = self.load_or_create_keymap()   # scancodes only
        self.layers_data = self.load_or_create_layers()   # layer assignments

        # Initialize assigned codes from existing mapping
        self.refresh_assigned_codes()

        self.init_ui()
        self.setWindowTitle(f"Key Mapper - {self.layout_data.get('name', 'Unknown')}")
        self.resize(900, 700)

    def refresh_assigned_codes(self):
        """Refresh the assigned codes set from current layer's mapping data."""
        self.assigned_codes.clear()
        layers = self.layers_data.get("layers", [])
        if self.current_layer_index < len(layers):
            for key in layers[self.current_layer_index].get("keys", []):
                code = key.get("code", "NONE")
                if code and code != "NONE":
                    self.assigned_codes.add(code)

    def get_assigned_codes(self) -> set:
        """Get the set of currently assigned key codes for current layer."""
        return self.assigned_codes

    def update_assigned_codes(self, old_code: str, new_code: str):
        """Update assigned codes when a key is changed."""
        if old_code and old_code != "NONE":
            self.assigned_codes.discard(old_code)
        if new_code and new_code != "NONE":
            self.assigned_codes.add(new_code)

    def load_layout(self) -> Dict:
        """Load the VIA layout JSON."""
        if not self.layout_path.exists():
            QMessageBox.critical(self, "Error", f"Layout file not found: {self.layout_path}")
            sys.exit(1)

        with open(self.layout_path, 'r') as f:
            return json.load(f)

    def load_or_create_keymap(self) -> Dict:
        """Load keymap JSON (scancodes only) or create an empty one."""
        if self.keymap_path.exists():
            with open(self.keymap_path, 'r') as f:
                data = json.load(f)
            if "scancodes" not in data:
                data["scancodes"] = {}
            return data
        return {"scancodes": {}}

    def load_or_create_layers(self) -> Dict:
        """Load layers JSON or create default layers from layout."""
        if self.layers_path.exists():
            with open(self.layers_path, 'r') as f:
                return json.load(f)
        layer_names = ["Basic", "TRG", "Constants", "Programmer", "Algebra"]
        positions = self.extract_key_positions()
        return {
            "layers": [
                {"name": name, "keys": [{"code": "NONE"} for _ in positions]}
                for name in layer_names
            ]
        }

    def init_ui(self):
        """Initialize the user interface with layer tabs."""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QVBoxLayout(central_widget)

        # Toolbar
        toolbar = QHBoxLayout()
        save_btn = QPushButton("Save Keymap")
        save_btn.clicked.connect(self.save_keymap)
        toolbar.addWidget(save_btn)

        reload_btn = QPushButton("Reload")
        reload_btn.clicked.connect(self.reload)
        toolbar.addWidget(reload_btn)

        clear_all_btn = QPushButton("Clear All")
        clear_all_btn.clicked.connect(self.clear_all)
        toolbar.addWidget(clear_all_btn)

        toolbar.addStretch()
        main_layout.addLayout(toolbar)

        # Layer tabs
        self.tab_widget = QTabWidget()
        self.layer_widgets = []  # Store scroll content widgets for each layer
        self.key_buttons_per_layer = []  # Store key buttons for each layer

        # Extract key positions from layout (shared across all layers)
        self.key_positions = self.extract_key_positions()

        # Create a tab for each layer
        for layer_idx, layer_data in enumerate(self.layers_data.get("layers", [])):
            scroll = QScrollArea()
            scroll.setWidgetResizable(True)
            scroll_content = QWidget()
            scroll_content.setMinimumSize(600, 400)

            # Build key grid for this layer
            key_buttons = self.build_layer_keys(scroll_content, layer_idx, layer_data)

            scroll.setWidget(scroll_content)
            self.tab_widget.addTab(scroll, layer_data.get("name", f"Layer {layer_idx}"))
            self.layer_widgets.append(scroll_content)
            self.key_buttons_per_layer.append(key_buttons)

        main_layout.addWidget(self.tab_widget)

        # Connect tab change to refresh assigned codes
        self.tab_widget.currentChanged.connect(self.on_tab_changed)

        # Status bar
        self.status_label = QLabel(f"Layers: {len(self.layers_data.get('layers', []))}")
        main_layout.addWidget(self.status_label)

    def on_tab_changed(self, index: int):
        """Handle tab change - refresh assigned codes for the new layer."""
        self.current_layer_index = index
        self.refresh_assigned_codes()

    def update_scancode(self, row: Optional[int], col: Optional[int], scancode: str):
        """Update a scancode entry in keymap_data."""
        if row is None or col is None:
            return
        key_str = f"{row},{col}"
        if "scancodes" not in self.keymap_data:
            self.keymap_data["scancodes"] = {}
        if scancode:
            self.keymap_data["scancodes"][key_str] = scancode
        else:
            self.keymap_data["scancodes"].pop(key_str, None)

    def extract_key_positions(self) -> List[Dict]:
        """Extract key positions from layout JSON, with scancodes from keymap JSON."""
        positions = []
        keymap_layout = self.layout_data.get("layouts", {}).get("keymap", [])
        scancode_map = self.keymap_data.get("scancodes", {})
        index = 0
        cursor_x = 0.0
        cursor_y = 0.0
        pending_w = 1.0
        pending_h = 1.0

        for row in keymap_layout:
            cursor_x = 0.0
            pending_w = 1.0
            pending_h = 1.0

            for item in row:
                if isinstance(item, dict):
                    if "x" in item:
                        cursor_x += item["x"]
                    if "y" in item:
                        cursor_y += item["y"]
                    if "w" in item:
                        pending_w = item["w"]
                    if "h" in item:
                        pending_h = item["h"]
                elif isinstance(item, str):
                    row_col = item.split(',')
                    if len(row_col) == 2:
                        r, c = int(row_col[0]), int(row_col[1])
                        positions.append({
                            "index": index,
                            "row": r,
                            "col": c,
                            "x": cursor_x,
                            "y": cursor_y,
                            "w": pending_w,
                            "h": pending_h,
                            "scancode": scancode_map.get(f"{r},{c}", "")
                        })
                        index += 1
                        cursor_x += pending_w
                        pending_w = 1.0
                        pending_h = 1.0

            cursor_y += 1.0

        return positions

    def build_layer_keys(self, parent_widget, layer_idx: int, layer_data: Dict) -> List[KeyButton]:
        """Build key buttons for a specific layer."""
        key_buttons = []
        layer_keys = layer_data.get("keys", [])
        scancode_map = self.keymap_data.get("scancodes", {})

        # Constants for rendering (match C++ render_layout.cpp)
        KEY_SIZE = 75  # Increased for taller buttons
        PADDING = 6
        MARGIN = 20

        # Build a lookup from (row,col) -> position data for rendering
        pos_by_rc = {(p["row"], p["col"]): p for p in self.key_positions}

        for pos_idx, key_data in enumerate(layer_keys):
                r, c = key_data.get("row"), key_data.get("col")
                pos = pos_by_rc.get((r, c))
                if pos is None:
                    continue
                scancode = scancode_map.get(f"{r},{c}", "")
                full_key_data = {**pos, **key_data,
                                 "layer_index": layer_idx,
                                 "position_index": pos_idx,
                                 "scancode": scancode}

                key_x = pos.get("x", 0.0)
                key_y = pos.get("y", 0.0)
                key_w = pos.get("w", 1.0)
                key_h = pos.get("h", 1.0)

                btn_x = MARGIN + int(key_x * KEY_SIZE)
                btn_y = MARGIN + int(key_y * KEY_SIZE)
                btn_w = int(key_w * KEY_SIZE) - PADDING
                btn_h = int(key_h * KEY_SIZE) - PADDING

                btn = KeyButton(full_key_data)
                btn.setFixedSize(btn_w, btn_h)
                btn.setParent(parent_widget)
                btn.move(btn_x, btn_y)
                btn.show()
                key_buttons.append(btn)

        return key_buttons

    def save_keymap(self):
        """Save scancodes to keymap file and layer assignments to layers file."""
        try:
            # Update layer key data from buttons, matched by row/col
            for layer_idx, key_buttons in enumerate(self.key_buttons_per_layer):
                layer_keys = self.layers_data["layers"][layer_idx]["keys"]
                rc_to_key = {(k.get("row"), k.get("col")): k for k in layer_keys}
                for btn in key_buttons:
                    r, c = btn.key_data.get("row"), btn.key_data.get("col")
                    entry = rc_to_key.get((r, c))
                    if entry is not None:
                        entry["code"] = btn.code

            # Save scancodes (keymap file)
            with open(self.keymap_path, 'w') as f:
                json.dump(self.keymap_data, f, indent=4)

            # Save layer assignments (layers file)
            with open(self.layers_path, 'w') as f:
                json.dump(self.layers_data, f, indent=4)

            self.mapping_changed = False
            self.status_label.setText(f"Saved")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save: {e}")

    def reload(self):
        """Reload the keymap from file."""
        if self.mapping_changed:
            reply = QMessageBox.question(
                self, "Unsaved Changes",
                "You have unsaved changes. Reload anyway?",
                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No
            )
            if reply == QMessageBox.StandardButton.No:
                return

        self.keymap_data = self.load_or_create_keymap()
        self.layers_data = self.load_or_create_layers()
        self.refresh_assigned_codes()
        # Rebuild UI
        self.tab_widget.clear()
        self.layer_widgets.clear()
        self.key_buttons_per_layer.clear()
        self.key_positions = self.extract_key_positions()

        for layer_idx, layer_data in enumerate(self.layers_data.get("layers", [])):
            scroll = QScrollArea()
            scroll.setWidgetResizable(True)
            scroll_content = QWidget()
            scroll_content.setMinimumSize(600, 400)

            key_buttons = self.build_layer_keys(scroll_content, layer_idx, layer_data)

            scroll.setWidget(scroll_content)
            self.tab_widget.addTab(scroll, layer_data.get("name", f"Layer {layer_idx}"))
            self.layer_widgets.append(scroll_content)
            self.key_buttons_per_layer.append(key_buttons)

        self.status_label.setText("Reloaded")

    def clear_all(self):
        """Clear all key codes to NONE across all layers."""
        reply = QMessageBox.question(
            self, "Clear All",
            "This will remove all key code assignments from all layers. Continue?",
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No
        )
        if reply == QMessageBox.StandardButton.No:
            return

        # Clear all codes in layers data
        for layer in self.layers_data.get("layers", []):
            for key in layer.get("keys", []):
                key["code"] = "NONE"

        # Refresh assigned codes
        self.refresh_assigned_codes()

        # Update all button displays
        for key_buttons in self.key_buttons_per_layer:
            for btn in key_buttons:
                btn.code = "NONE"
                btn.key_data["code"] = "NONE"
                btn.update_display()

        self.mapping_changed = True
        self.status_label.setText("Cleared all key codes")

    def clear_layout(self, layout):
        """Recursively clear a layout."""
        while layout.count():
            item = layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
            elif item.layout():
                self.clear_layout(item.layout())

    def closeEvent(self, event):
        """Handle window close - check for unsaved changes."""
        if self.mapping_changed:
            reply = QMessageBox.question(
                self, "Unsaved Changes",
                "You have unsaved changes. Save before exiting?",
                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No | QMessageBox.StandardButton.Cancel
            )
            if reply == QMessageBox.StandardButton.Yes:
                self.save_keymap()
                event.accept()
            elif reply == QMessageBox.StandardButton.No:
                event.accept()
            else:
                event.ignore()
        else:
            event.accept()


def main():
    DEFAULT_LAYOUT_PATH = "data/configs/mf/MF34.json"
    DEFAULT_KEYMAP_PATH = "data/configs/mf/MF34.keymap.json"
    DEFAULT_LAYERS_PATH = "data/configs/mf/MF34.layers.json"

    layout_path = DEFAULT_LAYOUT_PATH
    keymap_path = DEFAULT_KEYMAP_PATH
    layers_path = DEFAULT_LAYERS_PATH

    if len(sys.argv) >= 2:
        layout_path = sys.argv[1]
    if len(sys.argv) >= 3:
        keymap_path = sys.argv[2]
    if len(sys.argv) >= 4:
        layers_path = sys.argv[3]

    app = QApplication(sys.argv)
    window = KeyMapperWindow(layout_path, keymap_path, layers_path)
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
