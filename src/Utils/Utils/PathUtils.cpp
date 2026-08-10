#include "PathUtils.h"

#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace utils
{
    std::string GetExecutablePath()
    {
#ifdef _WIN32
        char result[MAX_PATH];
        return std::string(result, GetModuleFileNameA(NULL, result, MAX_PATH));
#elif defined(__linux__)
        char result[PATH_MAX];
        const auto count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
#elif defined(__APPLE__)
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);

        std::string result(size, '\0');
        if (_NSGetExecutablePath(result.data(), &size) != 0)
        {
            throw std::runtime_error("Could not determine executable path");
        }

        return result.c_str();
#else
        throw std::runtime_error("Unknown platform for executable path");
#endif
    }
} // namespace utils
