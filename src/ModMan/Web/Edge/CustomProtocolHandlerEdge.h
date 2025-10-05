#pragma once

#pragma warning(push, 0)
#include <webview/macros.h>
#pragma warning(pop)

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#pragma warning(push, 0)
#include <webview/webview.h>
#pragma warning(pop)

namespace edge
{
    void InstallCustomProtocolHandler(webview::webview& wv);
}

#endif
