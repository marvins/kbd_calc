/**
 * @file   keymap.cpp
 * @author Marvin
 * @date   2025-10-18
 *
 * @brief  Keymap implementation
 */

// C++ Standard Libraries
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

// Third-Party Libraries
#include <nlohmann/json.hpp>

// Project Libraries
#include <overboard/core/keymap.hpp>

namespace ovb::core {

/***************************/
/*       Constructor       */
/***************************/
Keymap::Keymap(const std::array<Layer, LAYER_COUNT>& layers) {
    m_layers = layers;
}

/***************************/
/*         Getters         */
/***************************/
const Layer& Keymap::get_layer(std::size_t index) const {
    return m_layers[index];
}

Action_Code Keymap::get_action(std::size_t layer, std::size_t key_index) const {
    return m_layers[layer].keys[key_index];
}

/****************************************/
/*      Load Layers from JSON File      */
/****************************************/
std::array<Layer, LAYER_COUNT> load_layers_from_json(
    const std::filesystem::path& json_path,
    const std::map<std::pair<int, int>, int>& matrix_index_map)
{
    std::ifstream file(json_path);
    if (!file.is_open()) {
        std::cerr << "[keymap] Failed to open keymap JSON file: " << json_path.string() << "\n";
        return {};
    }

    nlohmann::json j;
#ifdef __cpp_exceptions
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error& ex) {
        std::cerr << "[keymap] Failed to parse keymap JSON: " << ex.what() << "\n";
        return {};
    }
#else
    file >> j;
#endif

    if (!j.contains("layers") || !j["layers"].is_array()) {
        std::cerr << "[keymap] Invalid keymap JSON: missing or invalid 'layers' array\n";
        return {};
    }

    auto layers_json = j["layers"];
    if (layers_json.size() != LAYER_COUNT) {
        std::cerr << "[keymap] Invalid keymap JSON: expected " << LAYER_COUNT
                  << " layers, got " << layers_json.size() << "\n";
        return {};
    }

    std::array<Layer, LAYER_COUNT> layers;

    for (size_t i = 0; i < layers_json.size(); ++i) {
        auto layer_json = layers_json[i];

        if (!layer_json.contains("name") || !layer_json["name"].is_string()) {
            std::cerr << "[keymap] Invalid layer at index " << i << ": missing or invalid 'name'\n";
            return {};
        }

        if (!layer_json.contains("keys") || !layer_json["keys"].is_array()) {
            std::cerr << "[keymap] Invalid layer at index " << i << ": missing or invalid 'keys' array\n";
            return {};
        }

        auto keys_json = layer_json["keys"];

        // Set layer name from JSON
        layers[i].name = layer_json["name"].get<std::string>();

        // Build keys from JSON using matrix position -> visual index mapping
        // Resize to match the number of keys in the layout (matrix_index_map size)
        layers[i].keys.resize(matrix_index_map.size(), Action_Code::NONE);
        layers[i].labels.resize(matrix_index_map.size(), "");

        for (const auto& key_json : keys_json) {
            int row = 0;
            int col = 0;
            if (key_json.contains("row") && key_json["row"].is_number()) {
                row = key_json["row"].get<int>();
            }
            if (key_json.contains("col") && key_json["col"].is_number()) {
                col = key_json["col"].get<int>();
            }

            Action_Code code = Action_Code::NONE;
            if (key_json.contains("code") && key_json["code"].is_string()) {
                code = string_to_action_code(key_json["code"].get<std::string>());
            }

            std::string label;
            if (key_json.contains("label") && key_json["label"].is_string()) {
                label = key_json["label"].get<std::string>();
            }

            // Map matrix position to visual key index
            auto it = matrix_index_map.find({row, col});
            if (it != matrix_index_map.end()) {
                int key_index = it->second;
                if (key_index >= 0 && key_index < static_cast<int>(layers[i].keys.size())) {
                    layers[i].keys[static_cast<std::size_t>(key_index)]   = code;
                    layers[i].labels[static_cast<std::size_t>(key_index)] = label;
                }
            }
        }
    }

    return layers;
}

} // namespace ovb::core
