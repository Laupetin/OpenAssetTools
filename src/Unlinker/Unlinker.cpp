#include "Unlinker.h"

#include "ContentLister/ContentPrinter.h"
#include "ContentLister/ZoneDefWriter.h"
#include "IObjLoader.h"
#include "IObjWriter.h"
#include "ObjContainer/IWD/IWD.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "SearchPath/SearchPaths.h"
#include "UnlinkerArgs.h"
#include "Utils/ClassUtils.h"
#include "Utils/ObjFileStream.h"
#include "ZoneLoading.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <regex>
#include <set>

namespace fs = std::filesystem;

class Unlinker::Impl
{
public:
    /**
     * \copydoc Unlinker::Start
     */
    bool Start(const int argc, const char** argv)
    {
        auto shouldContinue = true;
        if (!m_args.ParseArgs(argc, argv, shouldContinue))
            return false;

        if (!shouldContinue)
            return true;

        if (!BuildSearchPaths())
            return false;

        if (!LoadZones())
            return false;

        const auto result = UnlinkZones();

        UnloadZones();
        return result;
    }

private:
    _NODISCARD bool ShouldLoadObj() const
    {
        return m_args.m_task != UnlinkerArgs::ProcessingTask::LIST && !m_args.m_skip_obj;
    }

    /**
     * \brief Loads a search path.
     * \param searchPath The search path to load.
     */
    void LoadSearchPath(ISearchPath& searchPath) const
    {
        if (ShouldLoadObj())
        {
            if (m_args.m_verbose)
                std::cout << std::format("Loading search path: \"{}\"\n", searchPath.GetPath());

            ObjLoading::LoadIWDsInSearchPath(searchPath);
        }
    }

    /**
     * \brief Unloads a search path.
     * \param searchPath The search path to unload.
     */
    void UnloadSearchPath(ISearchPath& searchPath) const
    {
        if (ShouldLoadObj())
        {
            if (m_args.m_verbose)
                std::cout << std::format("Unloading search path: \"{}\"\n", searchPath.GetPath());

            ObjLoading::UnloadIWDsInSearchPath(searchPath);
        }
    }

    /**
     * \brief Loads all search paths that are valid for the specified zone and returns them.
     * \param zonePath The path to the zone file that should be prepared for.
     * \return A \c SearchPaths object that contains all search paths that should be considered when loading the specified zone.
     */
    SearchPaths GetSearchPathsForZone(const std::string& zonePath)
    {
        SearchPaths searchPathsForZone;
        const auto absoluteZoneDirectory = fs::absolute(std::filesystem::path(zonePath).remove_filename()).string();

        if (m_last_zone_search_path != nullptr && m_last_zone_search_path->GetPath() == absoluteZoneDirectory)
        {
            searchPathsForZone.IncludeSearchPath(m_last_zone_search_path.get());
        }
        else if (m_absolute_search_paths.find(absoluteZoneDirectory) == m_absolute_search_paths.end())
        {
            if (m_last_zone_search_path)
            {
                UnloadSearchPath(*m_last_zone_search_path);
            }

            m_last_zone_search_path = std::make_unique<SearchPathFilesystem>(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(m_last_zone_search_path.get());
            LoadSearchPath(*m_last_zone_search_path);
        }

        for (auto* iwd : IWD::Repository)
        {
            searchPathsForZone.IncludeSearchPath(iwd);
        }

        return searchPathsForZone;
    }

    /**
     * \brief Initializes the Unlinker object's search paths based on the user's input.
     * \return \c true if building the search paths was successful, otherwise \c false.
     */
    bool BuildSearchPaths()
    {
        for (const auto& path : m_args.m_user_search_paths)
        {
            auto absolutePath = fs::absolute(path);

            if (!fs::is_directory(absolutePath))
            {
                std::cerr << std::format("Could not find directory of search path: \"{}\"\n", path);
                return false;
            }

            auto searchPath = std::make_unique<SearchPathFilesystem>(absolutePath.string());
            LoadSearchPath(*searchPath);
            m_search_paths.CommitSearchPath(std::move(searchPath));

            m_absolute_search_paths.insert(absolutePath.string());
        }

        if (m_args.m_verbose)
        {
            std::cout << std::format("{} SearchPaths{}\n", m_absolute_search_paths.size(), !m_absolute_search_paths.empty() ? ":" : "");
            for (const auto& absoluteSearchPath : m_absolute_search_paths)
                std::cout << std::format("  \"{}\"\n", absoluteSearchPath);

            if (!m_absolute_search_paths.empty())
                std::cerr << "\n";
        }

        return true;
    }

    bool WriteZoneDefinitionFile(const Zone& zone, const fs::path& zoneDefinitionFileFolder) const
    {
        auto zoneDefinitionFilePath(zoneDefinitionFileFolder);
        zoneDefinitionFilePath.append(zone.m_name);
        zoneDefinitionFilePath.replace_extension(".zone");

        std::ofstream zoneDefinitionFile(zoneDefinitionFilePath, std::fstream::out | std::fstream::binary);
        if (!zoneDefinitionFile.is_open())
        {
            std::cerr << std::format("Failed to open file for zone definition file of zone \"{}\".\n", zone.m_name);
            return false;
        }

        const auto* zoneDefWriter = IZoneDefWriter::GetZoneDefWriterForGame(zone.m_game->GetId());
        zoneDefWriter->WriteZoneDef(zoneDefinitionFile, m_args, zone);

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
            std::cerr << std::format("Failed to open file for zone definition file of zone \"{}\".\n", zone.m_name);
            return false;
        }

        return true;
    }

    void UpdateAssetIncludesAndExcludes(const AssetDumpingContext& context) const
    {
        const auto assetTypeCount = context.m_zone->m_pools->GetAssetTypeCount();

        ObjWriting::Configuration.AssetTypesToHandleBitfield = std::vector<bool>(assetTypeCount);

        std::vector<bool> handledSpecifiedAssets(m_args.m_specified_asset_types.size());
        for (auto i = 0; i < assetTypeCount; i++)
        {
            const auto assetTypeName = std::string(*context.m_zone->m_pools->GetAssetTypeName(i));

            const auto foundSpecifiedEntry = m_args.m_specified_asset_type_map.find(assetTypeName);
            if (foundSpecifiedEntry != m_args.m_specified_asset_type_map.end())
            {
                ObjWriting::Configuration.AssetTypesToHandleBitfield[i] = m_args.m_asset_type_handling == UnlinkerArgs::AssetTypeHandling::INCLUDE;
                assert(foundSpecifiedEntry->second < handledSpecifiedAssets.size());
                handledSpecifiedAssets[foundSpecifiedEntry->second] = true;
            }
            else
                ObjWriting::Configuration.AssetTypesToHandleBitfield[i] = m_args.m_asset_type_handling == UnlinkerArgs::AssetTypeHandling::EXCLUDE;
        }

        auto anySpecifiedValueInvalid = false;
        for (auto i = 0u; i < handledSpecifiedAssets.size(); i++)
        {
            if (!handledSpecifiedAssets[i])
            {
                std::cerr << std::format("Unknown asset type \"{}\"\n", m_args.m_specified_asset_types[i]);
                anySpecifiedValueInvalid = true;
            }
        }

        if (anySpecifiedValueInvalid)
        {
            std::cerr << "Valid asset types are:\n";

            auto first = true;
            for (auto i = 0; i < assetTypeCount; i++)
            {
                const auto assetTypeName = std::string(*context.m_zone->m_pools->GetAssetTypeName(i));

                if (first)
                    first = false;
                else
                    std::cerr << ", ";
                std::cerr << assetTypeName;
            }
            std::cerr << "\n";
        }
    }

    /**
     * \brief Performs the tasks specified by the command line arguments on the specified zone.
     * \param searchPath The search path for obj data.
     * \param zone The zone to handle.
     * \return \c true if handling the zone was successful, otherwise \c false.
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
            const auto outputFolderPath = m_args.GetOutputFolderPathForZone(zone);
            fs::create_directories(outputFolderPath);

            fs::path zoneDefinitionFileFolder(outputFolderPath);
            zoneDefinitionFileFolder.append("zone_source");
            fs::create_directories(zoneDefinitionFileFolder);

            if (!WriteZoneDefinitionFile(zone, zoneDefinitionFileFolder))
                return false;

            std::ofstream gdtStream;
            AssetDumpingContext context;
            context.m_zone = &zone;
            context.m_base_path = outputFolderPath;
            context.m_obj_search_path = &searchPath;

            if (m_args.m_use_gdt)
            {
                if (!OpenGdtFile(zone, outputFolderPath, gdtStream))
                    return false;
                auto gdt = std::make_unique<GdtOutputStream>(gdtStream);
                gdt->BeginStream();
                gdt->WriteVersion(GdtVersion(zone.m_game->GetShortName(), 1));
                context.m_gdt = std::move(gdt);
            }

            UpdateAssetIncludesAndExcludes(context);

            const auto* objWriter = IObjWriter::GetObjWriterForGame(zone.m_game->GetId());

            auto result = objWriter->DumpZone(context);

            if (m_args.m_use_gdt)
            {
                context.m_gdt->EndStream();
                gdtStream.close();
            }

            if (!result)
            {
                std::cerr << "Dumping zone failed!\n";
                return false;
            }
        }

        return true;
    }

    bool LoadZones()
    {
        for (const auto& zonePath : m_args.m_zones_to_load)
        {
            if (!fs::is_regular_file(zonePath))
            {
                std::cerr << std::format("Could not find file \"{}\".\n", zonePath);
                continue;
            }

            auto absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

            auto searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(&m_search_paths);

            auto zone = ZoneLoading::LoadZone(zonePath);
            if (zone == nullptr)
            {
                std::cerr << std::format("Failed to load zone \"{}\".\n", zonePath);
                return false;
            }

            if (m_args.m_verbose)
                std::cout << std::format("Loaded zone \"{}\"\n", zone->m_name);

            if (ShouldLoadObj())
            {
                const auto* objLoader = IObjLoader::GetObjLoaderForGame(zone->m_game->GetId());
                objLoader->LoadReferencedContainersForZone(searchPathsForZone, *zone);
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
                const auto* objLoader = IObjLoader::GetObjLoaderForGame(loadedZone->m_game->GetId());
                objLoader->UnloadContainersOfZone(*loadedZone);
            }

            loadedZone.reset();

            if (m_args.m_verbose)
                std::cout << std::format("Unloaded zone \"{}\"\n", zoneName);
        }
        m_loaded_zones.clear();
    }

    bool UnlinkZones()
    {
        for (const auto& zonePath : m_args.m_zones_to_unlink)
        {
            if (!fs::is_regular_file(zonePath))
            {
                std::cerr << std::format("Could not find file \"{}\".\n", zonePath);
                continue;
            }

            auto zoneDirectory = fs::path(zonePath).remove_filename();
            if (zoneDirectory.empty())
                zoneDirectory = fs::current_path();
            auto absoluteZoneDirectory = absolute(zoneDirectory).string();

            auto searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(&m_search_paths);

            std::string zoneName;
            auto zone = ZoneLoading::LoadZone(zonePath);
            if (zone == nullptr)
            {
                std::cerr << std::format("Failed to load zone \"{}\".\n", zonePath);
                return false;
            }

            zoneName = zone->m_name;
            if (m_args.m_verbose)
                std::cout << std::format("Loaded zone \"{}\"\n", zoneName);

            const auto* objLoader = IObjLoader::GetObjLoaderForGame(zone->m_game->GetId());
            if (ShouldLoadObj())
                objLoader->LoadReferencedContainersForZone(searchPathsForZone, *zone);

            if (!HandleZone(searchPathsForZone, *zone))
                return false;

            if (ShouldLoadObj())
                objLoader->UnloadContainersOfZone(*zone);

            zone.reset();
            if (m_args.m_verbose)
                std::cout << std::format("Unloaded zone \"{}\"\n", zoneName);
        }

        return true;
    }

    UnlinkerArgs m_args;
    SearchPaths m_search_paths;
    std::unique_ptr<SearchPathFilesystem> m_last_zone_search_path;
    std::set<std::string> m_absolute_search_paths;

    std::vector<std::unique_ptr<Zone>> m_loaded_zones;
};

Unlinker::Unlinker()
{
    m_impl = new Impl();
}

Unlinker::~Unlinker()
{
    delete m_impl;
    m_impl = nullptr;
}

bool Unlinker::Start(const int argc, const char** argv) const
{
    return m_impl->Start(argc, argv);
}
