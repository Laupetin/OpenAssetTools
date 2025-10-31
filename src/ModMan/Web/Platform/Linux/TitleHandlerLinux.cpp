#include "AssetHandlerLinux.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include "Web/UiAssets.h"

#include <format>
#include <iostream>
#include <unordered_map>

using namespace PLATFORM_NAMESPACE_LINUX;

namespace PLATFORM_NAMESPACE_LINUX
{
    void InstallTitleHandler(webview::webview& wv)
    {
        const auto webViewWidget = static_cast<GtkWidget*>(wv.browser_controller().value());
        const auto webView = WEBKIT_WEB_VIEW(webViewWidget);
        const auto windowWidget = static_cast<GtkWidget*>(wv.window().value());
        const auto window = GTK_WINDOW(windowWidget);

        auto on_title_changed = +[](GtkWidget* widget, GParamSpec paramSpec, GtkWindow* window)
        {
            gtk_window_set_title(window, webkit_web_view_get_title(WEBKIT_WEB_VIEW(widget)));
        };

        g_signal_connect(G_OBJECT(webView), "notify::title", G_CALLBACK(on_title_changed), (gpointer)window);
    }
} // namespace PLATFORM_NAMESPACE_LINUX

#endif
