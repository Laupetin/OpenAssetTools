#pragma once

#include "Web/WebWindowedLib.h"

#include <string>

namespace ui
{
    void NotifyZoneUnlinkProgress(std::string zoneName, double percentage);

    void RegisterUnlinkingBinds(webwindowed::commands_builder& commands);
} // namespace ui
