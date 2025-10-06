#pragma once

#include <webview/macros.h>

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <webview/webview.h>
#pragma GCC diagnostic pop

namespace gtk
{
    constexpr auto URL_PREFIX = "modman://localhost/";

    void InstallCustomProtocolHandler(webview::webview& wv);
} // namespace gtk

#endif
