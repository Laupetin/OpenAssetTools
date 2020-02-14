#include "Unlinker.h"

#include "Utils/Arguments/ArgumentParser.h"
#include "ZoneLoading.h"
#include "ObjWriting.h"
#include "ContentPrinter.h"
#include "Utils/PathUtils.h"
#include "Utils/FileAPI.h"
#include "ObjLoading.h"
#include "SearchPath/SearchPaths.h"
#include "SearchPath/SearchPathFilesystem.h"

#include <set>
#include <regex>
#include <filesystem>
#include "ObjContainer/IWD/IWD.h"
#include "UnlinkerArgs.h"

class Unlinker::Impl
{
    UnlinkerArgs m_args;
    SearchPaths m_search_paths;
    SearchPathFilesystem* m_last_zone_search_path;
    std::set<std::string> m_absolute_search_paths;

    bool ShouldLoadObj() const
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
        const std::string absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

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
            std::string absolutePath = std::filesystem::absolute(path).string();

            if (!FileAPI::DirectoryExists(absolutePath))
            {
                printf("Could not find directory of search path: \"%s\"\n", path.c_str());
                return false;
            }

            auto* searchPath = new SearchPathFilesystem(absolutePath);
            LoadSearchPath(searchPath);
            m_search_paths.CommitSearchPath(searchPath);

            m_absolute_search_paths.insert(absolutePath);
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
            const std::string outputFolderPath = m_args.GetOutputFolderPathForZone(zone);
            FileAPI::DirectoryCreate(outputFolderPath);

            const std::string zoneDefinitionFileFolder = utils::Path::Combine(outputFolderPath, "zone_source");
            FileAPI::DirectoryCreate(zoneDefinitionFileFolder);

            FileAPI::File zoneDefinitionFile = FileAPI::Open(
                utils::Path::Combine(zoneDefinitionFileFolder, zone->m_name + ".zone"),
                FileAPI::Mode::MODE_WRITE);

            if (zoneDefinitionFile.IsOpen())
            {
                ObjWriting::WriteZoneDefinition(zone, &zoneDefinitionFile);
                ObjWriting::DumpZone(zone, outputFolderPath);
            }
            else
            {
                printf("Failed to open file for zone definition file of zone \"%s\".\n", zone->m_name.c_str());
                return false;
            }

            zoneDefinitionFile.Close();
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

        for (const auto& zonePath : m_args.m_zones_to_load)
        {
            if (!FileAPI::FileExists(zonePath))
            {
                printf("Could not find file \"%s\".\n", zonePath.c_str());
                continue;
            }

            std::string absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

            SearchPaths searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(&m_search_paths);

            Zone* zone = ZoneLoading::LoadZone(zonePath);
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
                ObjLoading::LoadReferencedContainersForZone(&searchPathsForZone, zone);
                ObjLoading::LoadObjDataForZone(&searchPathsForZone, zone);
            }

            if (!HandleZone(zone))
            {
                return false;
            }

            if (ShouldLoadObj())
            {
                ObjLoading::UnloadContainersOfZone(zone);
            }

            delete zone;
        }

        return true;
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
