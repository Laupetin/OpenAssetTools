#include "AssetHandlerLinux.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include "Web/UiAssets.h"

#include <format>
#include <iostream>
#include <unordered_map>

using namespace PLATFORM_NAMESPACE_LINUX;

namespace
{
    std::unordered_map<std::string, UiFile> assetLookup;

    void ModManUriSchemeRequestCb(WebKitURISchemeRequest* request, gpointer user_data)
    {
        const gchar* asset = webkit_uri_scheme_request_get_path(request);

        const auto foundUiFile = assetLookup.find(asset);
        if (foundUiFile != assetLookup.end())
        {
            gsize stream_length = foundUiFile->second.dataSize;
            GInputStream* stream = g_memory_input_stream_new_from_data(foundUiFile->second.data, foundUiFile->second.dataSize, nullptr);

            webkit_uri_scheme_request_finish(request, stream, stream_length, ui::GetMimeTypeForFileName(foundUiFile->second.filename));
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
} // namespace

namespace PLATFORM_NAMESPACE_LINUX
{
    void InstallAssetHandler(webview::webview& wv)
    {
        const auto widget = static_cast<GtkWidget*>(wv.browser_controller().value());
        const auto webView = WEBKIT_WEB_VIEW(widget);
        const auto context = webkit_web_view_get_context(webView);

        assetLookup = ui::BuildUiFileLookup();

        webkit_web_context_register_uri_scheme(context, "modman", ModManUriSchemeRequestCb, NULL, nullptr);
    }
} // namespace PLATFORM_NAMESPACE_LINUX

#endif
