#!/usr/bin/env python3
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
try:
    from PyQt6.QtWidgets import (
        QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
        QPushButton, QLabel, QLineEdit, QFileDialog, QMessageBox, QScrollArea
    )
    from PyQt6.QtCore import Qt, QSize
    from PyQt6.QtGui import QFont
except ImportError:
    print("PyQt6 not found. Install with: pip install PyQt6")
    sys.exit(1)


class KeyButton(QPushButton):
    """A button representing a keyboard key with its label and code."""

    def __init__(self, key_data: Dict, parent=None):
        super().__init__(parent)
        self.key_data = key_data
        self.index = key_data.get("index", 0)
        self.matrix = key_data.get("matrix", "")
        self.label = key_data.get("label", "")
        self.code = key_data.get("code", "NONE")

        self.setFixedSize(60, 60)
        self.update_display()
        self.clicked.connect(self.on_click)

    def update_display(self):
        """Update button appearance based on current label/code."""
        display_text = self.label if self.label else self.code
        if not display_text or display_text == "NONE":
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
                }
                QPushButton:hover {
                    background-color: #666;
                }
            """)

    def on_click(self):
        """Handle button click - open code editor dialog."""
        dialog = KeyEditDialog(self, self.label, self.code)
        if dialog.exec():
            self.label = dialog.label_text
            self.code = dialog.code_text
            self.key_data["label"] = self.label
            self.key_data["code"] = self.code
            self.update_display()
            self.window().mapping_changed = True


class KeyEditDialog(QLabel):
    """Dialog for editing key label and code."""

    def __init__(self, parent, current_label: str, current_code: str):
        super().__init__(parent)
        self.setWindowTitle(f"Edit Key {parent.index} ({parent.matrix})")
        self.setFixedSize(300, 150)

        layout = QVBoxLayout()

        layout.addWidget(QLabel("Label (display text):"))
        self.label_input = QLineEdit(current_label)
        layout.addWidget(self.label_input)

        layout.addWidget(QLabel("Code (internal name):"))
        self.code_input = QLineEdit(current_code)
        layout.addWidget(self.code_input)

        buttons = QHBoxLayout()
        ok_btn = QPushButton("OK")
        ok_btn.clicked.connect(self.accept)
        cancel_btn = QPushButton("Cancel")
        cancel_btn.clicked.connect(self.reject)
        buttons.addWidget(ok_btn)
        buttons.addWidget(cancel_btn)
        layout.addLayout(buttons)

        self.setLayout(layout)

    @property
    def label_text(self) -> str:
        return self.label_input.text()

    @property
    def code_text(self) -> str:
        return self.code_input.text()


class KeyMapperWindow(QMainWindow):
    """Main window for the key mapper tool."""

    def __init__(self, layout_path: str, mapping_path: str):
        super().__init__()
        self.layout_path = Path(layout_path)
        self.mapping_path = Path(mapping_path)
        self.mapping_changed = False

        self.layout_data = self.load_layout()
        self.mapping_data = self.load_or_create_mapping()

        self.init_ui()
        self.setWindowTitle(f"Key Mapper - {self.layout_data.get('name', 'Unknown')}")
        self.resize(800, 600)

    def load_layout(self) -> Dict:
        """Load the VIA layout JSON."""
        if not self.layout_path.exists():
            QMessageBox.critical(self, "Error", f"Layout file not found: {self.layout_path}")
            sys.exit(1)

        with open(self.layout_path, 'r') as f:
            return json.load(f)

    def load_or_create_mapping(self) -> Dict:
        """Load mapping JSON or create it from layout."""
        if self.mapping_path.exists():
            with open(self.mapping_path, 'r') as f:
                return json.load(f)
        else:
            # Create mapping from layout with full position data
            mapping = {
                "name": f"{self.layout_data.get('name', 'Unknown')} Key Mapping",
                "layout_file": self.layout_path.name,
                "keys": []
            }

            # Extract keys from layout with position tracking
            keymap = self.layout_data.get("layouts", {}).get("keymap", [])
            index = 0
            cursor_x = 0.0
            cursor_y = 0.0
            pending_w = 1.0
            pending_h = 1.0

            for row in keymap:
                cursor_x = 0.0
                pending_w = 1.0
                pending_h = 1.0

                for item in row:
                    if isinstance(item, dict):
                        # Positioning object
                        if "x" in item:
                            cursor_x += item["x"]
                        if "y" in item:
                            cursor_y += item["y"]
                        if "w" in item:
                            pending_w = item["w"]
                        if "h" in item:
                            pending_h = item["h"]
                    elif isinstance(item, str):
                        # Key definition "row,col"
                        row_col = item.split(',')
                        if len(row_col) == 2:
                            mapping["keys"].append({
                                "index": index,
                                "row": int(row_col[0]),
                                "col": int(row_col[1]),
                                "x": cursor_x,
                                "y": cursor_y,
                                "w": pending_w,
                                "h": pending_h,
                                "label": "",
                                "code": "NONE"
                            })
                            index += 1
                            cursor_x += pending_w
                            pending_w = 1.0
                            pending_h = 1.0

                cursor_y += 1.0

            return mapping

    def init_ui(self):
        """Initialize the user interface."""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QVBoxLayout(central_widget)

        # Toolbar
        toolbar = QHBoxLayout()
        save_btn = QPushButton("Save Mapping")
        save_btn.clicked.connect(self.save_mapping)
        toolbar.addWidget(save_btn)

        reload_btn = QPushButton("Reload")
        reload_btn.clicked.connect(self.reload)
        toolbar.addWidget(reload_btn)

        toolbar.addStretch()
        main_layout.addLayout(toolbar)

        # Key grid (scrollable)
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll_content = QWidget()
        self.grid_layout = QVBoxLayout(scroll_content)

        # Build key grid from layout
        self.key_buttons: List[KeyButton] = []
        keymap = self.layout_data.get("layouts", {}).get("keymap", [])

        # Group keys by row for display
        row_widgets = []
        for row in keymap:
            row_layout = QHBoxLayout()
            row_layout.addStretch()

            for item in row:
                if isinstance(item, str):
                    # Find corresponding mapping by row/col
                    row_col = item.split(',')
                    if len(row_col) == 2:
                        key_row = int(row_col[0])
                        key_col = int(row_col[1])

                        key_mapping = None
                        for k in self.mapping_data.get("keys", []):
                            if k.get("row") == key_row and k.get("col") == key_col:
                                key_mapping = k
                                break

                        if key_mapping:
                            btn = KeyButton(key_mapping)
                            row_layout.addWidget(btn)
                            self.key_buttons.append(btn)
                elif isinstance(item, dict):
                    # Positioning object - add spacing
                    if "x" in item:
                        spacing = int(item["x"] * 60)
                        row_layout.addSpacing(spacing)

            row_layout.addStretch()
            self.grid_layout.addLayout(row_layout)

        self.grid_layout.addStretch()
        scroll.setWidget(scroll_content)
        main_layout.addWidget(scroll)

        # Status bar
        self.status_label = QLabel(f"Keys: {len(self.key_buttons)}")
        main_layout.addWidget(self.status_label)

    def save_mapping(self):
        """Save the current mapping to JSON."""
        try:
            with open(self.mapping_path, 'w') as f:
                json.dump(self.mapping_data, f, indent=2)
            self.mapping_changed = False
            self.status_label.setText(f"Saved to {self.mapping_path}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save: {e}")

    def reload(self):
        """Reload the mapping from file."""
        if self.mapping_changed:
            reply = QMessageBox.question(
                self, "Unsaved Changes",
                "You have unsaved changes. Reload anyway?",
                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No
            )
            if reply == QMessageBox.StandardButton.No:
                return

        self.mapping_data = self.load_or_create_mapping()
        # Rebuild UI
        for i in range(self.grid_layout.count()):
            item = self.grid_layout.takeAt(i)
            if item.widget():
                item.widget().deleteLater()
            elif item.layout():
                self.clear_layout(item.layout())

        self.key_buttons.clear()
        self.init_ui()
        self.status_label.setText("Reloaded")

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
                self.save_mapping()
                event.accept()
            elif reply == QMessageBox.StandardButton.No:
                event.accept()
            else:
                event.ignore()
        else:
            event.accept()


def main():
    if len(sys.argv) < 3:
        print("Usage: key_mapper.py <layout.json> <mapping.json>")
        print("Example: key_mapper.py data/MF34.json data/MF34.mapping.json")
        sys.exit(1)

    layout_path = sys.argv[1]
    mapping_path = sys.argv[2]

    app = QApplication(sys.argv)
    window = KeyMapperWindow(layout_path, mapping_path)
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
