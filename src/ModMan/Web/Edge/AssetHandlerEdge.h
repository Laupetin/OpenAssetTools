#pragma once

#include <webview/macros.h>

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#include "Web/WebViewLib.h"

namespace edge
{
    constexpr auto URL_PREFIX = "http://modman.local/";

    void InstallCustomProtocolHandler(webview::webview& wv);
} // namespace edge

#endif
