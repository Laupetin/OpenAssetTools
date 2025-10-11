#include "FastFileBinds.h"

#include "Context/ModManContext.h"
#include "Web/UiCommunication.h"

namespace
{
    void LoadFastFile(webview::webview& wv, std::string id, std::string path) // NOLINT(performance-unnecessary-value-param) Copy is made for thread safety
    {
        ModManContext::Get().m_db_thread.Dispatch(
            [&wv, id, path]
            {
                auto maybeZone = ModManContext::Get().m_fast_file.LoadFastFile(path);

                if (maybeZone)
                {
                    ui::PromiseResolve(wv, id, true);
                    con::debug("Loaded zone \"{}\"", maybeZone.value()->m_name);
                }
                else
                {
                    con::warn("Failed to load zone \"{}\": {}", path, maybeZone.error());
                    ui::PromiseReject(wv, id, std::move(maybeZone).error());
                }
            });
    }
} // namespace

namespace ui
{
    void RegisterFastFileBinds(webview::webview& wv)
    {
        BindAsync<std::string>(wv,
                               "loadFastFile",
                               [&wv](const std::string& id, std::string path)
                               {
                                   LoadFastFile(wv, id, std::move(path));
                               });
    }
} // namespace ui
