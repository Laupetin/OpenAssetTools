#pragma once

#pragma warning(push, 0)
#include <webview/macros.h>
#pragma warning(pop)

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#pragma warning(push, 0)
#include <webview/webview.h>
#pragma warning(pop)

namespace gtk
{
    void InstallCustomProtocolHandler(webview::webview& wv);
}

#endif
