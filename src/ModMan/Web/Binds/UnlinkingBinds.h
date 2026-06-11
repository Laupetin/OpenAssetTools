#pragma once

#include "Web/WebViewLib.h"

namespace ui
{
    void NotifyZoneUnlinkProgress(std::string zoneName, double percentage);

    void RegisterUnlinkingBinds(webview::commands_builder& commands);
} // namespace ui
