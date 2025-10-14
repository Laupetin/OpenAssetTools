#include "ZoneBinds.h"

#include "Context/ModManContext.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"

namespace
{
    class ZoneLoadProgressDto
    {
    public:
        std::string zoneName;
        double percentage;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneLoadProgressDto, zoneName, percentage);

    class ZoneLoadedDto
    {
    public:
        std::string zoneName;
        std::string filePath;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneLoadedDto, zoneName, filePath);

    class ZoneUnloadedDto
    {
    public:
        std::string zoneName;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneUnloadedDto, zoneName);

    void LoadFastFile(webview::webview& wv, std::string id, std::string path) // NOLINT(performance-unnecessary-value-param) Copy is made for thread safety
    {
        ModManContext::Get().m_db_thread.Dispatch(
            [&wv, id, path]
            {
                auto maybeZone = ModManContext::Get().m_fast_file.LoadFastFile(path);

                if (maybeZone)
                {
                    ui::PromiseResolve(wv,
                                       id,
                                       ZoneLoadedDto{
                                           .zoneName = maybeZone.value()->m_name,
                                           .filePath = path,
                                       });
                    con::debug("Loaded zone \"{}\"", maybeZone.value()->m_name);
                }
                else
                {
                    con::warn("Failed to load zone \"{}\": {}", path, maybeZone.error());
                    ui::PromiseReject(wv, id, std::move(maybeZone).error());
                }
            });
    }

    void UnloadZone(webview::webview& wv, std::string id, std::string zoneName) // NOLINT(performance-unnecessary-value-param) Copy is made for thread safety
    {
        ModManContext::Get().m_db_thread.Dispatch(
            [&wv, id, zoneName]
            {
                auto result = ModManContext::Get().m_fast_file.UnloadZone(zoneName);
                if (result)
                {
                    con::debug("Unloaded zone \"{}\"", zoneName);
                    ui::PromiseResolve(wv, id, true);
                }
                else
                {
                    con::warn("Failed unloading zone {}: {}", zoneName, result.error());
                    ui::PromiseReject(wv, id, std::move(result).error());
                }
            });
    }
} // namespace

namespace ui
{
    void NotifyZoneLoadProgress(std::string zoneName, const double percentage)
    {
        const ZoneLoadProgressDto dto{
            .zoneName = std::move(zoneName),
            .percentage = percentage,
        };
        Notify(*ModManContext::Get().m_main_webview, "zoneLoadProgress", dto);
    }

    void NotifyZoneLoaded(std::string zoneName, std::string fastFilePath)
    {
        const ZoneLoadedDto dto{
            .zoneName = std::move(zoneName),
            .filePath = std::move(fastFilePath),
        };
        Notify(*ModManContext::Get().m_main_webview, "zoneLoaded", dto);
    }

    void NotifyZoneUnloaded(std::string zoneName)
    {
        const ZoneUnloadedDto dto{
            .zoneName = std::move(zoneName),
        };
        Notify(*ModManContext::Get().m_main_webview, "zoneUnloaded", dto);
    }

    void RegisterZoneBinds(webview::webview& wv)
    {
        BindAsync<std::string>(wv,
                               "loadFastFile",
                               [&wv](const std::string& id, std::string path)
                               {
                                   LoadFastFile(wv, id, std::move(path));
                               });

        BindAsync<std::string>(wv,
                               "unloadZone",
                               [&wv](const std::string& id, std::string zoneName)
                               {
                                   UnloadZone(wv, id, std::move(zoneName));
                               });
    }
} // namespace ui
