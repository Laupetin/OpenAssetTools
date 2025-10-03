#include "CustomProtocolHandlerEdge.h"

#pragma warning(push, 0)
#include <webview/macros.h>
#include <webview/webview.h>
#pragma warning(pop)

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#include "ui/modmanui.h"

#include <Windows.h>
#include <format>
#include <iostream>
#include <sstream>
#include <webview/detail/backends/win32_edge.hh>
#include <wrl/event.h>

namespace
{
    constexpr auto MOD_MAN_URL_PREFIX = "http://modman/";

    std::string wide_string_to_string(const std::wstring& wide_string)
    {
        if (wide_string.empty())
        {
            return "";
        }

        const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0)
        {
            throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
        }

        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), result.data(), size_needed, nullptr, nullptr);
        return result;
    }

    std::wstring HeadersForAssetName(const std::string& assetName, const size_t contentLength)
    {
        std::wstringstream wss;

        wss << std::format(L"Content-Length: {}\n", contentLength);

        if (assetName.ends_with(".html"))
        {
            wss << L"Content-Type: text/html\n";
        }
        else if (assetName.ends_with(".js"))
        {
            wss << L"Content-Type: text/javascript\n";
        }
        else if (assetName.ends_with(".css"))
        {
            wss << L"Content-Type: text/css\n";
        }

        return wss.str();
    }
} // namespace

namespace edge
{
    void InstallCustomProtocolHandler(webview::webview& wv)
    {
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
        core->add_WebResourceRequested(Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                                           [core22](ICoreWebView2* sender, IUnknown* args) -> HRESULT
                                           {
                                               Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequestedEventArgs2> webResourceRequestArgs;
                                               if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&webResourceRequestArgs))))
                                               {
                                                   COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS requestSourceKind =
                                                       COREWEBVIEW2_WEB_RESOURCE_REQUEST_SOURCE_KINDS_ALL;
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

                                                   const auto uri = wide_string_to_string(wUri);
                                                   bool fileFound = false;
                                                   if (uri.starts_with(MOD_MAN_URL_PREFIX))
                                                   {
                                                       const auto asset = uri.substr(std::char_traits<char>::length(MOD_MAN_URL_PREFIX));

                                                       const auto foundUiFile = MOD_MAN_UI_FILES.find(asset);
                                                       if (foundUiFile != MOD_MAN_UI_FILES.end())
                                                       {
                                                           Microsoft::WRL::ComPtr<IStream> response_stream = SHCreateMemStream(
                                                               static_cast<const BYTE*>(foundUiFile->second.data), foundUiFile->second.dataSize);

                                                           const auto headers = HeadersForAssetName(asset, foundUiFile->second.dataSize);
                                                           environment->CreateWebResourceResponse(response_stream.Get(), 200, L"OK", headers.data(), &response);
                                                           fileFound = true;
                                                       }
                                                   }

                                                   if (!fileFound)
                                                   {
                                                       environment->CreateWebResourceResponse(nullptr, 404, L"Not found", L"", &response);
                                                   }

                                                   webResourceRequestArgs->put_Response(response.Get());
                                                   return S_OK;
                                               }

                                               return S_FALSE;
                                           })
                                           .Get(),
                                       &token);
    }
} // namespace edge

#endif
