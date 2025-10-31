#pragma once

#include "Web/Platform/Platform.h"

#include <webview/macros.h>

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#include "Web/WebViewLib.h"

namespace PLATFORM_NAMESPACE_WINDOWS
{
    void InstallFaviconHandler(webview::webview& wv);
} // namespace PLATFORM_NAMESPACE_WINDOWS

#endif
