#pragma once

#include "Web/WebViewLib.h"

namespace ui
{
    void NotifyZoneUnlinkProgress(std::string zoneName, double percentage);

    void RegisterUnlinkingBinds(webview::webview& wv);
} // namespace ui
