#pragma once

#include <webview/macros.h>

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include "Web/WebViewLib.h"

namespace gtk
{
    constexpr auto URL_PREFIX = "modman://localhost/";

    void InstallCustomProtocolHandler(webview::webview& wv);
} // namespace gtk

#endif
