#include "Unlinker.h"

#include "ContentLister/ContentPrinter.h"
#include "Game/AutoSearchPaths.h"
#include "IObjLoader.h"
#include "ObjWriter.h"
#include "ObjWriting.h"
#include "SearchPath/OutputPathFilesystem.h"
#include "SearchPath/SharedSearchPaths.h"
#include "UnlinkerArgs.h"
#include "Utils/Logging/Log.h"
#include "Zone/Definition/ZoneDefWriter.h"
#include "Zone/LoadedZoneInformation.h"
#include "ZoneLoading.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>

namespace fs = std::filesystem;

namespace
{
    class ReferencedSearchPaths
    {
    public:
        ReferencedSearchPaths()
            : m_shared_search_paths(nullptr)
        {
        }

        ReferencedSearchPaths(SharedSearchPaths& sharedSearchPaths, std::vector<std::string> searchPaths)
            : m_shared_search_paths(&sharedSearchPaths),
              m_search_paths(std::move(searchPaths))
        {
        }

        ~ReferencedSearchPaths()
        {
            Clear();
        }

        ReferencedSearchPaths(const ReferencedSearchPaths& other) = delete;

        ReferencedSearchPaths(ReferencedSearchPaths&& other) noexcept
            : m_shared_search_paths(other.m_shared_search_paths),
              m_search_paths(std::move(other.m_search_paths))
        {
            other.m_shared_search_paths = nullptr;
        }

        ReferencedSearchPaths& operator=(const ReferencedSearchPaths& other) = delete;

        ReferencedSearchPaths& operator=(ReferencedSearchPaths&& other) noexcept
        {
            m_shared_search_paths = other.m_shared_search_paths;
            other.m_shared_search_paths = nullptr;
            m_search_paths = std::move(other.m_search_paths);

            return *this;
        }

        void Clear()
        {
            if (m_shared_search_paths)
            {
                for (const auto& searchPath : m_search_paths)
                    m_shared_search_paths->UnrefSearchPath(searchPath);
            }
            m_shared_search_paths = nullptr;
        }

    private:
        SharedSearchPaths* m_shared_search_paths;
        std::vector<std::string> m_search_paths;
    };

    [[nodiscard]] std::vector<std::string> GetSearchPathsForZone(const std::string& zonePath, const GameId gameId)
    {
        return AutoSearchPaths::GetForGame(gameId)->GetSearchPathsForZonePath(zonePath);
    }

    class UnlinkerImpl : public Unlinker
    {
    public:
        explicit UnlinkerImpl(UnlinkerArgs args)
            : m_args(std::move(args))
        {
        }

        bool Start() override
        {
            SharedSearchPaths paths;
            for (const auto& userPath : m_args.m_user_search_paths)
                paths.RefSearchPath(userPath);

            if (!LoadZones(paths))
                return false;

            const auto result = UnlinkZones(paths);

            UnloadZones(paths);

            paths.Clear();

            Summarize(result);

            return result;
        }

    private:
        [[nodiscard]] bool ShouldLoadObj() const
        {
            return m_args.m_task != UnlinkerArgs::ProcessingTask::LIST && !m_args.m_skip_obj;
        }

        [[nodiscard]] bool WriteZoneDefinitionFile(const Zone& zone, const fs::path& zoneDefinitionFileFolder) const
        {
            auto zoneDefinitionFilePath(zoneDefinitionFileFolder);
            zoneDefinitionFilePath.append(zone.m_name);
            zoneDefinitionFilePath.replace_extension(".zone");

            std::ofstream zoneDefinitionFile(zoneDefinitionFilePath, std::fstream::out | std::fstream::binary);
            if (!zoneDefinitionFile.is_open())
            {
                con::error("Failed to open file for zone definition file of zone \"{}\".", zone.m_name);
                return false;
            }

            const auto* zoneDefWriter = IZoneDefWriter::GetZoneDefWriterForGame(zone.m_game_id);
            zoneDefWriter->WriteZoneDef(zoneDefinitionFile, zone, m_args.m_use_gdt, m_args.m_minimal_zone_def);

            zoneDefinitionFile.close();

            return true;
        }

        static bool OpenGdtFile(const Zone& zone, const fs::path& outputFolder, std::ofstream& stream)
        {
            auto gdtFilePath(outputFolder);
            gdtFilePath.append("source_data");

            fs::create_directories(gdtFilePath);

            gdtFilePath.append(zone.m_name);
            gdtFilePath.replace_extension(".gdt");

            stream = std::ofstream(gdtFilePath, std::fstream::out | std::fstream::binary);
            if (!stream.is_open())
            {
                con::error("Failed to open file for zone definition file of zone \"{}\".", zone.m_name);
                return false;
            }

            return true;
        }

        void UpdateAssetIncludesAndExcludes(const AssetDumpingContext& context) const
        {
            const auto gameId = context.m_zone.m_game_id;
            const auto* game = IGame::GetGameById(gameId);
            const auto assetTypeCount = game->GetAssetTypeCount();

            const auto initialValue = m_args.m_asset_type_handling == UnlinkerArgs::AssetTypeHandling::EXCLUDE;
            ObjWriting::Configuration.AssetTypesToHandleBitfield = std::vector(assetTypeCount, initialValue);

            std::vector<bool> handledSpecifiedAssets(m_args.m_specified_asset_types.size());
            auto anySpecifiedValueInvalid = false;
            for (const auto& specifiedValue : m_args.m_specified_asset_types)
            {
                const auto maybeAssetType = game->FindAssetTypeByName(specifiedValue);
                if (!maybeAssetType)
                {
                    con::error("Unknown asset type \"{}\"", specifiedValue);
                    anySpecifiedValueInvalid = true;
                    continue;
                }

                ObjWriting::Configuration.AssetTypesToHandleBitfield[*maybeAssetType] = !initialValue;
            }

            if (anySpecifiedValueInvalid)
            {
                con::error("Valid asset types are:");

                auto first = true;
                std::ostringstream ss;
                for (auto i = 0u; i < assetTypeCount; i++)
                {
                    const auto assetTypeName = std::string(*game->GetAssetTypeName(i));

                    if (first)
                        first = false;
                    else
                        ss << ", ";
                    ss << assetTypeName;
                }
                con::error(ss.str());
            }
        }

        /**
         * \brief Performs the tasks specified by the command line arguments on the specified zone.
         * \param searchPath The search path for obj data.
         * \param zone The zone to handle.
         * \return \c true if handling the zone was successful, otherwise \c false
         */
        bool HandleZone(ISearchPath& searchPath, Zone& zone) const
        {
            if (m_args.m_task == UnlinkerArgs::ProcessingTask::LIST)
            {
                const ContentPrinter printer(zone);
                printer.PrintContent();
            }
            else if (m_args.m_task == UnlinkerArgs::ProcessingTask::DUMP)
            {
                const auto outputFolderPathStr = m_args.GetOutputFolderPathForZone(zone);
                const fs::path outputFolderPath(outputFolderPathStr);
                fs::create_directories(outputFolderPath);

                fs::path zoneDefinitionFileFolder(outputFolderPath);
                zoneDefinitionFileFolder.append("zone_source");
                fs::create_directories(zoneDefinitionFileFolder);

                if (!WriteZoneDefinitionFile(zone, zoneDefinitionFileFolder))
                    return false;

                OutputPathFilesystem outputFolderOutputPath(outputFolderPath);
                AssetDumpingContext context(zone, outputFolderPathStr, outputFolderOutputPath, searchPath, std::nullopt);

                std::ofstream gdtStream;
                if (m_args.m_use_gdt)
                {
                    if (!OpenGdtFile(zone, outputFolderPath, gdtStream))
                        return false;
                    auto gdt = std::make_unique<GdtOutputStream>(gdtStream);
                    gdt->BeginStream();

                    const auto* game = IGame::GetGameById(zone.m_game_id);
                    gdt->WriteVersion(GdtVersion(game->GetShortName(), 1));

                    context.m_gdt = std::move(gdt);
                }

                UpdateAssetIncludesAndExcludes(context);

                auto* objWriter = IObjWriter::GetObjWriterForGame(zone.m_game_id);

                auto result = objWriter->DumpZone(context);

                if (m_args.m_use_gdt)
                {
                    context.m_gdt->EndStream();
                    gdtStream.close();
                }

                if (!result)
                {
                    con::error("Dumping zone failed!");
                    return false;
                }
            }

            return true;
        }

        bool LoadZones(SharedSearchPaths& paths)
        {
            for (const auto& zonePath : m_args.m_zones_to_load)
            {
                if (!fs::is_regular_file(zonePath))
                {
                    con::error("Could not find file \"{}\".", zonePath);
                    continue;
                }

                auto absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

                auto maybeZone = ZoneLoading::LoadZone(zonePath, std::nullopt);
                if (!maybeZone)
                {
                    con::error("Failed to load zone \"{}\": {}", zonePath, maybeZone.error());
                    return false;
                }

                auto zone = std::move(*maybeZone);

                con::info("Loaded zone \"{}\"", zone->m_name);

                auto searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory, zone->m_game_id);
                for (const auto& searchPath : searchPathsForZone)
                    paths.RefSearchPath(searchPath);

                if (ShouldLoadObj())
                {
                    const auto* objLoader = IObjLoader::GetObjLoaderForGame(zone->m_game_id);
                    objLoader->LoadReferencedContainersForZone(paths.GetSearchPath(), *zone);
                }

                m_loaded_zones.emplace_back(
                    std::make_unique<LoadedZoneInformation>(std::move(zone), std::move(absoluteZoneDirectory), std::move(searchPathsForZone)));
            }

            return true;
        }

        void UnloadZones(SharedSearchPaths& paths)
        {
            for (auto& loadedZone : std::ranges::reverse_view(m_loaded_zones))
            {
                // Copy zone name since we deallocate before logging
                const auto zoneName = loadedZone->GetZone().m_name;

                if (ShouldLoadObj())
                {
                    const auto* objLoader = IObjLoader::GetObjLoaderForGame(loadedZone->GetZone().m_game_id);
                    objLoader->UnloadContainersOfZone(loadedZone->GetZone());
                }

                for (const auto& searchPath : loadedZone->GetSearchPaths())
                    paths.UnrefSearchPath(searchPath);

                loadedZone.reset();

                con::info("Unloaded zone \"{}\"", zoneName);
            }

            m_loaded_zones.clear();
        }

        bool UnlinkZones(SharedSearchPaths& paths) const
        {
            ReferencedSearchPaths previousSearchPaths;

            for (const auto& zonePath : m_args.m_zones_to_unlink)
            {
                if (!fs::is_regular_file(zonePath))
                {
                    con::error("Could not find file \"{}\".", zonePath);
                    continue;
                }

                auto zoneDirectory = fs::path(zonePath).remove_filename();
                if (zoneDirectory.empty())
                    zoneDirectory = fs::current_path();
                auto absoluteZoneDirectory = absolute(zoneDirectory).string();

                auto maybeZone = ZoneLoading::LoadZone(zonePath, std::nullopt);
                if (!maybeZone)
                {
                    con::error("Failed to load zone \"{}\": {}", zonePath, maybeZone.error());
                    return false;
                }

                auto zone = std::move(*maybeZone);

                con::info("Loaded zone \"{}\"", zone->m_name);

                auto searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory, zone->m_game_id);
                for (const auto& searchPath : searchPathsForZone)
                    paths.RefSearchPath(searchPath);

                previousSearchPaths.Clear();
                previousSearchPaths = ReferencedSearchPaths(paths, std::move(searchPathsForZone));

                const auto* objLoader = IObjLoader::GetObjLoaderForGame(zone->m_game_id);
                if (ShouldLoadObj())
                    objLoader->LoadReferencedContainersForZone(paths.GetSearchPath(), *zone);

                if (!HandleZone(paths.GetSearchPath(), *zone))
                    return false;

                if (ShouldLoadObj())
                    objLoader->UnloadContainersOfZone(*zone);

                // Copy zone name for using it after freeing the zone
                std::string zoneName = zone->m_name;
                zone.reset();
                con::info("Unloaded zone \"{}\"", zoneName);
            }

            previousSearchPaths.Clear();

            return true;
        }

        static void Summarize(const bool result)
        {
            const char* resultStr = result ? "Finished" : "Failed";
            con::info("{} with {} warnings, {} errors", resultStr, con::warning_count(), con::error_count());
        }

        UnlinkerArgs m_args;
        std::vector<std::unique_ptr<LoadedZoneInformation>> m_loaded_zones;
    };
} // namespace

std::unique_ptr<Unlinker> Unlinker::Create(UnlinkerArgs args)
{
    return std::make_unique<UnlinkerImpl>(std::move(args));
}
