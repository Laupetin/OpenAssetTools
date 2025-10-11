#include "Binds.h"

#include "FastFileBinds.h"
#include "Web/Binds/DialogBinds.h"

namespace ui
{
    void RegisterAllBinds(webview::webview& wv)
    {
        RegisterDialogHandlerBinds(wv);
        RegisterFastFileBinds(wv);
    }
} // namespace ui
