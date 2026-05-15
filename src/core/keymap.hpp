#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string_view>

enum class Key_Label : uint8_t {
    NONE = 0,

    // Digits
    D_0, D_1, D_2, D_3, D_4,
    D_5, D_6, D_7, D_8, D_9,

    // Hex digits
    HEX_A, HEX_B, HEX_C, HEX_D, HEX_E, HEX_F,

    // Arithmetic
    ADD, SUBTRACT, MULTIPLY, DIVIDE, EQUALS, DECIMAL, PERCENT,

    // Editing
    BACKSPACE, CLEAR, ALL_CLEAR, NEGATE, PAREN_OPEN, PAREN_CLOSE,

    // Cursor
    CURSOR_LEFT, CURSOR_RIGHT,

    // Scientific
    SIN, COS, TAN, ASIN, ACOS, ATAN,
    LOG, LN, EXP, SQRT, FACTORIAL, RECIPROCAL,
    PI, EULER, PHI, TAU,
    POWER_2, POWER_N,

    // Programmer
    BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, SHIFT_LEFT, SHIFT_RIGHT,

    // Layer / meta
    LAYER_NEXT_TRIG,
    LAYER_NEXT_CONST,
    LAYER_PREV,
    LAYER_PREV_CONST,
};

std::string label_string(Key_Label lbl);

enum class Key_Code : uint16_t {
    NONE = 0,

    // Digits
    DIGIT_0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,

    // Basic ops
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EQUALS,
    DECIMAL,
    CLEAR,
    ALL_CLEAR,
    BACKSPACE,

    // Grouping / extra basic
    PAREN_OPEN,
    PAREN_CLOSE,
    PERCENT,
    NEGATE,

    // Memory
    MEM_STORE,
    MEM_RECALL,
    MEM_ADD,
    MEM_CLEAR,

    // Scientific
    SIN,
    COS,
    TAN,
    ASIN,
    ACOS,
    ATAN,
    LOG,
    LN,
    EXP,
    SQRT,
    POWER_2,
    POWER_3,
    POWER_N,
    FACTORIAL,
    RECIPROCAL,
    PI,
    EULER,

    // Programmer
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    BIT_NOT,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    HEX_A,
    HEX_B,
    HEX_C,
    HEX_D,
    HEX_E,
    HEX_F,

    // Cursor / editing
    CURSOR_LEFT,
    CURSOR_RIGHT,

    // Constants
    PHI,
    TAU,

    // Meta / control
    LAYER_NEXT,
    LAYER_PREV,
    CONST_LAYER,
    LAYER_HOME,
    MODE_TOGGLE,
};

static constexpr int GRID_COLS = 5;
static constexpr int GRID_ROWS = 6;
static constexpr int GRID_KEYS = GRID_COLS * GRID_ROWS;

struct Key_Def {
    Key_Code  code;
    Key_Label label;
};

struct Layer {
    std::string_view                    name;
    std::array<Key_Def, GRID_KEYS>      keys;
};

class Keymap {
    public:
        static constexpr int LAYER_COUNT = 4;

        constexpr int layer_count() const { return LAYER_COUNT; }

        constexpr const Layer& get_layer(std::size_t index) const;
        constexpr const Key_Def& get_key(std::size_t layer, std::size_t key_index) const;

        static constexpr std::array<Layer, LAYER_COUNT> build_layers() {
            using KC = Key_Code;
            using KL = Key_Label;

            return {{
                { "Basic", {{
                    { KC::LAYER_NEXT,   KL::LAYER_NEXT_TRIG}, { KC::NONE,      KL::NONE        }, { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT }, { KC::BACKSPACE, KL::BACKSPACE  },
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::SQRT,        KL::SQRT        }, { KC::POWER_N,      KL::POWER_N     }, { KC::ADD,       KL::ADD        },
                    { KC::CONST_LAYER, KL::LAYER_NEXT_CONST}, { KC::DIGIT_7,   KL::D_7         }, { KC::DIGIT_8,     KL::D_8         }, { KC::DIGIT_9,      KL::D_9         }, { KC::SUBTRACT,  KL::SUBTRACT   },
                    { KC::NONE,        KL::NONE            }, { KC::DIGIT_4,   KL::D_4         }, { KC::DIGIT_5,     KL::D_5         }, { KC::DIGIT_6,      KL::D_6         }, { KC::MULTIPLY,  KL::MULTIPLY   },
                    { KC::NONE,        KL::NONE            }, { KC::DIGIT_1,   KL::D_1         }, { KC::DIGIT_2,     KL::D_2         }, { KC::DIGIT_3,      KL::D_3         }, { KC::DIVIDE,    KL::DIVIDE     },
                    { KC::LAYER_PREV,  KL::LAYER_PREV      }, { KC::NEGATE,    KL::NEGATE      }, { KC::DIGIT_0,     KL::D_0         }, { KC::DECIMAL,      KL::DECIMAL     }, { KC::EQUALS,    KL::EQUALS     },
                }}},
                { "Trig", {{
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT }, { KC::BACKSPACE, KL::BACKSPACE  },
                    { KC::NONE,        KL::NONE            }, { KC::SIN,       KL::SIN         }, { KC::COS,         KL::COS         }, { KC::TAN,          KL::TAN         }, { KC::NONE,      KL::NONE       },
                    { KC::NONE,        KL::NONE            }, { KC::ASIN,      KL::ASIN        }, { KC::ACOS,        KL::ACOS        }, { KC::ATAN,         KL::ATAN        }, { KC::NONE,      KL::NONE       },
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                    { KC::LAYER_PREV,  KL::LAYER_PREV      }, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                }}},
                { "Constants", {{
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT }, { KC::BACKSPACE, KL::BACKSPACE  },
                    { KC::NONE,        KL::NONE            }, { KC::PI,        KL::PI          }, { KC::EULER,       KL::EULER       }, { KC::PHI,          KL::PHI         }, { KC::TAU,       KL::TAU        },
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                    { KC::NONE,        KL::NONE            }, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                    { KC::LAYER_HOME,  KL::LAYER_PREV_CONST}, { KC::NONE,      KL::NONE        }, { KC::NONE,        KL::NONE        }, { KC::NONE,         KL::NONE        }, { KC::NONE,      KL::NONE       },
                }}},
                { "Programmer", {{
                    { KC::NONE,        KL::NONE            }, { KC::ALL_CLEAR, KL::ALL_CLEAR   }, { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT }, { KC::BACKSPACE, KL::BACKSPACE  },
                    { KC::NONE,        KL::NONE            }, { KC::BIT_AND,   KL::BIT_AND     }, { KC::BIT_OR,      KL::BIT_OR      }, { KC::BIT_XOR,      KL::BIT_XOR     }, { KC::BIT_NOT,   KL::BIT_NOT    },
                    { KC::NONE,        KL::NONE            }, { KC::HEX_A,     KL::HEX_A       }, { KC::HEX_B,       KL::HEX_B       }, { KC::HEX_C,        KL::HEX_C       }, { KC::SHIFT_LEFT,KL::SHIFT_LEFT },
                    { KC::NONE,        KL::NONE            }, { KC::HEX_D,     KL::HEX_D       }, { KC::HEX_E,       KL::HEX_E       }, { KC::HEX_F,        KL::HEX_F       }, { KC::SHIFT_RIGHT,KL::SHIFT_RIGHT},
                    { KC::NONE,        KL::NONE            }, { KC::DIGIT_7,   KL::D_7         }, { KC::DIGIT_8,     KL::D_8         }, { KC::DIGIT_9,      KL::D_9         }, { KC::DIVIDE,    KL::DIVIDE     },
                    { KC::LAYER_PREV,  KL::LAYER_PREV      }, { KC::NEGATE,    KL::NEGATE      }, { KC::DIGIT_0,     KL::D_0         }, { KC::DECIMAL,      KL::DECIMAL     }, { KC::EQUALS,    KL::EQUALS     },
                }}},
            }};
        }

};

inline constexpr std::array<Layer, Keymap::LAYER_COUNT> KEYMAP_LAYERS = Keymap::build_layers();

inline constexpr const Layer& Keymap::get_layer(std::size_t index) const {
    return KEYMAP_LAYERS[index];
}

inline constexpr const Key_Def& Keymap::get_key(std::size_t layer, std::size_t key_index) const {
    return KEYMAP_LAYERS[layer].keys[key_index];
}
