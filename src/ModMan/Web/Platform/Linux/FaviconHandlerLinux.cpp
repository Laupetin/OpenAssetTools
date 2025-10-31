#include "AssetHandlerLinux.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include "Web/UiAssets.h"

#include <format>
#include <iostream>
#include <unordered_map>

using namespace PLATFORM_NAMESPACE_LINUX;

namespace
{
} // namespace

namespace PLATFORM_NAMESPACE_LINUX
{
    void InstallFaviconHandler(webview::webview& wv) {}
} // namespace PLATFORM_NAMESPACE_LINUX

#endif
