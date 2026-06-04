#!/usr/bin/env python3
"""
Validate keyboard.json configuration files against the JSON schema.

Usage:
    python scripts/validate_keyboard_config.py <path_to_keyboard.json>

This script validates a keyboard.json file against the schema defined in
data/schemas/keyboard.json.schema. It reports any validation errors found.
"""

import argparse
import json
import sys
from pathlib import Path

try:
    import jsonschema
except ImportError:
    print("Error: jsonschema library is required.")
    print("Install it with: pip install jsonschema")
    sys.exit(1)


def load_schema(schema_path: Path) -> dict:
    """Load the JSON schema file."""
    try:
        with open(schema_path, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: Schema file not found: {schema_path}")
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in schema file: {e}")
        sys.exit(1)


def load_config(config_path: Path) -> dict:
    """Load the keyboard configuration file."""
    try:
        with open(config_path, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: Config file not found: {config_path}")
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in config file: {e}")
        sys.exit(1)


def validate_config(config: dict, schema: dict) -> bool:
    """Validate the config against the schema."""
    try:
        jsonschema.validate(instance=config, schema=schema)
        return True
    except jsonschema.ValidationError as e:
        print(f"Validation error: {e.message}")
        print(f"  Path: {' -> '.join(str(p) for p in e.path) if e.path else 'root'}")
        print(f"  Schema path: {' -> '.join(str(p) for p in e.schema_path) if e.schema_path else 'root'}")
        if e.instance:
            print(f"  Invalid value: {e.instance}")
        return False


def main():
    parser = argparse.ArgumentParser(
        description="Validate keyboard.json configuration files"
    )
    parser.add_argument(
        "config_file",
        type=Path,
        help="Path to keyboard.json file to validate"
    )
    parser.add_argument(
        "--schema",
        type=Path,
        default=Path("data/schemas/keyboard.json.schema"),
        help="Path to JSON schema file (default: data/schemas/keyboard.json.schema)"
    )
    args = parser.parse_args()

    # Resolve paths relative to script location if not absolute
    script_dir = Path(__file__).parent.parent
    schema_path = args.schema if args.schema.is_absolute() else script_dir / args.schema
    config_path = args.config_file if args.config_file.is_absolute() else script_dir / args.config_file

    print(f"Validating: {config_path}")
    print(f"Using schema: {schema_path}")
    print()

    schema = load_schema(schema_path)
    config = load_config(config_path)

    if validate_config(config, schema):
        print("✓ Configuration is valid")
        print(f"  Keyboard: {config.get('name', 'Unknown')}")
        print(f"  Keys: {len(config.get('keys', {}))}")
        print(f"  Layers: {len(config.get('layers', []))}")
        return 0
    else:
        print("✗ Configuration is invalid")
        return 1


if __name__ == "__main__":
    sys.exit(main())
