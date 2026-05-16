#pragma once

#include <overboard/hal/i_logger.hpp>

class Stdout_Logger : public I_Logger {
    public:
        explicit Stdout_Logger(Log_Level min_level = Log_Level::Debug);

        void     log(Log_Level level, std::string_view message) override;
        uint64_t now_us() const override;

    private:
        Log_Level m_min_level;

        static const char* level_tag(Log_Level level);
};
