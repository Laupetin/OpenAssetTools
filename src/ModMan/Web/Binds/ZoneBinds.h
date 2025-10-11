#pragma once

#include "Web/WebViewLib.h"

namespace ui
{
    void NotifyZoneLoaded(std::string zoneName, std::string fastFilePath);
    void NotifyZoneUnloaded(std::string zoneName);

    void RegisterZoneBinds(webview::webview& wv);
} // namespace ui
