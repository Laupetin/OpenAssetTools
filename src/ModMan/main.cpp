#include "GitVersion.h"
#include "Web/Edge/AssetHandlerEdge.h"
#include "Web/Gtk/AssetHandlerGtk.h"
#include "Web/UiCommunication.h"
#include "Web/ViteAssets.h"
#include "Web/WebViewLib.h"

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

    void RunDevToolsWindow()
    {
        con::debug("Creating dev tools window");

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
        con::debug("Creating main window");

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
            ui::Bind<std::string, std::string>(w,
                                               "greet",
                                               [&w](std::string name) -> std::string
                                               {
                                                   ui::Notify(w, "greeting", name);
                                                   return std::format("Hello from C++ {}!", name);
                                               });

            // A binding that counts up or down and immediately returns the new value.
            ui::Bind(w,
                     "debug",
                     []()
                     {
                         con::info("Debug");
                     });

            // A binding that creates a new thread and returns the result at a later time.
            ui::BindAsync(w,
                          "compute",
                          [&](const std::string& id)
                          {
                              // Create a thread and forget about it for the sake of simplicity.
                              std::thread(
                                  [&, id]
                                  {
                                      // Simulate load.
                                      std::this_thread::sleep_for(std::chrono::seconds(5));
                                      ui::PromiseResolve(w, id, 42);
                                  })
                                  .detach();
                          });

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
                    []
                    {
                        RunDevToolsWindow();
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

    con::info("Starting ModMan " GIT_VERSION);

    const auto result = RunMainWindow();

    return result;
}
