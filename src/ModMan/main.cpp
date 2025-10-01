#include "ui/modmanui.h"

#pragma warning(push, 0)
#include "webview/webview.h"
#pragma warning(pop)

#include <chrono>
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
        w.set_title("Basic Example");
        w.set_size(480, 320, WEBVIEW_HINT_NONE);

        // A binding that counts up or down and immediately returns the new value.
        w.bind("count",
               [&](const std::string& req) -> std::string
               {
                   // Imagine that req is properly parsed or use your own JSON parser.
                   auto direction = std::stol(req.substr(1, req.size() - 1));
                   return std::to_string(count += direction);
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

        w.set_html(std::string(reinterpret_cast<const char*>(INDEX_HTML), std::extent_v<decltype(INDEX_HTML)>));
        w.run();
    }
    catch (const webview::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
