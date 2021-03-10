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
#include "Zone/Definition/ZoneDefinitionStream.h"

namespace fs = std::filesystem;

class Linker::Impl
{
    LinkerArgs m_args;
    std::vector<std::unique_ptr<ISearchPath>> m_loaded_zone_search_paths;
    SearchPaths m_asset_search_paths;
    SearchPaths m_gdt_search_paths;
    SearchPaths m_source_search_paths;

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
    
    SearchPaths GetAssetSearchPathsForZone(const std::string& zoneName)
    {
        SearchPaths searchPathsForZone;

        for (const auto& searchPathStr : m_args.GetAssetSearchPathsForZone(zoneName))
        {
            auto absolutePath = fs::absolute(searchPathStr);

            if (!fs::is_directory(absolutePath))
            {
                if (m_args.m_verbose)
                    std::cout << "Adding asset search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if (m_args.m_verbose)
                std::cout << "Adding asset search path: " << absolutePath.string() << std::endl;

            auto searchPath = std::make_unique<SearchPathFilesystem>(searchPathStr);
            LoadSearchPath(searchPath.get());
            searchPathsForZone.IncludeSearchPath(searchPath.get());
            m_loaded_zone_search_paths.emplace_back(std::move(searchPath));
        }

        searchPathsForZone.IncludeSearchPath(&m_asset_search_paths);

        for (auto* iwd : IWD::Repository)
        {
            searchPathsForZone.IncludeSearchPath(iwd);
        }

        return searchPathsForZone;
    }
    
    SearchPaths GetGdtSearchPathsForZone(const std::string& zoneName)
    {
        SearchPaths searchPathsForZone;

        for (const auto& searchPathStr : m_args.GetGdtSearchPathsForZone(zoneName))
        {
            auto absolutePath = fs::absolute(searchPathStr);

            if (!fs::is_directory(absolutePath))
            {
                if (m_args.m_verbose)
                    std::cout << "Adding gdt search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if (m_args.m_verbose)
                std::cout << "Adding gdt search path: " << absolutePath.string() << std::endl;

            searchPathsForZone.CommitSearchPath(std::make_unique<SearchPathFilesystem>(searchPathStr));
        }

        searchPathsForZone.IncludeSearchPath(&m_gdt_search_paths);

        return searchPathsForZone;
    }
    
    SearchPaths GetSourceSearchPathsForZone(const std::string& zoneName)
    {
        SearchPaths searchPathsForZone;

        for (const auto& searchPathStr : m_args.GetSourceSearchPathsForZone(zoneName))
        {
            auto absolutePath = fs::absolute(searchPathStr);

            if (!fs::is_directory(absolutePath))
            {
                if (m_args.m_verbose)
                    std::cout << "Adding source search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if (m_args.m_verbose)
                std::cout << "Adding source search path: " << absolutePath.string() << std::endl;

            searchPathsForZone.CommitSearchPath(std::make_unique<SearchPathFilesystem>(searchPathStr));
        }
        
        searchPathsForZone.IncludeSearchPath(&m_source_search_paths);

        return searchPathsForZone;
    }

    /**
     * \brief Initializes the Linker object's search paths based on the user's input.
     * \return \c true if building the search paths was successful, otherwise \c false.
     */
    bool BuildZoneIndependentSearchPaths()
    {
        for (const auto& path : m_args.GetZoneIndependentAssetSearchPaths())
        {
            auto absolutePath = fs::absolute(path);

            if (!fs::is_directory(absolutePath))
            {
                if(m_args.m_verbose)
                    std::cout << "Adding asset search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if(m_args.m_verbose)
                std::cout << "Adding asset search path: " << absolutePath.string() << std::endl;

            auto searchPath = std::make_unique<SearchPathFilesystem>(absolutePath.string());
            LoadSearchPath(searchPath.get());
            m_asset_search_paths.CommitSearchPath(std::move(searchPath));
        }

        for (const auto& path : m_args.GetZoneIndependentGdtSearchPaths())
        {
            auto absolutePath = fs::absolute(path);

            if (!fs::is_directory(absolutePath))
            {
                if (m_args.m_verbose)
                    std::cout << "Loading gdt search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if (m_args.m_verbose)
                std::cout << "Adding gdt search path: " << absolutePath.string() << std::endl;

            m_gdt_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absolutePath.string()));
        }

        for (const auto& path : m_args.GetZoneIndependentSourceSearchPaths())
        {
            auto absolutePath = fs::absolute(path);

            if (!fs::is_directory(absolutePath))
            {
                if (m_args.m_verbose)
                    std::cout << "Loading source search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if (m_args.m_verbose)
                std::cout << "Adding source search path: " << absolutePath.string() << std::endl;

            m_source_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absolutePath.string()));
        }

        return true;
    }
    
    bool BuildZone(const std::string& zoneName)
    {
        auto assetSearchPaths = GetAssetSearchPathsForZone(zoneName);
        auto gdtSearchPaths = GetGdtSearchPathsForZone(zoneName);
        auto sourceSearchPaths = GetSourceSearchPathsForZone(zoneName);

        std::unique_ptr<ZoneDefinition> zoneDefinition;
        {
            const auto definitionFileName = zoneName + ".zone";
            const auto definitionStream = sourceSearchPaths.Open(definitionFileName);
            if (!definitionStream)
            {
                std::cout << "Could not find zone definition file for zone \"" << zoneName << "\"." << std::endl;
                return false;
            }

            ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream, definitionFileName, m_args.m_verbose);
            zoneDefinition = zoneDefinitionInputStream.ReadDefinition();
        }

        if(!zoneDefinition)
        {
            std::cout << "Failed to read zone definition file for zone \"" << zoneName << "\"." << std::endl;
            return false;
        }

        for(const auto& loadedSearchPath : m_loaded_zone_search_paths)
        {
            UnloadSearchPath(loadedSearchPath.get());
        }
        m_loaded_zone_search_paths.clear();
        
        return true;
    }

public:
    Impl()
    = default;

    /**
     * \copydoc Linker::Start
     */
    bool Start(const int argc, const char** argv)
    {
        if (!m_args.ParseArgs(argc, argv))
            return false;

        if (!BuildZoneIndependentSearchPaths())
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
        }

        auto result = true;
        for (const auto& zone : m_args.m_zones_to_build)
        {
            if (!BuildZone(zone))
            {
                result = false;
                break;
            }
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
