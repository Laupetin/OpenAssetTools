#include "Context/ModManContext.h"
#include "GitVersion.h"
#include "ModManArgs.h"
#include "Web/Binds/Binds.h"
#include "Web/Platform/AssetHandler.h"
#include "Web/UiCommunication.h"
#include "Web/ViteAssets.h"
#include "Web/WebViewLib.h"

#include <format>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std::string_literals;
using namespace PLATFORM_NAMESPACE;

namespace
{
#ifdef _DEBUG
    void SpawnDevToolsWindow()
    {
        con::debug("Creating dev tools window");

        auto& context = ModManContext::Get();

        try
        {
            context.m_dev_tools_webview = std::make_unique<webview::webview>(false, nullptr);
            auto& newWindow = *context.m_dev_tools_webview;

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

    int SpawnMainWindow()
    {
        con::debug("Creating main window");

        auto& context = ModManContext::Get();
        try
        {
            context.m_main_webview = std::make_unique<webview::webview>(
#ifdef _DEBUG
                true,
#else
                false,
#endif
                nullptr);
            auto& newWindow = *context.m_main_webview;

            newWindow.set_title("OpenAssetTools ModMan");
            newWindow.set_size(1280, 640, WEBVIEW_HINT_NONE);
            newWindow.set_size(480, 320, WEBVIEW_HINT_MIN);

            InstallAssetHandler(newWindow);
            ui::RegisterAllBinds(newWindow);

#ifdef _DEBUG
            newWindow.navigate(VITE_DEV_SERVER ? std::format("http://localhost:{}", VITE_DEV_SERVER_PORT) : std::format("{}index.html", URL_PREFIX));

            if (VITE_DEV_SERVER)
            {
                newWindow.dispatch(
                    []
                    {
                        SpawnDevToolsWindow();
                    });
            }
#else
            newWindow.navigate(std::format("{}index.html", URL_PREFIX));
#endif
            newWindow.run();
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
#define MODMAN_ARGC __argc
#define MODMAN_ARGV const_cast<const char**>(__argv)
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
#else
#define MODMAN_ARGC argc
#define MODMAN_ARGV argv
int main(int argc, const char** argv)
#endif
{
#ifdef _WIN32
    // Attach console if possible on Windows for stdout/stderr in console
    if (AttachConsole(-1))
    {
        FILE* fDummy;
        (void)freopen_s(&fDummy, "CONOUT$", "w", stdout);
        (void)freopen_s(&fDummy, "CONOUT$", "w", stderr);
        (void)freopen_s(&fDummy, "CONIN$", "r", stdin);
        std::cout.clear();
        std::clog.clear();
        std::cerr.clear();
        std::cin.clear();
    }
#endif

    ModManArgs args;
    auto shouldContinue = true;
    if (!args.ParseArgs(MODMAN_ARGC, MODMAN_ARGV, shouldContinue))
        return false;

    if (!shouldContinue)
        return true;

    con::info("Starting ModMan " GIT_VERSION);

    ModManContext::Get().Startup();

    const auto result = SpawnMainWindow();

    ModManContext::Get().Destroy();

    return result;
}
