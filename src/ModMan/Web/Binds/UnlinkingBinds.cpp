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

    result::Expected<NoResult, std::string> UnlinkZoneInDbThread(const std::string& zoneName)
    {
        const auto& context = ModManContext::Get().m_fast_file;
        const auto existingZone = std::ranges::find_if(context.m_loaded_zones,
                                                       [&zoneName](const std::unique_ptr<Zone>& zone)
                                                       {
                                                           return zone->m_name == zoneName;
                                                       });

        if (existingZone == context.m_loaded_zones.end())
            return result::Unexpected(std::format("No zone with name {} loaded", zoneName));

        const auto& zone = *existingZone->get();

        const auto* objWriter = IObjWriter::GetObjWriterForGame(zone.m_game_id);

        const auto outputFolderPath = fs::path(utils::GetExecutablePath()).parent_path() / "zone_dump" / zoneName;
        const auto outputFolderPathStr = outputFolderPath.string();

        OutputPathFilesystem outputFolderOutputPath(outputFolderPath);
        SearchPaths searchPaths;
        AssetDumpingContext dumpingContext(zone, outputFolderPathStr, outputFolderOutputPath, searchPaths);
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
