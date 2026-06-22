/**
 * @file    file_logger.cpp
 * @author  Marvin Smith
 * @date    2025-01-20
 *
 * @brief   File-based logger implementation
 */

#include "file_logger.hpp"

// C++ Standard Libraries
#include <chrono>
#include <iomanip>
#include <sstream>

// Third-Party Libraries
#ifdef TARGET_RP2350
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#endif

// Project Libraries
#include <overboard/log/log_level.hpp>

namespace ovb::log {

/****************************/
/*        Constructor        */
/****************************/
File_Logger::File_Logger(const std::string& filename, bool enable_uart)
    : m_uart_enabled(enable_uart), m_initialized(false) {
    
    // Open log file
    m_file.open(filename, std::ios::app);
    if (m_file.is_open()) {
        m_initialized = true;
        m_file << "\n=== kbd_calc Log Session Started ===\n";
        m_file.flush();
    }
    
    // Initialize UART if requested and on RP2350
    if (m_uart_enabled) {
        init_uart();
    }
}

/****************************/
/*        Destructor         */
/****************************/
File_Logger::~File_Logger() {
    if (m_file.is_open()) {
        m_file << "=== kbd_calc Log Session Ended ===\n\n";
        m_file.close();
    }
}

/****************************/
/*           Log             */
/****************************/
void File_Logger::log(Log_Level level, std::string_view message) {
    log(level, std::source_location::current(), message);
}

/****************************/
/*      Log with Location    */
/****************************/
void File_Logger::log(Log_Level            level,
                     std::source_location loc,
                     std::string_view     message) {
    
    if (!m_initialized) {
        return;
    }
    
    std::string formatted = format_message(level, loc, message);
    
    // Write to file
    if (m_file.is_open()) {
        m_file << formatted;
        m_file.flush();
    }
    
    // Write to UART if enabled
    if (m_uart_enabled) {
        write_uart(formatted);
    }
}

/****************************/
/*         Get Time          */
/****************************/
uint64_t File_Logger::now_us() const {
#ifdef TARGET_RP2350
    return time_us_64();
#else
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
#endif
}

/****************************/
/*          Flush            */
/****************************/
void File_Logger::flush() {
    if (m_file.is_open()) {
        m_file.flush();
    }
}

/****************************/
/*      Initialize UART      */
/****************************/
void File_Logger::init_uart() {
#ifdef TARGET_RP2350
    // Initialize UART for console output
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);  // UART TX
    gpio_set_function(1, GPIO_FUNC_UART);  // UART RX
#endif
}

/****************************/
/*       Write UART          */
/****************************/
void File_Logger::write_uart(const std::string& message) {
#ifdef TARGET_RP2350
    uart_puts(uart0, message.c_str());
#endif
}

/****************************/
/*      Format Message       */
/****************************/
std::string File_Logger::format_message(Log_Level level, 
                                        std::source_location loc,
                                        std::string_view message) const {
    
    std::ostringstream oss;
    
    // Add timestamp
    uint64_t time_us = now_us();
    uint32_t time_ms = time_us / 1000;
    oss << "[" << std::setw(8) << std::setfill('0') << time_ms << "." 
        << std::setw(3) << std::setfill('0') << (time_us % 1000) << "] ";
    
    // Add log level
    oss << "[" << log_level_to_string(level) << "] ";
    
    // Add function and line if available
    if (loc.line() > 0) {
        // Extract just the function name from the full signature
        std::string function = loc.function_name();
        size_t paren_pos = function.find('(');
        if (paren_pos != std::string::npos) {
            function = function.substr(0, paren_pos);
        }
        
        // Extract just the class::method part
        size_t last_space = function.find_last_of(' ');
        if (last_space != std::string::npos) {
            function = function.substr(last_space + 1);
        }
        
        oss << "[" << function << ":" << loc.line() << "] ";
    }
    
    // Add message
    oss << message << "\n";
    
    return oss.str();
}

} // namespace ovb::log
