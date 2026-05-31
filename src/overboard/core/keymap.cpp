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
        throw std::runtime_error("Failed to open keymap JSON file: " + json_path.string());
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse keymap JSON: " + std::string(e.what()));
    }

    if (!j.contains("layers") || !j["layers"].is_array()) {
        throw std::runtime_error("Invalid keymap JSON: missing or invalid 'layers' array");
    }

    auto layers_json = j["layers"];
    if (layers_json.size() != LAYER_COUNT) {
        throw std::runtime_error("Invalid keymap JSON: expected " + std::to_string(LAYER_COUNT) +
                                 " layers, got " + std::to_string(layers_json.size()));
    }

    std::array<Layer, LAYER_COUNT> layers;

    for (size_t i = 0; i < layers_json.size(); ++i) {
        auto layer_json = layers_json[i];

        if (!layer_json.contains("name") || !layer_json["name"].is_string()) {
            throw std::runtime_error("Invalid layer at index " + std::to_string(i) + ": missing or invalid 'name'");
        }

        if (!layer_json.contains("keys") || !layer_json["keys"].is_array()) {
            throw std::runtime_error("Invalid layer at index " + std::to_string(i) + ": missing or invalid 'keys' array");
        }

        auto keys_json = layer_json["keys"];

        // Set layer name from JSON
        layers[i].name = layer_json["name"].get<std::string>();

        // Build keys from JSON using matrix position -> visual index mapping
        // Resize to match the number of keys in the layout (matrix_index_map size)
        layers[i].keys.resize(matrix_index_map.size(), Action_Code::NONE);

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

            // Map matrix position to visual key index
            auto it = matrix_index_map.find({row, col});
            if (it != matrix_index_map.end()) {
                int key_index = it->second;
                if (key_index >= 0 && key_index < static_cast<int>(layers[i].keys.size())) {
                    layers[i].keys[static_cast<std::size_t>(key_index)] = code;
                }
            }
        }
    }

    return layers;
}

} // namespace ovb::core
