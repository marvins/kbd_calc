/**
 * @file    mode_controller.cpp
 * @author  Overboard Team
 * @date    2026-07-09
 *
 * @brief   Operating Mode Controller Implementation
 */

#include "mode_controller.hpp"

// C++ Standard Libraries
#include <cstdio>

using namespace ovb::protocol;

/***************************/
/*       Constructor       */
/***************************/
Mode_Controller::Mode_Controller()
    : m_current_mode(Operating_Mode::Inactive)
{
}

/***************************/
/*          Init           */
/***************************/
void Mode_Controller::init() {
    m_current_mode = Operating_Mode::Inactive;
}

/***************************/
/*        Set Mode         */
/***************************/
void Mode_Controller::set_mode(Operating_Mode mode) {
    if (mode != m_current_mode) {
        printf("Mode transition: %d -> %d\n",
               static_cast<int>(m_current_mode),
               static_cast<int>(mode));
        m_current_mode = mode;
    }
}

/***************************/
/*      Current Mode       */
/***************************/
Operating_Mode Mode_Controller::current_mode() const {
    return m_current_mode;
}
