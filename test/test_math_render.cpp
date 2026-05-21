/**
 * @file    test_math_render.cpp
 * @author  Marvin Smith
 * @date    10/20/2025
 *
 * @brief Test suite for the Math Render functionality.
 */

// C++ Standard Libraries
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Third-Party Libraries
#include <png.h>
#include <SDL2/SDL.h>

// Project Libraries
#include <overboard/math/ast/ast.hpp>
#include <overboard/math/layout/box.hpp>
#include <overboard/math/layout/engine.hpp>
#include <overboard/math/parser.hpp>
#include <overboard/hal/font_5x7.hpp>

using namespace ovb;

// ─── Constants ───────────────────────────────────────────────────────────────

// SDL RGBA masks for 32-bit surface (R=0x00FF0000, G=0x0000FF00, B=0x000000FF, A=0xFF000000)
static constexpr uint32_t SDL_MASK_R = 0x00FF0000;
static constexpr uint32_t SDL_MASK_G = 0x0000FF00;
static constexpr uint32_t SDL_MASK_B = 0x000000FF;
static constexpr uint32_t SDL_MASK_A = 0xFF000000;

// Default render dimensions
static constexpr int RENDER_WIDTH  = 480;
static constexpr int RENDER_HEIGHT = 240;
static constexpr int RENDER_SCALE  = 3;

// Default colors
static constexpr uint8_t COLOR_BLACK = 0;
static constexpr uint8_t COLOR_WHITE = 255;

// ─── Functions ─────────────────────────────────────────────────────────────────

/****************************/
/*      Character Index     */
/****************************/
static int char_index(char c) {
    if (c >= ' ' && c <= 'z') return c - ' ';
    return -1;
}

/****************************/
/*       Draw Glyph         */
/****************************/
static void draw_glyph( SDL_Surface*       surf,
                        core::Point<int>   pos,
                        char               c,
                        uint32_t           color,
                        int                scale ) {
    int idx = char_index(c);
    if (idx < 0) return;

    uint32_t* pixels = static_cast<uint32_t*>(surf->pixels);
    int pitch = static_cast<int>(surf->pitch) / static_cast<int>(sizeof(uint32_t));

    for (int row = 0; row < 7; ++row) {
        uint8_t bitmask = hal::FONT_5X7[idx][row];
        for (int col = 0; col < 5; ++col) {
            if (bitmask & (0x10 >> col)) {
                for (int dy = 0; dy < scale; ++dy) {
                    for (int dx = 0; dx < scale; ++dx) {
                        int px = pos.x + col * scale + dx;
                        int py = pos.y + row * scale + dy;
                        if (px >= 0 && px < surf->w && py >= 0 && py < surf->h) {
                            pixels[py * pitch + px] = color;
                        }
                    }
                }
            }
        }
    }
}

/****************************/
/*       Draw Text          */
/****************************/
static void draw_text( SDL_Surface*       surf,
                       core::Point<int>   pos,
                       const std::string& text,
                       uint32_t           color,
                       int                scale ) {
    int cx = pos.x;
    for (char c : text) {
        draw_glyph( surf, core::Point<int>( cx, pos.y ), c, color, scale );
        cx += 6 * scale;
    }
}

/****************************/
/*       Draw Line          */
/****************************/
static void draw_line( SDL_Surface*     surf,
                       core::Point<int> start,
                       core::Point<int> end,
                       uint32_t         color ) {
    uint32_t* pixels = static_cast<uint32_t*>(surf->pixels);
    int pitch = static_cast<int>(surf->pitch) / static_cast<int>(sizeof(uint32_t));

    if (start.y == end.y) {
        for (int x = start.x; x <= end.x && x >= 0 && x < surf->w; ++x) {
            if (start.y >= 0 && start.y < surf->h) {
                pixels[start.y * pitch + x] = color;
            }
        }
    }
}

/****************************/
/*       Render Box         */
/****************************/
static void render_box( SDL_Surface*              surf,
                        const layout::Layout_Box& box,
                        uint32_t                  color ) {
    switch (box.kind) {
        case layout::Box_Kind::ATOM:
            draw_text( surf, box.tl(), box.text, color, box.scale );
            break;

        case layout::Box_Kind::FRACTION: {
            const auto& num = box.children[0];
            const auto& den = box.children[1];

            render_box( surf, num, color );

            int line_y = num.pos.y + num.size.y + 1;
            draw_line( surf,
                       core::Point<int>( box.pos.x, line_y ),
                       core::Point<int>( box.pos.x + box.size.x - 1, line_y ),
                       color );

            render_box( surf, den, color );
            break;
        }

        case layout::Box_Kind::POWER:
        case layout::Box_Kind::SEQUENCE:
            for (const auto& child : box.children) {
                render_box( surf, child, color );
            }
            break;

        case layout::Box_Kind::SUPERSCRIPT:
            break;
    }
}

/****************************/
/*       Save PNG           */
/****************************/
static bool save_png(const char* filename, SDL_Surface* surf) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        std::cerr << "Failed to open " << filename << " for writing\n";
        return false;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return false;
    }

    png_init_io(png, fp);

    // Write header
    png_set_IHDR(png, info, surf->w, surf->h, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // Convert and write pixel data
    uint32_t* pixels = static_cast<uint32_t*>(surf->pixels);
    int pitch = static_cast<int>(surf->pitch) / static_cast<int>(sizeof(uint32_t));
    std::vector<png_byte> row(surf->w * 3);

    for (int y = 0; y < surf->h; ++y) {
        for (int x = 0; x < surf->w; ++x) {
            uint32_t pixel = pixels[y * pitch + x];
            SDL_Color c;
            SDL_GetRGB(pixel, surf->format, &c.r, &c.g, &c.b);
            row[x * 3 + 0] = c.r;
            row[x * 3 + 1] = c.g;
            row[x * 3 + 2] = c.b;
        }
        png_write_row(png, row.data());
    }

    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    return true;
}

/****************************/
/*     Render Equation      */
/****************************/
static int render_equation(const std::string& expr, const std::string& output_file) {
    Parser parser(expr);
    ovb::ast::Node_Ptr tree;
    try {
        tree = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
        return 1;
    }

    SDL_Surface* surf = SDL_CreateRGBSurface( 0, RENDER_WIDTH, RENDER_HEIGHT, 32,
                                              SDL_MASK_R, SDL_MASK_G, SDL_MASK_B, SDL_MASK_A );
    if (!surf) {
        std::cerr << "Failed to create surface: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_FillRect( surf, nullptr, SDL_MapRGB( surf->format, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK ) );

    // Use Layout_Engine to build and position layout tree
    layout::Layout_Engine engine( RENDER_SCALE );
    auto root = engine.build( tree.get() );
    engine.prepare( root, core::Point<int>( RENDER_WIDTH, RENDER_HEIGHT ) );

    uint32_t white = SDL_MapRGB( surf->format, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE );
    render_box( surf, root, white );

    if ( !save_png( output_file.c_str(), surf ) ) {
        std::cerr << "Failed to save PNG\n";
        SDL_FreeSurface(surf);
        return 1;
    }

    SDL_FreeSurface(surf);
    std::cout << "Rendered \"" << expr << "\" to " << output_file << "\n";
    return 0;
}

/****************************/
/*          Main            */
/****************************/
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " \"<equation>\" [output.png]\n";
        std::cerr << "  Example: " << argv[0] << " \"1+2*3\" output.png\n";
        return 1;
    }

    std::string expr = argv[1];
    std::string output = (argc >= 3) ? argv[2] : "math_output.png";

    // Initialize SDL (minimal video init for surface creation)
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    int ret = render_equation(expr, output);

    SDL_Quit();
    return ret;
}
