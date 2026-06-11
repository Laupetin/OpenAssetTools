#include "Web/Platform/Platform.h"
#include "Web/Platform/TitleHandler.h"

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <format>
#include <iostream>
#include <sstream>
#include <wrl/event.h>

namespace
{
    HRESULT HandleTitleChanged(ICoreWebView2* core, HWND window)
    {
        LPWSTR title;

        if (FAILED(core->get_DocumentTitle(&title)))
        {
            std::cerr << "Failed to get title\n";
            return S_FALSE;
        }

        SetWindowTextW(window, title);
        CoTaskMemFree(title);

        return S_OK;
    }

    class TitleHandlerPlugin : public webview::plugin
    {
    public:
        std::expected<void, std::string> on_setup_window(webview::window& window, const webview::plugin_window_context& context) override
        {
            const auto maybeBrowserController = context.browser_controller();
            if (!maybeBrowserController.has_value())
                return std::unexpected("Failed to get browser controller");
            const auto controller = static_cast<ICoreWebView2Controller*>(maybeBrowserController.value());

            const auto maybeWindow = context.window();
            if (!maybeWindow.has_value())
                return std::unexpected("Failed to get window");
            auto windowHwnd = static_cast<HWND>(maybeWindow.value());

            Microsoft::WRL::ComPtr<ICoreWebView2> core;
            if (FAILED(controller->get_CoreWebView2(&core)))
                return std::unexpected("Failed to get webview");

            EventRegistrationToken token;
            if (FAILED(core->add_DocumentTitleChanged(Microsoft::WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
                                                          [windowHwnd](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                                          {
                                                              return HandleTitleChanged(sender, windowHwnd);
                                                          })
                                                          .Get(),
                                                      &token)))
            {
                return std::unexpected("Failed to add title handler");
            }

            return {};
        }
    };
} // namespace

namespace ui
{
    std::shared_ptr<webview::plugin> CreateTitleHandlerPlugin()
    {
        return std::make_shared<TitleHandlerPlugin>();
    }
} // namespace ui

#endif
