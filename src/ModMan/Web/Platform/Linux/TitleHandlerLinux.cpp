#include "Web/Platform/Platform.h"
#include "Web/Platform/TitleHandler.h"

#ifdef PLATFORM_LINUX

#include "Web/UiAssets.h"

#include <format>
#include <iostream>
#include <unordered_map>

namespace ui
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
} // namespace ui

#endif
