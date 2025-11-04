#pragma once

#include "Web/Platform/Platform.h"

#ifdef PLATFORM_WINDOWS

#include <string>

namespace utils
{
    std::string WideStringToString(const std::wstring& wideString);
    std::wstring StringToWideString(const std::string& string);
} // namespace utils

#endif
