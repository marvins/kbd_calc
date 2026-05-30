/**
 * @file    i_logger.hpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Logger interface for platform abstraction
 *
 * Core logging interface that can be implemented for
 * different targets (stdout, file, hardware UART, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>

// Project Libraries
#include <overboard/log/log_level.hpp>

namespace ovb::log {

/**
 * @brief   Logger interface for platform abstraction
 */
class I_Logger {

    public:

        /**
         * @brief Virtual destructor for interface
         */
        virtual ~I_Logger() = default;

        /**
         * @brief Log a message at the specified level
         * @param level Log severity level
         * @param message Message to log
         */
        virtual void log( Log_Level level, std::string_view message ) = 0;

        /**
         * @brief Log a message at the specified level with source location
         * @param level Log severity level
         * @param loc Source location
         * @param message Message to log
         */
        virtual void log( Log_Level            level,
                          std::source_location loc,
                          std::string_view     message = std::string_view() ) = 0;

        /**
         * @brief Log multiple arguments at the specified level
         * @param level Log severity level
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void log( Log_Level level,
                  Args&&... args ) {
            log( level, std::string_view(concatenate( args... )) );
        }

        /**
         * @brief Log multiple arguments at the specified level with source location
         * @param level Log severity level
         * @param loc Source location
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void log( Log_Level            level,
                  std::source_location loc,
                  Args&&...            args ) {
            log( level, loc, std::string_view(concatenate( args... )) );
        }

        /**
         * @brief Log a trace-level message
         * @param msg Message to log
         */
        void trace(std::string_view msg) { log(Log_Level::Trace, msg); }

        /**
         * @brief Log a trace-level message with source location
         * @param loc Source location
         * @param msg Message to log
         */
        void trace( std::source_location loc,
                    std::string_view     msg ) {
            log( Log_Level::Trace, loc, msg );
        }

        /**
         * @brief Log multiple arguments at trace level
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void trace(Args&&... args) {
            log( Log_Level::Trace, args... );
        }

        /**
         * @brief Log multiple arguments at trace level with source location
         * @param loc Source location
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void trace( std::source_location loc,
                    Args&&...            args ) {
            log( Log_Level::Trace, loc, args... );
        }

        /**
         * @brief Log a debug-level message
         * @param msg Message to log
         */
        void debug(std::string_view msg) { log(Log_Level::Debug, msg); }

        /**
         * @brief Log a debug-level message with source location
         * @param loc Source location
         * @param msg Message to log
         */
        void debug( std::source_location loc,
                    std::string_view     msg ) {
            log( Log_Level::Debug, loc, msg );
        }

        /**
         * @brief Log multiple arguments at debug level
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void debug( Args&&... args ) {
            log( Log_Level::Debug,
                 args... );
        }

        /**
         * @brief Log multiple arguments at debug level with source location
         * @param loc Source location
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void debug( std::source_location loc,
                    Args&&...            args ) {
            log( Log_Level::Debug, loc, args... );
        }

        /**
         * @brief Log an info-level message
         * @param msg Message to log
         */
        void info (std::string_view msg) { log(Log_Level::Info,  msg); }

        /**
         * @brief Log an info-level message with source location
         * @param loc Source location
         * @param msg Message to log
         */
        void info( std::source_location loc,
                   std::string_view     msg ) {
            log( Log_Level::Info, loc, msg );
        }

        /**
         * @brief Log multiple arguments at info level
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void info( Args&&... args ) {
            log( Log_Level::Info, args... );
        }

        /**
         * @brief Log multiple arguments at info level with source location
         * @param loc Source location
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void info( std::source_location loc,
                   Args&&...            args ) {
            log( Log_Level::Info, loc, args... );
        }

        /**
         * @brief Log a warning-level message
         * @param msg Message to log
         */
        void warn (std::string_view msg) { log(Log_Level::Warn,  msg); }

        /**
         * @brief Log a warning-level message with source location
         * @param loc Source location
         * @param msg Message to log
         */
        void warn( std::source_location loc,
                   std::string_view     msg ) {
            log( Log_Level::Warn, loc, msg );
        }

        /**
         * @brief Log multiple arguments at warning level
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void warn( Args&&... args ) {
            log( Log_Level::Warn, args... );
        }

        /**
         * @brief Log multiple arguments at warning level with source location
         * @param loc Source location
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void warn( std::source_location loc,
                   Args&&...            args ) {
            log( Log_Level::Warn, loc, args... );
        }

        /**
         * @brief Log an error-level message
         * @param msg Message to log
         */
        void error(std::string_view msg) { log(Log_Level::Error, msg); }

        /**
         * @brief Log an error-level message with source location
         * @param loc Source location
         * @param msg Message to log
         */
        void error( std::source_location loc,
                    std::string_view     msg ) {
            log( Log_Level::Error, loc, msg );
        }

        /**
         * @brief Log multiple arguments at error level
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void error( Args&&... args ) {
            log( Log_Level::Error, args... );
        }

        /**
         * @brief Log multiple arguments at error level with source location
         * @param loc Source location
         * @param args Arguments to concatenate and log
         */
        template<typename... Args>
        void error( std::source_location loc,
                    Args&&...            args ) {
            log( Log_Level::Error, loc, args... );
        }

        /**
         * @brief Get current time in microseconds
         * @return Current time in microseconds
         */
        virtual uint64_t now_us() const = 0;

    private:
        /**
         * @brief Concatenate multiple arguments into a single string
         * @param args Arguments to concatenate
         * @return Concatenated string
         */
        template<typename... Args>
        static std::string concatenate( Args... args ) {
            std::ostringstream oss;
            (oss << ... << args);
            return oss.str();
        }
};

} // namespace ovb::log
