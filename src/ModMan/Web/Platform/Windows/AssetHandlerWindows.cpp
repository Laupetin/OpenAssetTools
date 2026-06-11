#include "Web/Platform/AssetHandler.h"
#include "Web/Platform/Platform.h"

#ifdef PLATFORM_WINDOWS

#include "PlatformUtilsWindows.h"
#include "Web/UiAssets.h"

#include <Windows.h>
#include <format>
#include <iostream>
#include <sstream>
#include <wrl/event.h>

namespace
{
    constexpr auto LOCALHOST_PREFIX = "http://localhost:";

    std::wstring HeadersForAssetName(const std::string& assetName, const size_t contentLength)
    {
        std::wstringstream wss;

        wss << std::format(L"Content-Length: {}\n", contentLength);
        wss << L"Content-Type: " << utils::StringToWideString(ui::GetMimeTypeForFileName(assetName));

        return wss.str();
    }

    class AssetHandlerPlugin : public webview::plugin
    {
    public:
        AssetHandlerPlugin()
            : m_ui_file_lookup(ui::BuildUiFileLookup())
        {
        }

        std::expected<void, std::string> on_setup_window(webview::window& window, const webview::plugin_window_context& context) override
        {
            const auto maybeBrowserController = context.browser_controller();
            if (!maybeBrowserController.has_value())
                return std::unexpected("Failed to get browser controller");

            const auto controller = static_cast<ICoreWebView2Controller*>(maybeBrowserController.value());
            Microsoft::WRL::ComPtr<ICoreWebView2> core;
            if (FAILED(controller->get_CoreWebView2(&core)))
                return std::unexpected("Failed to get webview");

            Microsoft::WRL::ComPtr<ICoreWebView2_22> core22;
            if (FAILED(core->QueryInterface(IID_PPV_ARGS(&core22))))
                return std::unexpected("Failed to get core22");

            if (FAILED(core22->AddWebResourceRequestedFilterWithRequestSourceKinds(
                    L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL, COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_ALL)))
            {
                return std::unexpected("Failed to install request filter");
            }

            EventRegistrationToken token;
            if (FAILED(core->add_WebResourceRequested(Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                                                          [core22, this](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                                          {
                                                              return HandleResourceRequested(core22.Get(), args);
                                                          })
                                                          .Get(),
                                                      &token)))
            {
                return std::unexpected("Failed to add resource requested filter");
            }

            return {};
        }

    private:
        HRESULT HandleResourceRequested(ICoreWebView2_22* core22, IUnknown* args)
        {
            Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequestedEventArgs2> webResourceRequestArgs;
            if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&webResourceRequestArgs))))
            {
                COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS requestSourceKind = COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_ALL;
                if (FAILED(webResourceRequestArgs->get_RequestedSourceKind(&requestSourceKind)))
                {
                    std::cerr << "Failed to get requested source kind\n";
                    return S_FALSE;
                }

                Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequest> request;
                if (FAILED(webResourceRequestArgs->get_Request(&request)))
                {
                    std::cerr << "Failed to get request\n";
                    return S_FALSE;
                }

                LPWSTR wUri;
                if (FAILED(request->get_Uri(&wUri)))
                {
                    std::cerr << "Failed to get uri\n";
                    return S_FALSE;
                }

                Microsoft::WRL::ComPtr<ICoreWebView2Environment> environment;
                if (FAILED(core22->get_Environment(&environment)))
                {
                    std::cerr << "Failed to get environment\n";
                    return S_FALSE;
                }

                Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;

                const auto uri = utils::WideStringToString(wUri);
                bool fileFound = false;

#ifdef _DEBUG
                // Allow dev server access
                if (uri.starts_with(LOCALHOST_PREFIX))
                    return S_OK;
#endif

                if (uri.starts_with(ui::URL_PREFIX))
                {
                    const auto asset = uri.substr(std::char_traits<char>::length(ui::URL_PREFIX) - 1);

                    const auto foundUiFile = m_ui_file_lookup.find(asset);
                    if (foundUiFile != m_ui_file_lookup.end())
                    {
                        const Microsoft::WRL::ComPtr<IStream> responseStream =
                            SHCreateMemStream(static_cast<const BYTE*>(foundUiFile->second.data), static_cast<UINT>(foundUiFile->second.dataSize));

                        const auto headers = HeadersForAssetName(asset, foundUiFile->second.dataSize);
                        if (FAILED(environment->CreateWebResourceResponse(responseStream.Get(), 200, L"OK", headers.data(), &response)))
                        {
                            std::cerr << "Failed to create web resource\n";
                            return S_FALSE;
                        }

                        fileFound = true;
                    }
                }

                if (!fileFound)
                {
                    if (FAILED(environment->CreateWebResourceResponse(nullptr, 404, L"Not found", L"", &response)))
                    {
                        std::cerr << "Failed to create web resource\n";
                        return S_FALSE;
                    }
                }

                if (FAILED(webResourceRequestArgs->put_Response(response.Get())))
                {
                    std::cerr << "Failed to put response\n";
                    return S_FALSE;
                }

                return S_OK;
            }

            return S_FALSE;
        }

        std::unordered_map<std::string, UiFile> m_ui_file_lookup;
    };
} // namespace

namespace ui
{
    std::shared_ptr<webview::plugin> CreateAssetHandlerPlugin()
    {
        return std::make_shared<AssetHandlerPlugin>();
    }
} // namespace ui

#endif
