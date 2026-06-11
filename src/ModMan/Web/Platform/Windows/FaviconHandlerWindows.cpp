#include "Web/Platform/FaviconHandler.h"
#include "Web/Platform/Platform.h"

#ifdef PLATFORM_WINDOWS

#include "Web/UiAssets.h"

#include <Windows.h>
#include <format>
#include <gdiplus.h>
#include <iostream>
#include <wrl/event.h>

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

        if (FAILED(core15->get_FaviconUri(&url)))
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
            if (FAILED(core15->GetFavicon(COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
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

    class FaviconPlugin : public webview::plugin
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

            Microsoft::WRL::ComPtr<ICoreWebView2_15> core15;
            if (FAILED(core->QueryInterface(IID_PPV_ARGS(&core15))))
                return std::unexpected("Failed to get core15");

            const Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
            ULONG_PTR gdiPlusToken;
            Gdiplus::GdiplusStartup(&gdiPlusToken, &gdiPlusStartupInput, nullptr);
            EventRegistrationToken token;
            if (FAILED(core15->add_FaviconChanged(Microsoft::WRL::Callback<ICoreWebView2FaviconChangedEventHandler>(
                                                      [core15, windowHwnd](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                                      {
                                                          return HandleFaviconChanged(core15.Get(), windowHwnd);
                                                      })
                                                      .Get(),
                                                  &token)))
            {
                return std::unexpected("Failed to add favicon handler");
            }

            return {};
        }
    };
} // namespace

namespace ui
{
    std::shared_ptr<webview::plugin> CreateFaviconPlugin()
    {
        return std::make_shared<FaviconPlugin>();
    }
} // namespace ui

#endif
