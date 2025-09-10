#pragma once

#include <format>

namespace con
{
    enum class LogLevel
    {
        DEBUG = 0,
        INFO,
        WARN,
        ERROR
    };

    extern LogLevel globalLogLevel;
    extern bool globalUseColor;

    void _debug_internal(const std::string& str);
    void _info_internal(const std::string& str);
    void _warn_internal(const std::string& str);
    void _error_internal(const std::string& str);

    template<class... Args> void debug(std::format_string<Args...> fmt, Args&&... args)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::DEBUG))
            return;
        _debug_internal(std::vformat(fmt.get(), std::make_format_args(args...)));
    }

    template<class... Args> void info(std::format_string<Args...> fmt, Args&&... args)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::INFO))
            return;
        _info_internal(std::vformat(fmt.get(), std::make_format_args(args...)));
    }

    template<class... Args> void warn(std::format_string<Args...> fmt, Args&&... args)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::WARN))
            return;
        _warn_internal(std::vformat(fmt.get(), std::make_format_args(args...)));
    }

    template<class... Args> void error(std::format_string<Args...> fmt, Args&&... args)
    {
        _error_internal(std::vformat(fmt.get(), std::make_format_args(args...)));
    }
} // namespace con
