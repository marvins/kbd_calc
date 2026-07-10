/**
 * @file    mode_controller.hpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   Operating Mode Controller
 *
 * Manages state transitions between Inactive, Passthrough, and Macro modes.
 */
#pragma once

// Project Libraries
#include <overboard/protocol/uart_protocol.hpp>

using namespace ovb::protocol;

/**
 * @class Mode_Controller
 * @brief Operating mode controller
 *
 * Manages state transitions between Inactive, Passthrough, and Macro modes.
 */
class Mode_Controller {
    public:

        /**
         * @brief Default constructor
         */
        Mode_Controller();

        /**
         * @brief Initialize mode controller
         */
        void init();

        /**
         * @brief Set the current mode
         */
        void set_mode(Operating_Mode mode);

        /**
         * @brief Get the current mode
         */
        Operating_Mode current_mode() const;

    private:

        /// @brief Current operating mode
        Operating_Mode m_current_mode;
};
