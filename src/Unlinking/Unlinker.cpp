#include "Unlinker.h"

#include "ContentLister/ContentPrinter.h"
#include "IObjLoader.h"
#include "IObjWriter.h"
#include "ObjWriting.h"
#include "SearchPath/IWD.h"
#include "SearchPath/OutputPathFilesystem.h"
#include "UnlinkerArgs.h"
#include "UnlinkerPaths.h"
#include "Utils/Logging/Log.h"
#include "Utils/ObjFileStream.h"
#include "Zone/AssetNameResolver.h"
#include "Zone/Definition/ZoneDefWriter.h"
#include "ZoneLoading.h"

#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

namespace
{
    class UnlinkerImpl : public Unlinker
    {
    public:
        explicit UnlinkerImpl(UnlinkerArgs args)
            : m_args(std::move(args))
        {
        }

        bool Start() override
        {
            UnlinkerPaths paths;
            if (!paths.LoadUserPaths(m_args))
                return false;

            if (!LoadZones(paths))
                return false;

            const auto result = UnlinkZones(paths);

            UnloadZones();
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
            zoneDefWriter->WriteZoneDef(zoneDefinitionFile, zone, m_args.m_use_gdt);

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
            const AssetNameResolver assetNameResolver(gameId);
            auto anySpecifiedValueInvalid = false;
            for (const auto& specifiedValue : m_args.m_specified_asset_types)
            {
                const auto maybeAssetType = assetNameResolver.GetAssetTypeByName(specifiedValue);
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

                const auto* objWriter = IObjWriter::GetObjWriterForGame(zone.m_game_id);

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

        bool LoadZones(UnlinkerPaths& paths)
        {
            for (const auto& zonePath : m_args.m_zones_to_load)
            {
                if (!fs::is_regular_file(zonePath))
                {
                    con::error("Could not find file \"{}\".", zonePath);
                    continue;
                }

                auto absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

                auto searchPathsForZone = paths.GetSearchPathsForZone(absoluteZoneDirectory);
                auto maybeZone = ZoneLoading::LoadZone(zonePath, std::nullopt);
                if (!maybeZone)
                {
                    con::error("Failed to load zone \"{}\": {}", zonePath, maybeZone.error());
                    return false;
                }

                auto zone = std::move(*maybeZone);

                con::debug("Loaded zone \"{}\"", zone->m_name);

                if (ShouldLoadObj())
                {
                    const auto* objLoader = IObjLoader::GetObjLoaderForGame(zone->m_game_id);
                    objLoader->LoadReferencedContainersForZone(*searchPathsForZone, *zone);
                }

                m_loaded_zones.emplace_back(std::move(zone));
            }

            return true;
        }

        void UnloadZones()
        {
            for (auto i = m_loaded_zones.rbegin(); i != m_loaded_zones.rend(); ++i)
            {
                auto& loadedZone = *i;

                // Copy zone name since we deallocate before logging
                const auto zoneName = loadedZone->m_name;

                if (ShouldLoadObj())
                {
                    const auto* objLoader = IObjLoader::GetObjLoaderForGame(loadedZone->m_game_id);
                    objLoader->UnloadContainersOfZone(*loadedZone);
                }

                loadedZone.reset();

                con::debug("Unloaded zone \"{}\"", zoneName);
            }
            m_loaded_zones.clear();
        }

        bool UnlinkZones(UnlinkerPaths& paths) const
        {
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

                auto searchPathsForZone = paths.GetSearchPathsForZone(absoluteZoneDirectory);

                auto maybeZone = ZoneLoading::LoadZone(zonePath, std::nullopt);
                if (!maybeZone)
                {
                    con::error("Failed to load zone \"{}\": {}", zonePath, maybeZone.error());
                    return false;
                }

                auto zone = std::move(*maybeZone);

                con::debug("Loaded zone \"{}\"", zone->m_name);

                const auto* objLoader = IObjLoader::GetObjLoaderForGame(zone->m_game_id);
                if (ShouldLoadObj())
                    objLoader->LoadReferencedContainersForZone(*searchPathsForZone, *zone);

                if (!HandleZone(*searchPathsForZone, *zone))
                    return false;

                if (ShouldLoadObj())
                    objLoader->UnloadContainersOfZone(*zone);

                // Copy zone name for using it after freeing the zone
                std::string zoneName = zone->m_name;
                zone.reset();
                con::debug("Unloaded zone \"{}\"", zoneName);
            }

            return true;
        }

        UnlinkerArgs m_args;
        std::vector<std::unique_ptr<Zone>> m_loaded_zones;
    };
} // namespace

std::unique_ptr<Unlinker> Unlinker::Create(UnlinkerArgs args)
{
    return std::make_unique<UnlinkerImpl>(std::move(args));
}
