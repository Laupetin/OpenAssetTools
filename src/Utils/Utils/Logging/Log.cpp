#include "Log.h"

#include <format>
#include <iostream>

namespace con
{
    LogLevel globalLogLevel = LogLevel::INFO;
    bool globalUseColor = true;

    void _debug_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cout << std::format("\x1B[90m{}\x1B[0m\n", str);
        else
            std::cout << std::format("{}\n", str);
    }

    void _info_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cout << std::format("\x1B[37m{}\x1B[0m\n", str);
        else
            std::cout << std::format("{}\n", str);
    }

    void _warn_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cout << std::format("\x1B[33m{}\x1B[0m\n", str);
        else
            std::cout << std::format("{}\n", str);
    }

    void _error_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cerr << std::format("\x1B[31m{}\x1B[0m\n", str);
        else
            std::cerr << std::format("{}\n", str);
    }
} // namespace con
