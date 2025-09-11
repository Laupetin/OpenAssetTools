#pragma once

#include <format>
#include <string>

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

    inline void debug(const std::string& str)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::DEBUG))
            return;
        _debug_internal(str);
    }

    template<class Arg0, class... OtherArgs> void debug(std::format_string<Arg0, OtherArgs...> fmt, Arg0&& arg0, OtherArgs&&... otherArgs)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::DEBUG))
            return;
        _debug_internal(std::vformat(fmt.get(), std::make_format_args(arg0, otherArgs...)));
    }

    inline void info(const std::string& str)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::INFO))
            return;
        _info_internal(str);
    }

    template<class Arg0, class... OtherArgs> void info(std::format_string<Arg0, OtherArgs...> fmt, Arg0&& arg0, OtherArgs&&... otherArgs)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::INFO))
            return;
        _info_internal(std::vformat(fmt.get(), std::make_format_args(arg0, otherArgs...)));
    }

    inline void warn(const std::string& str)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::WARN))
            return;
        _warn_internal(str);
    }

    template<class Arg0, class... OtherArgs> void warn(std::format_string<Arg0, OtherArgs...> fmt, Arg0&& arg0, OtherArgs&&... otherArgs)
    {
        if (static_cast<unsigned>(globalLogLevel) > static_cast<unsigned>(LogLevel::WARN))
            return;
        _warn_internal(std::vformat(fmt.get(), std::make_format_args(arg0, otherArgs...)));
    }

    inline void error(const std::string& str)
    {
        _error_internal(str);
    }

    template<class Arg0, class... OtherArgs> void error(std::format_string<Arg0, OtherArgs...> fmt, Arg0&& arg0, OtherArgs&&... otherArgs)
    {
        _error_internal(std::vformat(fmt.get(), std::make_format_args(arg0, otherArgs...)));
    }
} // namespace con
