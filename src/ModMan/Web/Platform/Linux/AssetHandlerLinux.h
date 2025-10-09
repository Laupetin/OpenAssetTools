#pragma once

#include "Web/Platform/Platform.h"

#include <webview/macros.h>

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include "Web/WebViewLib.h"

namespace PLATFORM_NAMESPACE_LINUX
{
    constexpr auto URL_PREFIX = "modman://localhost/";

    void InstallAssetHandler(webview::webview& wv);
} // namespace PLATFORM_NAMESPACE_LINUX

#endif
