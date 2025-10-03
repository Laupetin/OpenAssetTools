#pragma warning(push, 0)
#include "webview/webview.h"
#pragma warning(pop)

#include "webview/edge/CustomProtocolHandlerEdge.h"
#include "webview/gtk/CustomProtocolHandlerGtk.h"

#include <chrono>
#include <format>
#include <iostream>
#include <thread>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#else
int main()
{
#endif

    try
    {
        long count = 0;
        webview::webview w(true, nullptr);
        w.set_title("OpenAssetTools ModMan");
        w.set_size(480, 320, WEBVIEW_HINT_NONE);
        w.set_size(480, 320, WEBVIEW_HINT_MIN);

        // A binding that counts up or down and immediately returns the new value.
        w.bind("greet",
               [&](const std::string& req) -> std::string
               {
                   const auto name = req.substr(2, req.size() - 4);
                   return std::format("\"Hello from C++ {}!\"", name);
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

        edge::InstallCustomProtocolHandler(w);

        w.navigate("http://modman/index.html");
        w.run();
    }
    catch (const webview::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
