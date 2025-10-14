#pragma once

#include "Web/WebViewLib.h"

namespace ui
{
    void NotifyZoneLoadProgress(std::string zoneName, double percentage);
    void NotifyZoneLoaded(std::string zoneName, std::string fastFilePath);
    void NotifyZoneUnloaded(std::string zoneName);

    void RegisterZoneBinds(webview::webview& wv);
} // namespace ui
