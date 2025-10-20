#include "Binds.h"

#include "AssetBinds.h"
#include "DialogBinds.h"
#include "UnlinkingBinds.h"
#include "ZoneBinds.h"

namespace ui
{
    void RegisterAllBinds(webview::webview& wv)
    {
        RegisterAssetBinds(wv);
        RegisterDialogHandlerBinds(wv);
        RegisterUnlinkingBinds(wv);
        RegisterZoneBinds(wv);
    }
} // namespace ui
