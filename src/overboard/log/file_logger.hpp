/**
 * @file    file_logger.hpp
 * @author  Marvin Smith
 * @date    2025-01-20
 *
 * @brief   File-based logger implementation
 */

#pragma once

// C++ Standard Libraries
#include <fstream>
#include <memory>
#include <string>

// Third-Party Libraries
#ifdef TARGET_RP2350
#include <pico/stdlib.h>
#include <hardware/uart.h>
#endif

// Project Libraries
#include <overboard/log/i_logger.hpp>

namespace ovb::log {

/**
 * @brief File-based logger implementation
 *
 * Supports both file output and optional UART console output
 * for debugging display issues on RP2350.
 */
class File_Logger : public I_Logger {

    public:

        /**
         * @brief Constructor
         * @param filename Path to log file
         * @param enable_uart Enable UART console output (RP2350 only)
         */
        File_Logger(const std::string& filename, 
                   bool enable_uart = false);

        /**
         * @brief Destructor
         */
        virtual ~File_Logger();

        /**
         * @brief Log a message at the specified level
         */
        void log(Log_Level level, std::string_view message) override;

        /**
         * @brief Log a message at the specified level with source location
         */
        void log(Log_Level            level,
                 std::source_location loc,
                 std::string_view     message = std::string_view()) override;

        /**
         * @brief Get current time in microseconds
         */
        uint64_t now_us() const override;

        /**
         * @brief Flush pending log messages
         */
        void flush();

    private:

        std::ofstream m_file;
        bool m_uart_enabled;
        bool m_initialized;

        /**
         * @brief Initialize UART for console output
         */
        void init_uart();

        /**
         * @brief Write to UART console
         */
        void write_uart(const std::string& message);

        /**
         * @brief Format log message with timestamp and level
         */
        std::string format_message(Log_Level level, 
                                  std::source_location loc,
                                  std::string_view message) const;
};

} // namespace ovb::log
