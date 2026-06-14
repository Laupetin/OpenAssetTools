#pragma once

#define NOMINMAX

#ifdef _MSC_VER
#pragma warning(push, 0)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <webview/webview.hpp>

// Plugins
#include <webview/plugin/favicon_handler.hpp>
#include <webview/plugin/title_handler.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#ifdef ERROR
#undef ERROR
#endif
