/**
 * @file    input_linux.cpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Linux input event implementation
 */
#include <overboard/hal/pi_zero/input_linux.hpp>

// C++ Standard Libraries
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <unistd.h>

#include <array>
#include <cstring>

// Overboard Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pi_zero {

struct Linux_Input::Impl {
    std::vector<int>                fds;
    std::vector<std::string>        device_paths;
    std::atomic<bool>               quit_requested = false;
    std::array<pollfd, 16>          poll_fds;
    size_t                          num_poll_fds = 0;
};

/************************************/
/*          Constructor             */
/************************************/
Linux_Input::Linux_Input(const std::vector<std::string>& device_paths)
    : m_impl(std::make_unique<Impl>()) {
    m_impl->device_paths = device_paths;

    for (const auto& path : device_paths) {
        int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            LOG_DEBUG("Failed to open input device: ", path);
            continue;
        }

        // Grab the device exclusively
        if (ioctl(fd, EVIOCGRAB, 1) < 0) {
            LOG_DEBUG("Failed to grab input device: ", path);
        }

        m_impl->fds.push_back(fd);
        if (m_impl->num_poll_fds < m_impl->poll_fds.size()) {
            m_impl->poll_fds[m_impl->num_poll_fds].fd = fd;
            m_impl->poll_fds[m_impl->num_poll_fds].events = POLLIN;
            m_impl->num_poll_fds++;
        }
    }

    LOG_INFO("Linux_Input: Opened ", m_impl->fds.size(), " input devices");
}

/************************************/
/*          Destructor              */
/************************************/
Linux_Input::~Linux_Input() {
    for (int fd : m_impl->fds) {
        ioctl(fd, EVIOCGRAB, 0);  // Release grab
        close(fd);
    }
}

/************************************/
/*          Poll for events         */
/************************************/
bool Linux_Input::poll(Key_Event& out_event) {
    struct input_event ev;

    for (int fd : m_impl->fds) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY) {
                out_event.kind = Key_Event_Kind::Action;
                out_event.key_index = static_cast<int>(ev.code);
                out_event.type = (ev.value == 1) ? Key_Event_Type::Press : Key_Event_Type::Release;

                // Check for Ctrl+C
                if (ev.code == KEY_C && ev.value == 1) {
                    // Note: Would need to track Ctrl state properly in real implementation
                }

                return true;
            }
        }
    }

    return false;
}

/************************************/
/*          Should quit             */
/************************************/
bool Linux_Input::should_quit() const {
    return m_impl->quit_requested.load();
}

/************************************/
/*          Pump events             */
/************************************/
void Linux_Input::pump() {
    if (m_impl->num_poll_fds == 0) {
        return;
    }

    int ret = poll(m_impl->poll_fds.data(), m_impl->num_poll_fds, 0);
    if (ret < 0) {
        LOG_DEBUG("poll() failed");
    }
}

} // namespace ovb::hal::pi_zero
