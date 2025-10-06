#pragma once

#include <webview/macros.h>

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#pragma warning(push, 0)
#include <webview/webview.h>
#pragma warning(pop)

namespace edge
{
    constexpr auto URL_PREFIX = "http://modman.local/";

    void InstallCustomProtocolHandler(webview::webview& wv);
} // namespace edge

#endif
