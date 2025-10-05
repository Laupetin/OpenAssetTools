#include "CustomProtocolHandlerEdge.h"

#pragma warning(push, 0)
#include <webview/macros.h>
#include <webview/webview.h>
#pragma warning(pop)

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#include "Web/UiAssets.h"

#include <Windows.h>
#include <format>
#include <iostream>
#include <sstream>
#include <webview/detail/backends/win32_edge.hh>
#include <wrl/event.h>

namespace
{
    std::unordered_map<std::string, UiFile> assetLookup;

    std::string WideStringToString(const std::wstring& wideString)
    {
        if (wideString.empty())
            return "";

        const auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), nullptr, 0, nullptr, nullptr);
        if (sizeNeeded <= 0)
            throw std::runtime_error(std::format("WideCharToMultiByte() failed: {}", sizeNeeded));

        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), result.data(), sizeNeeded, nullptr, nullptr);
        return result;
    }

    std::wstring StringToWideString(const std::string& string)
    {
        if (string.empty())
            return L"";

        const auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
        if (sizeNeeded <= 0)
            throw std::runtime_error(std::format("MultiByteToWideChar() failed: {}", sizeNeeded));

        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), result.data(), sizeNeeded);
        return result;
    }

    std::wstring HeadersForAssetName(const std::string& assetName, const size_t contentLength)
    {
        std::wstringstream wss;

        wss << std::format(L"Content-Length: {}\n", contentLength);
        wss << L"Content-Type: " << StringToWideString(GetMimeTypeForFileName(assetName));

        return wss.str();
    }

    HRESULT HandleResourceRequested(ICoreWebView2_22* core22, IUnknown* args)
    {
        Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequestedEventArgs2> webResourceRequestArgs;
        if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&webResourceRequestArgs))))
        {
            COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS requestSourceKind = COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_ALL;
            if (!SUCCEEDED(webResourceRequestArgs->get_RequestedSourceKind(&requestSourceKind)))
            {
                std::cerr << "Failed to get requested source kind\n";
                return S_FALSE;
            }

            Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequest> request;
            if (!SUCCEEDED(webResourceRequestArgs->get_Request(&request)))
            {
                std::cerr << "Failed to get request\n";
                return S_FALSE;
            }

            LPWSTR wUri;
            if (!SUCCEEDED(request->get_Uri(&wUri)))
            {
                std::cerr << "Failed to get uri\n";
                return S_FALSE;
            }

            Microsoft::WRL::ComPtr<ICoreWebView2Environment> environment;
            if (!SUCCEEDED(core22->get_Environment(&environment)))
            {
                std::cerr << "Failed to get environment\n";
                return S_FALSE;
            }

            Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;

            const auto uri = WideStringToString(wUri);
            bool fileFound = false;
            if (uri.starts_with(edge::URL_PREFIX))
            {
                const auto asset = uri.substr(std::char_traits<char>::length(edge::URL_PREFIX) - 1);

                const auto foundUiFile = assetLookup.find(asset);
                if (foundUiFile != assetLookup.end())
                {
                    const Microsoft::WRL::ComPtr<IStream> responseStream =
                        SHCreateMemStream(static_cast<const BYTE*>(foundUiFile->second.data), foundUiFile->second.dataSize);

                    const auto headers = HeadersForAssetName(asset, foundUiFile->second.dataSize);
                    if (!SUCCEEDED(environment->CreateWebResourceResponse(responseStream.Get(), 200, L"OK", headers.data(), &response)))
                    {
                        std::cerr << "Failed to create web resource\n";
                        return S_FALSE;
                    }

                    fileFound = true;
                }
            }

            if (!fileFound)
            {
                if (!SUCCEEDED(environment->CreateWebResourceResponse(nullptr, 404, L"Not found", L"", &response)))
                {
                    std::cerr << "Failed to create web resource\n";
                    return S_FALSE;
                }
            }

            if (!SUCCEEDED(webResourceRequestArgs->put_Response(response.Get())))
            {
                std::cerr << "Failed to put response\n";
                return S_FALSE;
            }

            return S_OK;
        }

        return S_FALSE;
    }
} // namespace

namespace edge
{
    void InstallCustomProtocolHandler(webview::webview& wv)
    {
        assetLookup = BuildUiFileLookup();

        const auto controller = static_cast<ICoreWebView2Controller*>(wv.browser_controller().value());
        Microsoft::WRL::ComPtr<ICoreWebView2> core;
        if (!SUCCEEDED(controller->get_CoreWebView2(&core)))
        {
            std::cerr << "Failed to get webview\n";
            return;
        }

        Microsoft::WRL::ComPtr<ICoreWebView2_22> core22;
        if (!SUCCEEDED(core->QueryInterface(IID_PPV_ARGS(&core22))))
        {
            std::cerr << "Failed to get core22\n";
            return;
        }

        if (!SUCCEEDED(core22->AddWebResourceRequestedFilterWithRequestSourceKinds(
                L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL, COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_ALL)))
        {
            std::cerr << "Failed to install request filter\n";
            return;
        }

        EventRegistrationToken token;
        if (!SUCCEEDED(core->add_WebResourceRequested(Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                                                          [core22](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                                          {
                                                              return HandleResourceRequested(core22.Get(), args);
                                                          })
                                                          .Get(),
                                                      &token)))
        {
            std::cerr << "Failed to add resource requested filter\n";
        }
    }
} // namespace edge

#endif
