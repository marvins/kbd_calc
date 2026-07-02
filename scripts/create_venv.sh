#!/usr/bin/env bash
#
# create_venv.sh — Create and configure the project virtual environment
#
# Usage:
#   ./scripts/create_venv.sh                      # Use default python3
#   ./scripts/create_venv.sh --python python3.14  # Specify Python binary
#   ./scripts/create_venv.sh --overwrite          # Recreate existing venv
#   ./scripts/create_venv.sh --no-color           # Disable colored output
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV_DIR="$PROJECT_ROOT/venv"
REQUIREMENTS="$PROJECT_ROOT/requirements.txt"

# Defaults
PYTHON_BIN="python3"
OVERWRITE=false
USE_COLOR=true

#---------------------------------------------------------------------------
# Argument parsing
#---------------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        --python)
            PYTHON_BIN="$2"
            shift 2
            ;;
        --overwrite)
            OVERWRITE=true
            shift
            ;;
        --no-overwrite)
            OVERWRITE=false
            shift
            ;;
        --no-color)
            USE_COLOR=false
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [--python <binary>] [--overwrite] [--no-overwrite] [--no-color]"
            echo ""
            echo "Options:"
            echo "  --python <bin>   Python interpreter to use (default: python3)"
            echo "  --overwrite      Remove and recreate existing venv"
            echo "  --no-overwrite   Abort if venv already exists (default)"
            echo "  --no-color       Disable ANSI color output"
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            echo "Run $0 --help for usage." >&2
            exit 1
            ;;
    esac
done

#---------------------------------------------------------------------------
# Color setup
#---------------------------------------------------------------------------
if [[ "$USE_COLOR" == true ]]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[0;33m'
    BLUE='\033[0;34m'
    NC='\033[0m'
else
    RED=''
    GREEN=''
    YELLOW=''
    BLUE=''
    NC=''
fi

info()  { echo -e "${GREEN}[INFO]${NC} $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }
step()  { echo -e "${BLUE}[STEP]${NC} $*"; }

#---------------------------------------------------------------------------
# Validate Python binary
#---------------------------------------------------------------------------
if ! command -v "$PYTHON_BIN" &>/dev/null; then
    error "Python binary not found: $PYTHON_BIN"
    exit 1
fi

PYTHON_VERSION=$("$PYTHON_BIN" --version 2>&1)
info "Using $PYTHON_VERSION ($PYTHON_BIN)"

#---------------------------------------------------------------------------
# Handle existing venv
#---------------------------------------------------------------------------
if [[ -d "$VENV_DIR" ]]; then
    if [[ "$OVERWRITE" == true ]]; then
        warn "Removing existing venv at $VENV_DIR"
        rm -rf "$VENV_DIR"
    else
        error "Virtual environment already exists at $VENV_DIR"
        echo "  Use --overwrite to recreate, or activate with:"
        echo "    source venv/bin/activate"
        exit 1
    fi
fi

#---------------------------------------------------------------------------
# Create venv
#---------------------------------------------------------------------------
step "Creating virtual environment..."
"$PYTHON_BIN" -m venv "$VENV_DIR"
info "Created venv at $VENV_DIR"

#---------------------------------------------------------------------------
# Upgrade pip
#---------------------------------------------------------------------------
step "Upgrading pip..."
"$VENV_DIR/bin/pip" install --upgrade pip --quiet
info "pip upgraded to $("$VENV_DIR/bin/pip" --version | awk '{print $2}')"

#---------------------------------------------------------------------------
# Install requirements
#---------------------------------------------------------------------------
if [[ -f "$REQUIREMENTS" ]]; then
    step "Installing dependencies from requirements.txt..."
    "$VENV_DIR/bin/pip" install -r "$REQUIREMENTS" --quiet
    info "Dependencies installed."
else
    warn "No requirements.txt found at $REQUIREMENTS — skipping dependency install."
fi

#---------------------------------------------------------------------------
# Done
#---------------------------------------------------------------------------
echo ""
info "Virtual environment ready. Activate with:"
echo "  source venv/bin/activate"
