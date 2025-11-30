#include "Log.h"

#include <format>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
    bool logLevelSet = false;

    bool globalUseColor = true;
    bool colorSet = false;

    bool CanUseColor()
    {
#ifdef _WIN32
        DWORD dwMode = 0;
        const auto stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(stdoutHandle, &dwMode);
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        {
            SetConsoleMode(stdoutHandle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            dwMode = 0;
            GetConsoleMode(stdoutHandle, &dwMode);
            if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                return false;
        }

        const auto stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
        GetConsoleMode(stderrHandle, &dwMode);
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        {
            SetConsoleMode(stderrHandle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            dwMode = 0;
            GetConsoleMode(stderrHandle, &dwMode);
            if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                return false;
        }
#endif

        return true;
    }
} // namespace

namespace con
{
    LogLevel _globalLogLevel = LogLevel::INFO;

    void init()
    {
        if (!logLevelSet)
            set_log_level(LogLevel::INFO);

        if (!colorSet)
            set_use_color(true);
    }

    void set_log_level(const LogLevel value)
    {
        logLevelSet = true;
        _globalLogLevel = value;
    }

    void set_use_color(const bool value)
    {
        colorSet = true;
        globalUseColor = value && CanUseColor();
    }

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
