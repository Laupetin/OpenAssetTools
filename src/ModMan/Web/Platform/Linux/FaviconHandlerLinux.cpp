#include "Web/Platform/FaviconHandler.h"
#include "Web/Platform/Platform.h"

#ifdef PLATFORM_LINUX

#include "Web/UiAssets.h"

#include <format>
#include <iostream>
#include <unordered_map>

namespace ui
{
    void InstallFaviconHandler(webview::webview& wv)
    {
        // The icon system on Linux works a bit different than on Windows
        // and doesn't really support this kind of dynamic icon setting
        // we skip it for now
    }
} // namespace ui

#endif
