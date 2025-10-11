#include "Binds.h"

#include "Web/Binds/DialogBinds.h"
#include "ZoneBinds.h"

namespace ui
{
    void RegisterAllBinds(webview::webview& wv)
    {
        RegisterDialogHandlerBinds(wv);
        RegisterZoneBinds(wv);
    }
} // namespace ui
