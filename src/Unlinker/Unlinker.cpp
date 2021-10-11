#include "Unlinker.h"

#include <set>
#include <regex>
#include <filesystem>
#include <fstream>

#include "Utils/ClassUtils.h"
#include "Utils/Arguments/ArgumentParser.h"
#include "ZoneLoading.h"
#include "ObjWriting.h"
#include "ContentLister/ContentPrinter.h"
#include "ObjLoading.h"
#include "SearchPath/SearchPaths.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "ContentLister/ZoneDefWriter.h"
#include "ObjContainer/IWD/IWD.h"
#include "UnlinkerArgs.h"
#include "Game/IW3/ZoneDefWriterIW3.h"
#include "Game/IW4/ZoneDefWriterIW4.h"
#include "Game/IW5/ZoneDefWriterIW5.h"
#include "Game/T5/ZoneDefWriterT5.h"
#include "Game/T6/ZoneDefWriterT6.h"
#include "Utils/ObjFileStream.h"

namespace fs = std::filesystem;

const IZoneDefWriter* const ZONE_DEF_WRITERS[]
{
    new IW3::ZoneDefWriter(),
    new IW4::ZoneDefWriter(),
    new IW5::ZoneDefWriter(),
    new T5::ZoneDefWriter(),
    new T6::ZoneDefWriter()
};

class Unlinker::Impl
{
    UnlinkerArgs m_args;
    SearchPaths m_search_paths;
    SearchPathFilesystem* m_last_zone_search_path;
    std::set<std::string> m_absolute_search_paths;

    std::vector<std::unique_ptr<Zone>> m_loaded_zones;

    _NODISCARD bool ShouldLoadObj() const
    {
        return m_args.m_task != UnlinkerArgs::ProcessingTask::LIST;
    }

    /**
     * \brief Loads a search path.
     * \param searchPath The search path to load.
     */
    void LoadSearchPath(ISearchPath* searchPath) const
    {
        if (ShouldLoadObj())
        {
            if (m_args.m_verbose)
            {
                printf("Loading search path: \"%s\"\n", searchPath->GetPath().c_str());
            }

            ObjLoading::LoadIWDsInSearchPath(searchPath);
        }
    }

    /**
     * \brief Unloads a search path.
     * \param searchPath The search path to unload.
     */
    void UnloadSearchPath(ISearchPath* searchPath) const
    {
        if (ShouldLoadObj())
        {
            if (m_args.m_verbose)
            {
                printf("Unloading search path: \"%s\"\n", searchPath->GetPath().c_str());
            }

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
            searchPathsForZone.IncludeSearchPath(m_last_zone_search_path);
        }
        else if (m_absolute_search_paths.find(absoluteZoneDirectory) == m_absolute_search_paths.end())
        {
            if (m_last_zone_search_path != nullptr)
            {
                UnloadSearchPath(m_last_zone_search_path);
                delete m_last_zone_search_path;
            }

            m_last_zone_search_path = new SearchPathFilesystem(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(m_last_zone_search_path);
            LoadSearchPath(m_last_zone_search_path);
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
                printf("Could not find directory of search path: \"%s\"\n", path.c_str());
                return false;
            }

            auto searchPath = std::make_unique<SearchPathFilesystem>(absolutePath.string());
            LoadSearchPath(searchPath.get());
            m_search_paths.CommitSearchPath(std::move(searchPath));

            m_absolute_search_paths.insert(absolutePath.string());
        }

        if (m_args.m_verbose)
        {
            printf("%u SearchPaths%s\n", m_absolute_search_paths.size(), !m_absolute_search_paths.empty() ? ":" : "");
            for (const auto& absoluteSearchPath : m_absolute_search_paths)
            {
                printf("  \"%s\"\n", absoluteSearchPath.c_str());
            }

            if (!m_absolute_search_paths.empty())
            {
                puts("");
            }
        }

        return true;
    }

    bool WriteZoneDefinitionFile(Zone* zone, const fs::path& zoneDefinitionFileFolder) const
    {
        auto zoneDefinitionFilePath(zoneDefinitionFileFolder);
        zoneDefinitionFilePath.append(zone->m_name);
        zoneDefinitionFilePath.replace_extension(".zone");

        std::ofstream zoneDefinitionFile(zoneDefinitionFilePath, std::fstream::out | std::fstream::binary);
        if (!zoneDefinitionFile.is_open())
        {
            printf("Failed to open file for zone definition file of zone \"%s\".\n", zone->m_name.c_str());
            return false;
        }

        auto result = false;
        for (const auto* zoneDefWriter : ZONE_DEF_WRITERS)
        {
            if (zoneDefWriter->CanHandleZone(zone))
            {
                zoneDefWriter->WriteZoneDef(zoneDefinitionFile, &m_args, zone);
                result = true;
                break;
            }
        }

        if (!result)
        {
            printf("Failed to find writer for zone definition file of zone \"%s\".\n", zone->m_name.c_str());
        }

        zoneDefinitionFile.close();
        return result;
    }

    static bool OpenGdtFile(Zone* zone, const fs::path& outputFolder, std::ofstream& stream)
    {
        auto gdtFilePath(outputFolder);
        gdtFilePath.append("source_data");

        fs::create_directories(gdtFilePath);

        gdtFilePath.append(zone->m_name);
        gdtFilePath.replace_extension(".gdt");

        stream = std::ofstream(gdtFilePath, std::fstream::out | std::fstream::binary);
        if (!stream.is_open())
        {
            printf("Failed to open file for zone definition file of zone \"%s\".\n", zone->m_name.c_str());
            return false;
        }

        return true;
    }

    void UpdateAssetIncludesAndExcludes(const AssetDumpingContext& context) const
    {
        const auto assetTypeCount = context.m_zone->m_pools->GetAssetTypeCount();

        ObjWriting::Configuration.AssetTypesToHandleBitfield = std::vector<bool>(assetTypeCount);

        for(auto i = 0; i < assetTypeCount; i++)
        {
            const auto assetTypeName = std::string(context.m_zone->m_pools->GetAssetTypeName(i));

            if (m_args.m_specified_asset_types.find(assetTypeName) != m_args.m_specified_asset_types.end())
                ObjWriting::Configuration.AssetTypesToHandleBitfield[i] = m_args.m_asset_type_handling == UnlinkerArgs::AssetTypeHandling::INCLUDE;
            else
                ObjWriting::Configuration.AssetTypesToHandleBitfield[i] = m_args.m_asset_type_handling == UnlinkerArgs::AssetTypeHandling::EXCLUDE;
        }
    }

    /**
     * \brief Performs the tasks specified by the command line arguments on the specified zone.
     * \param zone The zone to handle.
     * \return \c true if handling the zone was successful, otherwise \c false.
     */
    bool HandleZone(Zone* zone) const
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
            context.m_zone = zone;
            context.m_base_path = outputFolderPath;

            if (m_args.m_use_gdt)
            {
                if (!OpenGdtFile(zone, outputFolderPath, gdtStream))
                    return false;
                auto gdt = std::make_unique<GdtOutputStream>(gdtStream);
                gdt->BeginStream();
                gdt->WriteVersion(GdtVersion(zone->m_game->GetShortName(), 1));
                context.m_gdt = std::move(gdt);
            }

            UpdateAssetIncludesAndExcludes(context);
            ObjWriting::DumpZone(context);

            if (m_args.m_use_gdt)
            {
                context.m_gdt->EndStream();
                gdtStream.close();
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
                printf("Could not find file \"%s\".\n", zonePath.c_str());
                continue;
            }

            auto absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

            auto searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(&m_search_paths);

            auto zone = ZoneLoading::LoadZone(zonePath);
            if (zone == nullptr)
            {
                printf("Failed to load zone \"%s\".\n", zonePath.c_str());
                return false;
            }

            if (m_args.m_verbose)
            {
                printf("Loaded zone \"%s\"\n", zone->m_name.c_str());
            }

            if (ShouldLoadObj())
            {
                ObjLoading::LoadReferencedContainersForZone(&searchPathsForZone, zone.get());
                ObjLoading::LoadObjDataForZone(&searchPathsForZone, zone.get());
            }


            m_loaded_zones.emplace_back(std::move(zone));
        }

        return true;
    }

    void UnloadZones()
    {
        if (ShouldLoadObj())
        {
            for (auto& loadedZone : m_loaded_zones)
            {
                ObjLoading::UnloadContainersOfZone(loadedZone.get());
            }
        }
        m_loaded_zones.clear();
    }

    bool UnlinkZones()
    {
        for (const auto& zonePath : m_args.m_zones_to_unlink)
        {
            if (!fs::is_regular_file(zonePath))
            {
                printf("Could not find file \"%s\".\n", zonePath.c_str());
                continue;
            }

            auto absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

            auto searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(&m_search_paths);

            auto zone = ZoneLoading::LoadZone(zonePath);
            if (zone == nullptr)
            {
                printf("Failed to load zone \"%s\".\n", zonePath.c_str());
                return false;
            }

            if (m_args.m_verbose)
            {
                printf("Loaded zone \"%s\"\n", zone->m_name.c_str());
            }

            if (ShouldLoadObj())
            {
                ObjLoading::LoadReferencedContainersForZone(&searchPathsForZone, zone.get());
                ObjLoading::LoadObjDataForZone(&searchPathsForZone, zone.get());
            }

            if (!HandleZone(zone.get()))
                return false;

            if (ShouldLoadObj())
                ObjLoading::UnloadContainersOfZone(zone.get());
        }

        return true;
    }

public:
    Impl()
    {
        m_last_zone_search_path = nullptr;
    }

    /**
     * \copydoc Unlinker::Start
     */
    bool Start(const int argc, const char** argv)
    {
        if (!m_args.ParseArgs(argc, argv))
            return false;

        if (!BuildSearchPaths())
            return false;

        if (!LoadZones())
            return false;

        const auto result = UnlinkZones();

        UnloadZones();
        return result;
    }
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
