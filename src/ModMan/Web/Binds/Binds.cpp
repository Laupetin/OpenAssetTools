#include "Binds.h"

#include "UnlinkingBinds.h"
#include "Web/Binds/DialogBinds.h"
#include "ZoneBinds.h"

namespace ui
{
    void RegisterAllBinds(webview::webview& wv)
    {
        RegisterDialogHandlerBinds(wv);
        RegisterUnlinkingBinds(wv);
        RegisterZoneBinds(wv);
    }
} // namespace ui
