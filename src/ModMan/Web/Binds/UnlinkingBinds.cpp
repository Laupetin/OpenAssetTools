#include "UnlinkingBinds.h"

#include "Context/ModManContext.h"
#include "IObjWriter.h"
#include "SearchPath/OutputPathFilesystem.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/PathUtils.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace
{
    class ZoneUnlinkProgressDto
    {
    public:
        std::string zoneName;
        double percentage;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneUnlinkProgressDto, zoneName, percentage);

    constexpr double MIN_PROGRESS_TO_REPORT = 0.5;

    class UnlinkingEventProgressReporter : public ProgressCallback
    {
    public:
        explicit UnlinkingEventProgressReporter(std::string zoneName)
            : m_zone_name(std::move(zoneName)),
              m_last_progress(0)
        {
        }

        void OnProgress(const size_t current, const size_t total) override
        {
            const double percentage = static_cast<double>(current) / static_cast<double>(total) * 100.0;

            if (percentage - m_last_progress >= MIN_PROGRESS_TO_REPORT)
            {
                m_last_progress = percentage;
                ui::NotifyZoneUnlinkProgress(m_zone_name, percentage);
            }
        }

    private:
        std::string m_zone_name;
        double m_last_progress;
    };

    result::Expected<NoResult, std::string> UnlinkZoneInDbThread(const std::string& zoneName)
    {
        const auto& context = ModManContext::Get().m_fast_file;
        const auto existingZone = std::ranges::find_if(context.m_loaded_zones,
                                                       [&zoneName](const std::unique_ptr<LoadedZone>& loadedZone)
                                                       {
                                                           return loadedZone->m_zone->m_name == zoneName;
                                                       });

        if (existingZone == context.m_loaded_zones.end())
            return result::Unexpected(std::format("No zone with name {} loaded", zoneName));

        const auto& loadedZone = *existingZone->get();

        const auto* objWriter = IObjWriter::GetObjWriterForGame(loadedZone.m_zone->m_game_id);

        const auto outputFolderPath = fs::path(utils::GetExecutablePath()).parent_path() / "zone_dump" / zoneName;
        const auto outputFolderPathStr = outputFolderPath.string();

        OutputPathFilesystem outputFolderOutputPath(outputFolderPath);
        SearchPaths searchPaths;
        AssetDumpingContext dumpingContext(
            *loadedZone.m_zone, outputFolderPathStr, outputFolderOutputPath, searchPaths, std::make_unique<UnlinkingEventProgressReporter>(zoneName));
        objWriter->DumpZone(dumpingContext);

        return NoResult();
    }

    void UnlinkZone(webview::webview& wv, std::string id, std::string zoneName) // NOLINT(performance-unnecessary-value-param) Copy is made for thread safety
    {
        ModManContext::Get().m_db_thread.Dispatch(
            [&wv, id, zoneName]
            {
                auto result = UnlinkZoneInDbThread(zoneName);

                if (result)
                {
                    con::debug("Unlinked zone \"{}\"", zoneName);
                    ui::PromiseResolve(wv, id, true);
                }
                else
                {
                    con::warn("Failed to unlink zone \"{}\": {}", zoneName, result.error());
                    ui::PromiseReject(wv, id, std::move(result).error());
                }
            });
    }
} // namespace

namespace ui
{
    void NotifyZoneUnlinkProgress(std::string zoneName, const double percentage)
    {
        const ZoneUnlinkProgressDto dto{
            .zoneName = std::move(zoneName),
            .percentage = percentage,
        };
        Notify(*ModManContext::Get().m_main_webview, "zoneUnlinkProgress", dto);
    }

    void RegisterUnlinkingBinds(webview::webview& wv)
    {
        BindAsync<std::string>(wv,
                               "unlinkZone",
                               [&wv](const std::string& id, std::string zoneName)
                               {
                                   UnlinkZone(wv, id, std::move(zoneName));
                               });
    }
} // namespace ui
