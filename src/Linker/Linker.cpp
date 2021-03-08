#include "Linker.h"

#include <set>
#include <regex>
#include <filesystem>
#include <fstream>

#include "Utils/ClassUtils.h"
#include "Utils/Arguments/ArgumentParser.h"
#include "ZoneLoading.h"
#include "ObjWriting.h"
#include "ObjLoading.h"
#include "SearchPath/SearchPaths.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "ObjContainer/IWD/IWD.h"
#include "LinkerArgs.h"

#include "Utils/ObjFileStream.h"

namespace fs = std::filesystem;

class Linker::Impl
{
    LinkerArgs m_args;
    SearchPaths m_search_paths;
    SearchPathFilesystem* m_last_zone_search_path;
    std::set<std::string> m_absolute_search_paths;

    /**
     * \brief Loads a search path.
     * \param searchPath The search path to load.
     */
    void LoadSearchPath(ISearchPath* searchPath) const
    {
        if (m_args.m_verbose)
        {
            printf("Loading search path: \"%s\"\n", searchPath->GetPath().c_str());
        }

        ObjLoading::LoadIWDsInSearchPath(searchPath);
    }

    /**
     * \brief Unloads a search path.
     * \param searchPath The search path to unload.
     */
    void UnloadSearchPath(ISearchPath* searchPath) const
    {
        if (m_args.m_verbose)
        {
            printf("Unloading search path: \"%s\"\n", searchPath->GetPath().c_str());
        }

        ObjLoading::UnloadIWDsInSearchPath(searchPath);
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
     * \brief Initializes the Linker object's search paths based on the user's input.
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

    /**
     * \brief Performs the tasks specified by the command line arguments on the specified zone.
     * \param zone The zone to handle.
     * \return \c true if handling the zone was successful, otherwise \c false.
     */
    bool BuildZone(const std::string& zoneName) const
    {
        return true;
    }

public:
    Impl()
    {
        m_last_zone_search_path = nullptr;
    }

    /**
     * \copydoc Linker::Start
     */
    bool Start(const int argc, const char** argv)
    {
        if (!m_args.ParseArgs(argc, argv))
            return false;

        if (!BuildSearchPaths())
            return false;

        std::vector<std::unique_ptr<Zone>> zones;

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

            auto zone = std::unique_ptr<Zone>(ZoneLoading::LoadZone(zonePath));
            if (zone == nullptr)
            {
                printf("Failed to load zone \"%s\".\n", zonePath.c_str());
                return false;
            }

            if (m_args.m_verbose)
            {
                printf("Loaded zone \"%s\"\n", zone->m_name.c_str());
            }

            ObjLoading::LoadReferencedContainersForZone(&searchPathsForZone, zone.get());
            ObjLoading::LoadObjDataForZone(&searchPathsForZone, zone.get());
        }

        auto result = true;
        for(const auto& zone : m_args.m_zones_to_build)
        {
            if (!BuildZone(zone))
            {
                result = false;
                break;
            }
        }

        for(const auto& zone : zones)
        {
            ObjLoading::UnloadContainersOfZone(zone.get());
        }
        zones.clear();

        return result;
    }
};

Linker::Linker()
{
    m_impl = new Impl();
}

Linker::~Linker()
{
    delete m_impl;
    m_impl = nullptr;
}

bool Linker::Start(const int argc, const char** argv) const
{
    return m_impl->Start(argc, argv);
}
