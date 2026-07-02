#!/usr/bin/env bash
#
# build_docs.sh — Build project documentation
#
# Usage:
#   ./scripts/build_docs.sh                  # Full build (Doxygen + Jupyter Book)
#   ./scripts/build_docs.sh --local          # Build + start local preview server
#   ./scripts/build_docs.sh --doxygen-only   # Only regenerate Doxygen API docs
#   ./scripts/build_docs.sh --serve          # Full build + start preview server
#   ./scripts/build_docs.sh --clean|-c       # Remove all build artifacts
#   ./scripts/build_docs.sh -l|--log <path>  # Write output to log file
#
set -euo pipefail

# Handle -l/--log BEFORE any output
# Parse args to find log file path
log_path=""
prev=""
for arg in "$@"; do
    if [[ "$prev" == "-l" ]] || [[ "$prev" == "--log" ]]; then
        log_path="$arg"
        break
    fi
    prev="$arg"
done

if [[ -n "$log_path" ]]; then
    mkdir -p "$(dirname "$log_path")"
    echo "Logging to: $log_path" >&2
    exec > "$log_path" 2>&1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV_DIR="$PROJECT_ROOT/venv"
DOCS_DIR="$PROJECT_ROOT/docs"
BUILD_DIR="$DOCS_DIR/_build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

info()  { echo -e "${GREEN}[INFO]${NC} $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }

#---------------------------------------------------------------------------
# Check prerequisites
#---------------------------------------------------------------------------
check_prerequisites() {
    # Check venv
    if [[ ! -d "$VENV_DIR" ]]; then
        error "Virtual environment not found at $VENV_DIR"
        echo "  Create one with: python3 -m venv venv && source venv/bin/activate && pip install -r requirements.txt"
        exit 1
    fi

    # Check doxygen
    if ! command -v doxygen &>/dev/null; then
        error "doxygen not found. Install with: brew install doxygen"
        exit 1
    fi

    # Check jupyter-book
    if ! "$VENV_DIR/bin/jupyter-book" --version &>/dev/null; then
        error "jupyter-book not installed in venv. Run: source venv/bin/activate && pip install -r requirements.txt"
        exit 1
    fi
}

#---------------------------------------------------------------------------
# Build Doxygen API docs
#---------------------------------------------------------------------------
build_doxygen() {
    info "Building Doxygen API documentation..."

    # Inject version from VERSION file into Doxyfile
    local version_file="$PROJECT_ROOT/VERSION"
    if [[ -f "$version_file" ]]; then
        local version=$(cat "$version_file" | tr -d '[:space:]')
        info "Setting PROJECT_NUMBER to $version"
        if [[ "$OSTYPE" == "darwin"* ]]; then
            sed -i '' "s/^PROJECT_NUMBER.*/PROJECT_NUMBER = $version/" "$DOCS_DIR/Doxyfile"
        else
            sed -i "s/^PROJECT_NUMBER.*/PROJECT_NUMBER = $version/" "$DOCS_DIR/Doxyfile"
        fi
    else
        warn "VERSION file not found, leaving PROJECT_NUMBER empty"
    fi

    # Set PlantUML JAR path based on platform
    local plantuml_jar
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS (Homebrew) - PlantUML is installed as a binary, not JAR
        # Doxygen can use the binary directly
        if command -v plantuml &>/dev/null; then
            plantuml_jar=$(command -v plantuml)
        elif [[ -d "/opt/homebrew" ]]; then
            plantuml_jar="/opt/homebrew/bin/plantuml"
        else
            plantuml_jar="/usr/local/bin/plantuml"
        fi
    else
        # Linux (Debian/Ubuntu) - JAR is installed via apt-get
        plantuml_jar="/usr/share/plantuml/plantuml.jar"
    fi

    if [[ -f "$plantuml_jar" ]]; then
        info "Setting PLANTUML_JAR_PATH to $plantuml_jar"
        if [[ "$OSTYPE" == "darwin"* ]]; then
            sed -i '' "s|^PLANTUML_JAR_PATH.*|PLANTUML_JAR_PATH      = $plantuml_jar|" "$DOCS_DIR/Doxyfile"
        else
            sed -i "s|^PLANTUML_JAR_PATH.*|PLANTUML_JAR_PATH      = $plantuml_jar|" "$DOCS_DIR/Doxyfile"
        fi
    else
        warn "PlantUML not found at $plantuml_jar — PlantUML diagrams will be skipped"
    fi

    mkdir -p "$BUILD_DIR/api"
    cd "$DOCS_DIR"
    doxygen Doxyfile
    cd "$PROJECT_ROOT"
    info "Doxygen output: $BUILD_DIR/api/html/index.html"
}

#---------------------------------------------------------------------------
# Build Jupyter Book (narrative docs)
#---------------------------------------------------------------------------
build_jupyter_book() {
    info "Building Jupyter Book documentation..."
    cd "$DOCS_DIR"
    "$VENV_DIR/bin/jupyter-book" build
    info "Jupyter Book output: $BUILD_DIR/"
}

#---------------------------------------------------------------------------
# Start local preview server
#---------------------------------------------------------------------------
start_preview() {
    info "Starting local preview server..."
    cd "$DOCS_DIR"
    "$VENV_DIR/bin/jupyter-book" start
}

#---------------------------------------------------------------------------
# Clean build artifacts
#---------------------------------------------------------------------------
clean() {
    info "Cleaning documentation build artifacts..."
    rm -rf "$BUILD_DIR"
    rm -rf "$DOCS_DIR/_api"
    info "Clean complete."
}

#---------------------------------------------------------------------------
# Main
#---------------------------------------------------------------------------
main() {
    local mode="full"
    local clean_first=false

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -l|--log)
                shift 2
                ;;
            --clean|-c)
                clean_first=true
                shift
                ;;
            --doxygen-only|--local|--serve)
                mode="$1"
                shift
                ;;
            *)
                if [[ "$mode" == "full" ]]; then
                    mode="$1"
                fi
                shift
                ;;
        esac
    done

    # Clean first if requested
    if [[ "$clean_first" == true ]]; then
        info "Clean flag detected - running clean before build..."
        clean
    fi

    case "$mode" in
        --clean|-c)
            # Already handled above
            ;;
        --doxygen-only)
            check_prerequisites
            build_doxygen
            ;;
        --local)
            check_prerequisites
            build_doxygen
            start_preview
            ;;
        --serve)
            check_prerequisites
            build_doxygen
            build_jupyter_book
            info "Documentation build complete. Starting preview server..."
            start_preview
            ;;
        full|"")
            check_prerequisites
            build_doxygen
            build_jupyter_book
            info "Documentation build complete."
            info "  Narrative docs: $BUILD_DIR/"
            info "  API reference:  $BUILD_DIR/api/html/index.html"
            ;;
        *)
            echo "Usage: $0 [--local|--doxygen-only|--serve|--clean|-c] [-l|--log <path>]"
            exit 1
            ;;
    esac
}

main "$@"
