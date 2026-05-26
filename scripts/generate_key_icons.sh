#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
OUTPUT_DIR="${PROJECT_DIR}/output"

# Create output directory
mkdir -p "${OUTPUT_DIR}"

# Build the tool first
echo "Building test_math_render..."
cd "${PROJECT_DIR}"
./scripts/build.sh -j8 > /dev/null 2>&1

# Font file (adjust path as needed)
FONT_FILE="/System/Library/Fonts/Helvetica.ttc"

# Define expressions to render
declare -a EXPRESSIONS=(
    "x^2:power_2.png"
    "x^3:power_3.png"
    "x^n:power_n.png"
    "sqrt(x):sqrt.png"
    "pi:pi.png"
    "phi:phi.png"
    "tau:tau.png"
    "e:e.png"
    "1/2:fraction.png"
    "2*3:multiply.png"
    "2-3:subtract.png"
    "sin(x):sin.png"
    "cos(x):cos.png"
    "tan(x):tan.png"
    "log(x):log.png"
    "ln(x):ln.png"
)

# Render each expression
for expr in "${EXPRESSIONS[@]}"; do
    IFS=':' read -r math_expr output_file <<< "$expr"
    echo "Rendering: $math_expr -> $output_file"
    "${BUILD_DIR}/test_math_render" "$FONT_FILE" "$math_expr" "${OUTPUT_DIR}/${output_file}"
done

echo ""
echo "Icons generated in: ${OUTPUT_DIR}"
