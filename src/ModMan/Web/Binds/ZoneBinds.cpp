#include "ZoneBinds.h"

#include "Context/ModManContext.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"

NLOHMANN_JSON_SERIALIZE_ENUM(GameId,
                             {
                                 {GameId::IW3, "IW3"},
                                 {GameId::IW4, "IW4"},
                                 {GameId::IW5, "IW5"},
                                 {GameId::T5,  "T5" },
                                 {GameId::T6,  "T6" },
});

NLOHMANN_JSON_SERIALIZE_ENUM(GamePlatform,
                             {
                                 {GamePlatform::PC,   "PC"  },
                                 {GamePlatform::XBOX, "XBOX"},
                                 {GamePlatform::PS3,  "PS3" },
});

namespace
{
    class ZoneDto
    {
    public:
        std::string name;
        std::string filePath;
        GameId game;
        GamePlatform platform;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneDto, name, filePath, game, platform);

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
        ZoneDto zone;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneLoadedDto, zone);

    class ZoneUnloadedDto
    {
    public:
        std::string zoneName;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneUnloadedDto, zoneName);

    ZoneDto CreateZoneDto(const LoadedZone& loadedZone)
    {
        return ZoneDto{
            .name = loadedZone.m_zone->m_name,
            .filePath = loadedZone.m_file_path,
            .game = loadedZone.m_zone->m_game_id,
            .platform = loadedZone.m_zone->m_platform,
        };
    }

    std::vector<ZoneDto> GetLoadedZones()
    {
        auto& context = ModManContext::Get().m_fast_file;

        std::vector<ZoneDto> result;

        {
            std::shared_lock lock(context.m_zone_lock);
            result.reserve(context.m_loaded_zones.size());

            for (const auto& loadedZone : context.m_loaded_zones)
            {
                result.emplace_back(CreateZoneDto(*loadedZone));
            }
        }

        return result;
    }

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
                                           .zone = CreateZoneDto(*maybeZone.value()),
                                       });
                    con::debug("Loaded zone \"{}\"", maybeZone.value()->m_zone->m_name);
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

    void NotifyZoneLoaded(const LoadedZone& loadedZone)
    {
        const ZoneLoadedDto dto{
            .zone = CreateZoneDto(loadedZone),
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
        BindRetOnly<std::vector<ZoneDto>>(wv,
                                          "getZones",
                                          []
                                          {
                                              return GetLoadedZones();
                                          });

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
