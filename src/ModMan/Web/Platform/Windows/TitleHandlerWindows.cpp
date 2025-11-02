#include "Web/Platform/Platform.h"
#include "Web/Platform/TitleHandler.h"

#ifdef PLATFORM_WINDOWS

#include "PlatformUtilsWindows.h"
#include "Web/UiAssets.h"

#include <Windows.h>
#include <format>
#include <iostream>
#include <sstream>
#include <webview/detail/backends/win32_edge.hh>
#include <wrl/event.h>

namespace
{
    HRESULT HandleTitleChanged(ICoreWebView2* core, HWND window)
    {
        LPWSTR title;

        if (!SUCCEEDED(core->get_DocumentTitle(&title)))
        {
            std::cerr << "Failed to get title\n";
            return S_FALSE;
        }

        SetWindowTextW(window, title);
        CoTaskMemFree(title);

        return S_OK;
    }
} // namespace

namespace ui
{
    void InstallTitleHandler(webview::webview& wv)
    {
        const auto controller = static_cast<ICoreWebView2Controller*>(wv.browser_controller().value());
        auto window = static_cast<HWND>(wv.window().value());
        Microsoft::WRL::ComPtr<ICoreWebView2> core;
        if (!SUCCEEDED(controller->get_CoreWebView2(&core)))
        {
            std::cerr << "Failed to get webview\n";
            return;
        }

        EventRegistrationToken token;
        if (!SUCCEEDED(core->add_DocumentTitleChanged(Microsoft::WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
                                                          [window](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                                          {
                                                              return HandleTitleChanged(sender, window);
                                                          })
                                                          .Get(),
                                                      &token)))
        {
            std::cerr << "Failed to add title handler\n";
        }
    }
} // namespace ui

#endif
