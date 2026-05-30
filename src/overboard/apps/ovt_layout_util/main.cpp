/**
 * @file    main.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Command-line tool to render VIA keyboard layout as PNG
 */

// C++ Standard Libraries
#include <filesystem>
#include <string>
#include <string_view>

// Project Libraries
#include <overboard/apps/ovt_layout_util/config.hpp>
#include <overboard/io/via_layout.hpp>
#include <overboard/log/stdout_logger.hpp>

// Third-Party Libraries
#include <png.h>

using namespace ovb;

/**
 * @brief Render layout to PNG
 */
void render_to_png(const io::Via_Layout& layout, const std::string& output_path, log::I_Logger& logger) {
    constexpr int KEY_SIZE = 60;  // pixels per key unit
    constexpr int PADDING = 6;     // pixels between keys
    constexpr int MARGIN = 20;     // pixels around the layout

    // Calculate bounds from VIA coordinates
    auto bounds = io::calculate_bounds(layout);
    int img_w = static_cast<int>(bounds.x) * KEY_SIZE + MARGIN * 2;
    int img_h = static_cast<int>(bounds.y) * KEY_SIZE + MARGIN * 2;

    // Create image buffer (RGB)
    std::vector<uint8_t> img(static_cast<size_t>(img_w) * static_cast<size_t>(img_h) * 3, 240); // Light gray background

    // Draw keys using VIA coordinates directly
    for (const auto& key : layout.keys) {
        int x = static_cast<int>(key.x) * KEY_SIZE + MARGIN;
        int y = static_cast<int>(key.y) * KEY_SIZE + MARGIN;
        int w = static_cast<int>(key.w) * KEY_SIZE - PADDING;
        int h = static_cast<int>(key.h) * KEY_SIZE - PADDING;

        // Log first few rectangles for debugging
        static int key_count = 0;
        if (key_count < 5) {
            logger.info("Key rect: x=" + std::to_string(x) + ", y=" + std::to_string(y) +
                       ", w=" + std::to_string(w) + ", h=" + std::to_string(h));
            key_count++;
        }

        // Draw key rectangle (dark gray)
        for (int py = y; py < y + h; ++py) {
            for (int px = x; px < x + w; ++px) {
                if (px >= 0 && px < img_w && py >= 0 && py < img_h) {
                    size_t idx = (static_cast<size_t>(py) * static_cast<size_t>(img_w) + static_cast<size_t>(px)) * 3;
                    img[idx + 0] = 60;  // R
                    img[idx + 1] = 60;  // G
                    img[idx + 2] = 80;  // B
                }
            }
        }

        // Draw border
        for (int py = y; py < y + h; ++py) {
            for (int px = x; px < x + w; ++px) {
                if (px >= 0 && px < img_w && py >= 0 && py < img_h) {
                    if (px == x || px == x + w - 1 || py == y || py == y + h - 1) {
                        size_t idx = (static_cast<size_t>(py) * static_cast<size_t>(img_w) + static_cast<size_t>(px)) * 3;
                        img[idx + 0] = 100;
                        img[idx + 1] = 100;
                        img[idx + 2] = 130;
                    }
                }
            }
        }
    }

    // Write PNG using libpng
    FILE* fp = fopen(output_path.c_str(), "wb");
    if (!fp) {
        throw std::runtime_error("Failed to open file for writing: " + output_path);
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        throw std::runtime_error("Failed to create PNG write struct");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Failed to create PNG info struct");
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        throw std::runtime_error("PNG write error");
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, static_cast<png_uint_32>(img_w), static_cast<png_uint_32>(img_h), 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    std::vector<png_bytep> row_pointers(static_cast<size_t>(img_h));
    for (int y = 0; y < img_h; ++y) {
        row_pointers[static_cast<size_t>(y)] = img.data() + static_cast<size_t>(y) * static_cast<size_t>(img_w) * 3;
    }

    png_set_rows(png_ptr, info_ptr, row_pointers.data());
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

/**
 * @brief Main function to render VIA keyboard layout as PNG
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit code
 */
int main(int argc, char* argv[]) {

    // Parse the command line
    auto config_opt = ovb::apps::lutil::Config::parse(argc, argv);

    if (!config_opt) {
        return 1;
    }

    const auto& config = *config_opt;

    if (config.help_requested()) {
        ovb::apps::lutil::Config::print_usage(argv[0]);
        return 0;
    }

    // Configure the logger
    log::Stdout_Logger logger(config.log_level());

    if (config.config_folder().empty() || config.output_path().empty()) {
        logger.error("Missing required arguments");
        ovb::apps::lutil::Config::print_usage(argv[0]);
        return 1;
    }

    try {
        // Build path to main.json
        std::filesystem::path input_path = config.config_folder() / "main.json";

        logger.info("Parsing: " + input_path.string());
        io::Via_Layout via_layout = io::parse_via_layout(input_path);

        logger.info("Keyboard: " + via_layout.name + "\n" +
                    "Matrix: " + std::to_string(via_layout.matrix_rows) + "x" + std::to_string(via_layout.matrix_cols) + "\n" +
                    "Keys: " + std::to_string(via_layout.keys.size()));

        render_to_png(via_layout, config.output_path().string(), logger);

        logger.info("Rendered to: " + config.output_path().string());
        return 0;
    } catch (const std::exception& e) {
        logger.error(std::string("Error: ") + e.what());
        return 1;
    }
}
