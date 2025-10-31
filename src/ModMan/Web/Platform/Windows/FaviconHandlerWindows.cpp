#include "AssetHandlerWindows.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#include "PlatformUtilsWindows.h"
#include "Web/UiAssets.h"

#include <Windows.h>
#include <format>
#include <gdiplus.h>
#include <iostream>
#include <sstream>
#include <webview/detail/backends/win32_edge.hh>
#include <wrl/event.h>

using namespace PLATFORM_NAMESPACE_WINDOWS;

namespace
{
    class UniqueHIcon
    {
    public:
        UniqueHIcon()
            : m_icon(nullptr)
        {
        }

        ~UniqueHIcon()
        {
            if (m_icon)
            {
                DestroyIcon(m_icon);
                m_icon = nullptr;
            }
        }

        UniqueHIcon(const UniqueHIcon& other) = default;

        UniqueHIcon(UniqueHIcon&& other) noexcept
            : m_icon(other.m_icon)
        {
            other.m_icon = nullptr;
        }

        UniqueHIcon& operator=(const UniqueHIcon& other) = default;

        UniqueHIcon& operator=(UniqueHIcon&& other) noexcept
        {
            m_icon = other.m_icon;
            other.m_icon = nullptr;
            return *this;
        }

        HICON& get()
        {
            return m_icon;
        }

    private:
        HICON m_icon;
    };

    std::unordered_map<HWND, UniqueHIcon> icons;

    HRESULT HandleFaviconChanged(ICoreWebView2_15* core15, HWND window)
    {
        LPWSTR url;

        if (!SUCCEEDED(core15->get_FaviconUri(&url)))
        {
            std::cerr << "Failed to get favicon uri\n";
            return S_FALSE;
        }

        const std::wstring strUrl(url);
        CoTaskMemFree(url);

        if (strUrl.empty())
        {
            icons.erase(icons.find(window));
            SendMessage(window, WM_SETICON, ICON_SMALL, (LPARAM)NULL);
        }
        else
        {
            if (!SUCCEEDED(core15->GetFavicon(COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
                                              Microsoft::WRL::Callback<ICoreWebView2GetFaviconCompletedHandler>(
                                                  [window](HRESULT errorCode, IStream* iconStream) -> HRESULT
                                                  {
                                                      Gdiplus::Bitmap iconBitmap(iconStream);
                                                      UniqueHIcon icon;
                                                      if (iconBitmap.GetHICON(&icon.get()) == Gdiplus::Status::Ok)
                                                      {
                                                          SendMessage(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon.get()));
                                                          icons.emplace(window, std::move(icon)).first->second.get();
                                                      }
                                                      else
                                                      {
                                                          icons.erase(icons.find(window));
                                                          SendMessage(window, WM_SETICON, ICON_SMALL, NULL);
                                                      }

                                                      return S_OK;
                                                  })
                                                  .Get())))
            {
                std::cerr << "Failed to get favicon\n";
                return S_FALSE;
            }
        }
        return S_OK;
    }
} // namespace

namespace PLATFORM_NAMESPACE_WINDOWS
{
    void InstallFaviconHandler(webview::webview& wv)
    {
        const auto controller = static_cast<ICoreWebView2Controller*>(wv.browser_controller().value());
        auto window = static_cast<HWND>(wv.window().value());
        Microsoft::WRL::ComPtr<ICoreWebView2> core;
        if (!SUCCEEDED(controller->get_CoreWebView2(&core)))
        {
            std::cerr << "Failed to get webview\n";
            return;
        }

        Microsoft::WRL::ComPtr<ICoreWebView2_15> core15;
        if (!SUCCEEDED(core->QueryInterface(IID_PPV_ARGS(&core15))))
        {
            std::cerr << "Failed to get core15\n";
            return;
        }

        const Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
        ULONG_PTR gdiPlusToken;
        Gdiplus::GdiplusStartup(&gdiPlusToken, &gdiPlusStartupInput, nullptr);
        EventRegistrationToken token;
        if (!SUCCEEDED(core15->add_FaviconChanged(Microsoft::WRL::Callback<ICoreWebView2FaviconChangedEventHandler>(
                                                      [core15, window](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                                      {
                                                          return HandleFaviconChanged(core15.Get(), window);
                                                      })
                                                      .Get(),
                                                  &token)))
        {
            std::cerr << "Failed to add favicon handler\n";
        }
    }
} // namespace PLATFORM_NAMESPACE_WINDOWS

#endif
