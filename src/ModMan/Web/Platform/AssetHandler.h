#pragma once

#include "Web/Platform/Platform.h"
#include "Web/WebViewLib.h"

#include <webview/macros.h>

namespace ui
{
#if defined(PLATFORM_WINDOWS)
    constexpr auto URL_PREFIX = "http://modman.local/";
#elif defined(PLATFORM_LINUX)
    constexpr auto URL_PREFIX = "modman://localhost/";
#endif

    void InstallAssetHandler(webview::webview& wv);
} // namespace ui
