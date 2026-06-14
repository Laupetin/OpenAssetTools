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

namespace
{
#ifdef _DEBUG
    void SpawnDevToolsWindow()
    {
        con::debug("Creating dev tools window");

        auto& context = ModManContext::Get();

        context.m_dev_tools_webview = std::make_shared<webview::window>();
        auto& newWindow = *context.m_dev_tools_webview;

        newWindow.set_title("Devtools");
        newWindow.set_window_size(640, 480);
        newWindow.set_window_min(480, 320);
        (void)newWindow.navigate(std::format("http://localhost:{}/__devtools__/", VITE_DEV_SERVER_PORT));
    }
#endif

    int SpawnMainWindow()
    {
        con::debug("Creating main window");

        auto& context = ModManContext::Get();
        context.m_main_webview = std::make_shared<webview::window>();
        auto& newWindow = *context.m_main_webview;

#ifdef _DEBUG
        newWindow.set_debug(true);
#endif

        newWindow.set_title("OpenAssetTools ModMan");
        // newWindow.set_window_min(640, 480);
        newWindow.set_window_size(1280, 640);

        newWindow.register_plugin(ui::CreateAssetHandlerPlugin());

        webview::commands_builder commands;
        ui::RegisterAllBinds(commands);
        newWindow.set_commands(commands.build());

#ifdef _DEBUG
        (void)newWindow.navigate(VITE_DEV_SERVER ? std::format("http://localhost:{}", VITE_DEV_SERVER_PORT) : std::format("{}index.html", ui::URL_PREFIX));

        if (VITE_DEV_SERVER)
        {
            newWindow.dispatch(
                []
                {
                    SpawnDevToolsWindow();
                });
        }
#else
        (void)newWindow.navigate(std::format("{}index.html", ui::URL_PREFIX));
#endif

        webview::app app;
        app.register_plugin(std::make_shared<webview::favicon_handler_plugin>());
        app.register_plugin(std::make_shared<webview::title_handler_plugin>());

        (void)app.run(context.m_main_webview);

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

#ifdef __linux__
    g_set_prgname("OpenAssetTools-ModMan");
    g_set_application_name("OpenAssetTools ModMan");
#endif

    con::init();

    ModManArgs args;
    auto shouldContinue = true;
    if (!args.ParseArgs(MODMAN_ARGC, MODMAN_ARGV, shouldContinue))
        return 1;

    if (!shouldContinue)
        return 0;

    con::info("Starting ModMan " GIT_VERSION);

    ModManContext::Get().Startup();

    const auto result = SpawnMainWindow();

    ModManContext::Get().Destroy();

    return result;
}
