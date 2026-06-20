#pragma once

#include "Context/FastFileContext.h"
#include "Web/WebWindowedLib.h"

namespace ui
{
    void NotifyZoneLoadProgress(std::string zoneName, double percentage);
    void NotifyZoneLoaded(const LoadedZoneInformation& loadedZone);
    void NotifyZoneUnloaded(std::string zoneName);

    void RegisterZoneBinds(webwindowed::commands_builder& commands);
} // namespace ui
