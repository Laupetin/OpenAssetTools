#include "Binds.h"

#include "Web/Binds/DialogBinds.h"

namespace ui
{
    void RegisterAllBinds(webview::webview& wv)
    {
        RegisterDialogHandlerBinds(wv);
    }
} // namespace ui
