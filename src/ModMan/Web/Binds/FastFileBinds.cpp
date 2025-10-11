#include "FastFileBinds.h"

#include "Context/ModManContext.h"
#include "Web/UiCommunication.h"

namespace
{

}

namespace ui
{
    void RegisterFastFileBinds(webview::webview& wv)
    {
        BindAsync<std::string>(wv,
                               "loadFastFile",
                               [&wv](const std::string& id, std::string path)
                               {
                                   std::string idMove(id);
                                   ModManContext::Get().m_db_thread.Dispatch(
                                       [&wv, idMove, path]
                                       {
                                           ModManContext::Get().m_fast_file.LoadFastFile(path);
                                           PromiseResolve(wv, idMove, true);
                                       });
                               });
    }
} // namespace ui
