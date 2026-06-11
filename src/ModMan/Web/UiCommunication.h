#pragma once

#include "Utils/Logging/Log.h"
#include "WebViewLib.h"

#pragma warning(push, 0)
#include <nlohmann/json.hpp>
#pragma warning(pop)

namespace ui
{
    inline void Bind(webview::commands_builder& commands, const std::string& name, std::function<void(webview::detail::window_base& calling_window)> fn)
    {
        commands.add_command_void(name,
                                  [fn2 = std::move(fn)](webview::detail::window_base& calling_window, std::string message_json_str) -> std::string
                                  {
                                      fn2(calling_window);
                                      return "";
                                  });
    }

    template<typename TInput>
    void Bind(webview::commands_builder& commands, const std::string& name, std::function<void(webview::detail::window_base& calling_window, TInput)> fn)
    {
        commands.add_command_void(name,
                                  [fn2 = std::move(fn)](webview::detail::window_base& calling_window, std::string message_json_str) -> std::string
                                  {
                                      TInput param;
                                      try
                                      {
                                          const auto json = nlohmann::json::parse(message_json_str);
                                          if (!json.is_array())
                                          {
                                              con::error("Webview params are not an array: {}", message_json_str);
                                              return "";
                                          }

                                          if (json.empty())
                                              param = nlohmann::json().get<TInput>();
                                          else
                                              param = json.at(0).get<TInput>();
                                      }
                                      catch (const nlohmann::json::exception& e)
                                      {
                                          con::error("Failed to parse json of webview call: {}", e.what());
                                          return "";
                                      }

                                      fn2(calling_window, std::move(param));
                                      return "";
                                  });
    }

    template<typename TReturn>
    void BindRetOnly(webview::commands_builder& commands, const std::string& name, std::function<TReturn(webview::detail::window_base& calling_window)> fn)
    {
        commands.add_command_sync(name,
                                  [fn2 = std::move(fn)](webview::detail::window_base& calling_window, std::string message_json_str) -> std::string
                                  {
                                      auto result = fn2(calling_window);

                                      return nlohmann::json(result).dump();
                                  });
    }

    template<typename TInput, typename TReturn>
    void Bind(webview::commands_builder& commands, const std::string& name, std::function<TReturn(webview::detail::window_base& calling_window, TInput)> fn)
    {
        commands.add_command_sync(name,
                                  [fn2 = std::move(fn)](webview::detail::window_base& calling_window, std::string message_json_str) -> std::string
                                  {
                                      TInput param;
                                      try
                                      {
                                          const auto json = nlohmann::json::parse(message_json_str);
                                          if (!json.is_array())
                                          {
                                              con::error("Webview params are not an array: {}", message_json_str);
                                              return "";
                                          }

                                          if (json.empty())
                                              param = nlohmann::json().get<TInput>();
                                          else
                                              param = json.at(0).get<TInput>();
                                      }
                                      catch (const nlohmann::json::exception& e)
                                      {
                                          con::error("Failed to parse json of webview call: {}", e.what());
                                          return "";
                                      }

                                      auto result = fn2(calling_window, std::move(param));
                                      return nlohmann::json(result).dump();
                                  });
    }

    inline void BindAsync(webview::commands_builder& commands,
                          const std::string& name,
                          std::function<void(const std::string& id, webview::detail::window_base& calling_window)> fn)
    {
        commands.add_command_async(name,
                                   [fn2 = std::move(fn)](std::string promise_id, webview::detail::window_base& calling_window, std::string message_json_str)
                                   {
                                       fn2(promise_id, calling_window);
                                   });
    }

    template<typename TInput>
    void BindAsync(webview::commands_builder& commands,
                   const std::string& name,
                   std::function<void(const std::string& id, webview::detail::window_base& calling_window, TInput)> fn)
    {
        commands.add_command_async(name,
                                   [fn2 = std::move(fn)](std::string promise_id, webview::detail::window_base& calling_window, std::string message_json_str)
                                   {
                                       TInput param;
                                       try
                                       {
                                           const auto json = nlohmann::json::parse(message_json_str);
                                           if (!json.is_array())
                                           {
                                               con::error("Webview params are not an array: {}", message_json_str);
                                               return "";
                                           }

                                           if (json.empty())
                                               param = nlohmann::json().get<TInput>();
                                           else
                                               param = json.at(0).get<TInput>();
                                       }
                                       catch (const nlohmann::json::exception& e)
                                       {
                                           con::error("Failed to parse json of webview call: {}", e.what());
                                           return "";
                                       }

                                       fn2(promise_id, calling_window, std::move(param));
                                       return "";
                                   });
    }

    template<typename TPayload> void PromiseResolve(webview::detail::window_base& window, const std::string& id, const TPayload& payload)
    {
        window.promise_resolve(id, nlohmann::json(payload).dump());
    }

    template<typename TPayload> void PromiseReject(webview::detail::window_base& window, const std::string& id, const TPayload& payload)
    {
        window.promise_reject(id, nlohmann::json(payload).dump());
    }

    template<typename TPayload> void Notify(webview::detail::window_base& window, const std::string& eventKey, const TPayload& payload)
    {
        window.notify(eventKey, nlohmann::json(payload).dump());
    }
} // namespace ui
