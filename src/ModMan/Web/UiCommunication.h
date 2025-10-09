#pragma once

#include "Utils/Logging/Log.h"
#include "WebViewLib.h"

#pragma warning(push, 0)
#include <nlohmann/json.hpp>
#pragma warning(pop)

namespace ui
{
    inline void Bind(webview::webview& wv, const std::string& name, std::function<void()> fn)
    {
        wv.bind(name,
                [fn](const std::string& req) -> std::string
                {
                    fn();
                    return "";
                });
    }

    template<typename TInput> void Bind(webview::webview& wv, const std::string& name, std::function<void(TInput)> fn)
    {
        wv.bind(name,
                [fn](const std::string& req) -> std::string
                {
                    TInput param;
                    try
                    {
                        const auto json = nlohmann::json::parse(req);
                        if (!json.is_array())
                        {
                            con::error("Webview params are not an array: {}", req);
                            return "";
                        }
                        param = json.at(0).get<TInput>();
                    }
                    catch (const nlohmann::json::exception& e)
                    {
                        con::error("Failed to parse json of webview call: {}", e.what());
                        return "";
                    }

                    fn(std::move(param));
                    return "";
                });
    }

    template<typename TReturn> void BindRetOnly(webview::webview& wv, const std::string& name, std::function<TReturn()> fn)
    {
        wv.bind(name,
                [fn](const std::string& req) -> std::string
                {
                    auto result = fn();

                    return nlohmann::json(result).dump();
                });
    }

    template<typename TInput, typename TReturn> void Bind(webview::webview& wv, const std::string& name, std::function<TReturn(TInput)> fn)
    {
        wv.bind(name,
                [fn](const std::string& req) -> std::string
                {
                    TInput param;
                    try
                    {
                        const auto json = nlohmann::json::parse(req);
                        if (!json.is_array())
                        {
                            con::error("Webview params are not an array: {}", req);
                            return "";
                        }
                        param = json.at(0).get<TInput>();
                    }
                    catch (const nlohmann::json::exception& e)
                    {
                        con::error("Failed to parse json of webview call: {}", e.what());
                        return "";
                    }

                    auto result = fn(std::move(param));
                    return nlohmann::json(result).dump();
                });
    }

    inline void BindAsync(webview::webview& wv, const std::string& name, std::function<void(const std::string& id)> fn)
    {
        wv.bind(
            name,
            [fn](const std::string& id, const std::string& req, void* /* arg */)
            {
                fn(id);
            },
            nullptr);
    }

    template<typename TInput> void BindAsync(webview::webview& wv, const std::string& name, std::function<void(const std::string& id, TInput)> fn)
    {
        wv.bind(
            name,
            [fn](const std::string& id, const std::string& req, void* /* arg */)
            {
                TInput param;
                try
                {
                    const auto json = nlohmann::json::parse(req);
                    if (!json.is_array())
                    {
                        con::error("Webview params are not an array: {}", req);
                        return "";
                    }
                    param = json.at(0).get<TInput>();
                }
                catch (const nlohmann::json::exception& e)
                {
                    con::error("Failed to parse json of webview call: {}", e.what());
                    return "";
                }

                fn(id, std::move(param));
                return "";
            },
            nullptr);
    }

    template<typename TPayload> void PromiseResolve(webview::webview& wv, const std::string& id, const TPayload& payload)
    {
        wv.resolve(id, 0, nlohmann::json(payload).dump());
    }

    template<typename TPayload> void PromiseReject(webview::webview& wv, const std::string& id, const TPayload& payload)
    {
        wv.resolve(id, 1, nlohmann::json(payload).dump());
    }

    template<typename TPayload> void Notify(webview::webview& wv, const std::string& eventKey, const TPayload& payload)
    {
        wv.notify(eventKey, nlohmann::json(payload).dump());
    }
} // namespace ui
