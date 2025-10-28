#pragma once

#include "Context/FastFileContext.h"
#include "Web/WebViewLib.h"

namespace ui
{
    void NotifyZoneLoadProgress(std::string zoneName, double percentage);
    void NotifyZoneLoaded(const LoadedZone& loadedZone);
    void NotifyZoneUnloaded(std::string zoneName);

    void RegisterZoneBinds(webview::webview& wv);
} // namespace ui
