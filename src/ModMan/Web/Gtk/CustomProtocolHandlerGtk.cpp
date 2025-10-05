#include "CustomProtocolHandlerGtk.h"

#pragma warning(push, 0)
#include <webview/macros.h>
#include <webview/webview.h>
#pragma warning(pop)

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include "Web/UiAssets.h"

#include <format>
#include <iostream>

#define G_SPAWN_ERROR g_spawn_error_quark()

G_DEFINE_QUARK(g - spawn - error - quark, g_spawn_error)

namespace
{
    std::unordered_map<std::string, UiFile> assetLookup;

    const char* ContentTypeForAssetName(const std::string& assetName)
    {
        const char* mimeType;

        if (assetName.ends_with(".html"))
        {
            mimeType = "text/html";
        }
        else if (assetName.ends_with(".js"))
        {
            mimeType = "text/javascript";
        }
        else if (assetName.ends_with(".css"))
        {
            mimeType = "text/css";
        }
        else
        {
            mimeType = "application/octet-stream";
        }

        return mimeType;
    }

    void ModManUriSchemeRequestCb(WebKitURISchemeRequest* request, gpointer user_data)
    {
        const gchar* asset = webkit_uri_scheme_request_get_path(request);

        std::cout << std::format("Modman request: {}\n", asset);

        const auto foundUiFile = assetLookup.find(asset);
        if (foundUiFile != assetLookup.end())
        {
            gsize stream_length = foundUiFile->second.dataSize;
            GInputStream* stream = g_memory_input_stream_new_from_data(foundUiFile->second.data, foundUiFile->second.dataSize, nullptr);

            webkit_uri_scheme_request_finish(request, stream, stream_length, ContentTypeForAssetName(foundUiFile->second.filename));
            g_object_unref(stream);
        }
        else
        {
            GError* error = g_error_new(G_SPAWN_ERROR, 123, "Could not find %s.", asset);
            webkit_uri_scheme_request_finish_error(request, error);
            g_error_free(error);
            return;
        }
    }

    void OnResourceLoadStarted(WebKitWebView* self, WebKitWebResource* resource, WebKitURIRequest* request, gpointer user_data)
    {
        std::cout << webkit_uri_request_get_http_method(request) << " " << webkit_uri_request_get_uri(request) << "\n";

        std::string uri(webkit_uri_request_get_uri(request));
        if (uri.starts_with("http://"))
        {
            uri = std::format("modman://{}", uri.substr(7));
        }
        else if (uri.starts_with("https://"))
        {
            uri = std::format("modman://{}", uri.substr(8));
        }

        std::cout << std::format("Setting uri: {}\n", uri);
        webkit_uri_request_set_uri(request, uri.c_str());
    }
} // namespace

namespace gtk
{
    void InstallCustomProtocolHandler(webview::webview& wv)
    {
        const auto widget = static_cast<GtkWidget*>(wv.browser_controller().value());
        const auto webView = WEBKIT_WEB_VIEW(widget);
        const auto context = webkit_web_view_get_context(webView);

        assetLookup = BuildUiFileLookup();

        g_signal_connect(webView, "resource-load-started", G_CALLBACK(OnResourceLoadStarted), NULL);

        webkit_web_context_register_uri_scheme(context, "modman", ModManUriSchemeRequestCb, NULL, nullptr);
    }
} // namespace gtk

#endif
