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
    constexpr auto URL_PREFIX = "modman://localhost/";

    void InstallCustomProtocolHandler(webview::webview& wv);
} // namespace gtk

#endif
