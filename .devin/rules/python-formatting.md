---
description: Python formatting guidelines
trigger: model_decision
---

# Python Formatting Guidelines

When editing Python files, follow these rules:

## requirements.txt

- Package names in `requirements.txt` should be **alphabetically sorted**
- This makes it easier to maintain and review dependencies
- Example:
  ```
  # Python dependencies for project tools
  jsonschema>=4.0.0
  PyQt6>=6.0.0
  ```

## Python Scripts

- Follow PEP 8 style guidelines
- Use 4 spaces for indentation
- Maximum line length: 100 characters
- Use type hints where appropriate
- Add docstrings to functions and classes
