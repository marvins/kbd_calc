# CMake script to embed a JSON file as a C++ byte array
# Usage: cmake -DINPUT_FILE=path/to/file.json -DOUTPUT_HEADER=path/to/embedded.hpp -P embed_json.cmake

file(READ "${INPUT_FILE}" JSON_CONTENT)

# Convert to hex escape sequence for C++ string literal
string(HEX "${JSON_CONTENT}" JSON_HEX)

# Create the header file
file(WRITE "${OUTPUT_HEADER}"
"/**
 * @file    ${CMAKE_CURRENT_BINARY_DIR}/embedded.hpp
 * @brief   Embedded JSON resource (auto-generated)
 *
 * This file was auto-generated from ${INPUT_FILE}
 * Do not edit manually.
 */
#pragma once

#include <cstdint>
#include <cstddef>

namespace ovb::resources {

inline constexpr std::uint8_t embedded_json_data[] = {${JSON_HEX}};

inline constexpr std::size_t embedded_json_size = sizeof(embedded_json_data);

} // namespace ovb::resources
"
)
