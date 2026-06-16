#pragma once

#define NOMINMAX

#ifdef _MSC_VER
#pragma warning(push, 0)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <webwindowed/webwindowed.hpp>

// Plugins
#include <webwindowed/plugin/asset_handler.hpp>
#include <webwindowed/plugin/favicon_handler.hpp>
#include <webwindowed/plugin/title_handler.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#ifdef ERROR
#undef ERROR
#endif
