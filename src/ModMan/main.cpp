#ifdef _MSC_VER
#pragma warning(push, 0)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "webview/webview.h"

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include "Web/Edge/AssetHandlerEdge.h"
#include "Web/Gtk/AssetHandlerGtk.h"
#include "Web/ViteAssets.h"

#include <chrono>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

using namespace std::string_literals;

namespace
{
#ifdef _DEBUG
    std::optional<webview::webview> devToolWindow;

    void RunDevToolsWindow(webview::webview& parent)
    {
        try
        {
            auto& newWindow = devToolWindow.emplace(false, nullptr);
            newWindow.set_title("Devtools");
            newWindow.set_size(640, 480, WEBVIEW_HINT_NONE);
            newWindow.set_size(480, 320, WEBVIEW_HINT_MIN);
            newWindow.navigate(std::format("http://localhost:{}/__devtools__/", VITE_DEV_SERVER_PORT));
        }
        catch (const webview::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
#endif

    int RunMainWindow()
    {
        try
        {
            webview::webview w(
#ifdef _DEBUG
                true,
#else
                false,
#endif
                nullptr);
            w.set_title("OpenAssetTools ModMan");
            w.set_size(1280, 640, WEBVIEW_HINT_NONE);
            w.set_size(480, 320, WEBVIEW_HINT_MIN);

            // A binding that counts up or down and immediately returns the new value.
            w.bind("greet",
                   [&](const std::string& req) -> std::string
                   {
                       const auto name = req.substr(2, req.size() - 4);
                       w.notify("greeting", webview::json_escape(name));
                       return webview::json_escape(std::format("Hello from C++ {}!", name));
                   });

            // A binding that creates a new thread and returns the result at a later time.
            w.bind(
                "compute",
                [&](const std::string& id, const std::string& req, void* /*arg*/)
                {
                    // Create a thread and forget about it for the sake of simplicity.
                    std::thread(
                        [&, id, req]
                        {
                            // Simulate load.
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                            // Imagine that req is properly parsed or use your own JSON parser.
                            const auto* result = "42";
                            w.resolve(id, 0, result);
                        })
                        .detach();
                },
                nullptr);

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
            edge::InstallCustomProtocolHandler(w);
            constexpr auto urlPrefix = edge::URL_PREFIX;
#elif defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
            gtk::InstallCustomProtocolHandler(w);
            constexpr auto urlPrefix = gtk::URL_PREFIX;
#else
#error Unsupported platform
#endif

#ifdef _DEBUG
            w.navigate(VITE_DEV_SERVER ? std::format("http://localhost:{}", VITE_DEV_SERVER_PORT) : std::format("{}index.html", urlPrefix));

            if (VITE_DEV_SERVER)
            {
                w.dispatch(
                    [&w]
                    {
                        RunDevToolsWindow(w);
                    });
            }
#else
            w.navigate(std::format("{}index.html", urlPrefix));
#endif
            w.run();
        }
        catch (const webview::exception& e)
        {
            std::cerr << e.what() << '\n';
            return 1;
        }

        return 0;
    }
} // namespace

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#else
int main()
{
#endif

    const auto result = RunMainWindow();

    return result;
}
