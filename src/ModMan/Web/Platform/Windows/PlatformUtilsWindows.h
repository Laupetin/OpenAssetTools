#pragma once

#ifdef _WIN32

#include "Web/Platform/Platform.h"

#include <optional>
#include <string>

namespace PLATFORM_NAMESPACE_WINDOWS
{
    std::string WideStringToString(const std::wstring& wideString);
    std::wstring StringToWideString(const std::string& string);
} // namespace PLATFORM_NAMESPACE_WINDOWS

#endif
