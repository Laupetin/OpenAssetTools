#include "PlatformUtilsWindows.h"

#ifdef _WIN32

#include <exception>
#include <format>
#include <Windows.h>

namespace PLATFORM_NAMESPACE_WINDOWS
{
    std::string WideStringToString(const std::wstring& wideString)
    {
        if (wideString.empty())
            return "";

        const auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), nullptr, 0, nullptr, nullptr);
        if (sizeNeeded <= 0)
            throw std::runtime_error(std::format("WideCharToMultiByte() failed: {}", sizeNeeded));

        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), result.data(), sizeNeeded, nullptr, nullptr);
        return result;
    }

    std::wstring StringToWideString(const std::string& string)
    {
        if (string.empty())
            return L"";

        const auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
        if (sizeNeeded <= 0)
            throw std::runtime_error(std::format("MultiByteToWideChar() failed: {}", sizeNeeded));

        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), result.data(), sizeNeeded);
        return result;
    }
} // namespace PLATFORM_NAMESPACE_WINDOWS

#endif
