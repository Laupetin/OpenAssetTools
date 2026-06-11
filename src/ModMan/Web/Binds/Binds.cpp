#include "Binds.h"

#include "AssetBinds.h"
#include "DialogBinds.h"
#include "UnlinkingBinds.h"
#include "ZoneBinds.h"

namespace ui
{
    void RegisterAllBinds(webview::commands_builder& commands)
    {
        RegisterAssetBinds(commands);
        RegisterDialogHandlerBinds(commands);
        RegisterUnlinkingBinds(commands);
        RegisterZoneBinds(commands);
    }
} // namespace ui
