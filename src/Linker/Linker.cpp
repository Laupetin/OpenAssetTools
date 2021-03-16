#include "Linker.h"

#include <set>
#include <regex>
#include <filesystem>
#include <fstream>
#include <deque>

#include "Utils/ClassUtils.h"
#include "Utils/Arguments/ArgumentParser.h"
#include "ZoneLoading.h"
#include "ObjWriting.h"
#include "ObjLoading.h"
#include "SearchPath/SearchPaths.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "ObjContainer/IWD/IWD.h"
#include "LinkerArgs.h"
#include "ZoneWriting.h"
#include "ZoneCreation/ZoneCreationContext.h"
#include "ZoneCreation/IZoneCreator.h"
#include "Game/IW4/ZoneCreatorIW4.h"
#include "Game/T6/ZoneCreatorT6.h"

#include "Utils/ObjFileStream.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/AssetList/AssetListStream.h"
#include "Zone/Definition/ZoneDefinitionStream.h"

namespace fs = std::filesystem;

const IZoneCreator* const ZONE_CREATORS[]
{
    new IW4::ZoneCreator(),
    new T6::ZoneCreator()
};

class Linker::Impl
{
    static constexpr const char* METADATA_GAME = "game";
    static constexpr const char* METADATA_GDT = "gdt";

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
                if (m_args.m_verbose)
                    std::cout << "Adding asset search path (Not found): " << absolutePath.string() << std::endl;
                continue;
            }

            if (m_args.m_verbose)
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

    bool IncludeAdditionalZoneDefinitions(const std::string& initialFileName, ZoneDefinition& zoneDefinition, ISearchPath* sourceSearchPath) const
    {
        std::set<std::string> sourceNames;
        sourceNames.emplace(initialFileName);

        std::deque<std::string> toIncludeQueue;
        for (const auto& include : zoneDefinition.m_includes)
            toIncludeQueue.emplace_back(include);

        while (!toIncludeQueue.empty())
        {
            const auto& source = toIncludeQueue.front();

            if (sourceNames.find(source) == sourceNames.end())
            {
                sourceNames.emplace(source);

                std::unique_ptr<ZoneDefinition> includeDefinition;
                {
                    const auto definitionFileName = source + ".zone";
                    const auto definitionStream = sourceSearchPath->Open(definitionFileName);
                    if (!definitionStream.IsOpen())
                    {
                        std::cout << "Could not find zone definition file for zone \"" << source << "\"." << std::endl;
                        return false;
                    }

                    ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
                    includeDefinition = zoneDefinitionInputStream.ReadDefinition();
                }

                if (!includeDefinition)
                {
                    std::cout << "Failed to read zone definition file for zone \"" << source << "\"." << std::endl;
                    return false;
                }

                for (const auto& include : includeDefinition->m_includes)
                    toIncludeQueue.emplace_back(include);

                zoneDefinition.Include(*includeDefinition);
            }

            toIncludeQueue.pop_front();
        }

        return true;
    }

    std::unique_ptr<ZoneDefinition> ReadZoneDefinition(const std::string& zoneName, ISearchPath* sourceSearchPath) const
    {
        std::unique_ptr<ZoneDefinition> zoneDefinition;
        {
            const auto definitionFileName = zoneName + ".zone";
            const auto definitionStream = sourceSearchPath->Open(definitionFileName);
            if (!definitionStream.IsOpen())
            {
                std::cout << "Could not find zone definition file for zone \"" << zoneName << "\"." << std::endl;
                return nullptr;
            }

            ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
            zoneDefinition = zoneDefinitionInputStream.ReadDefinition();
        }

        if (!zoneDefinition)
        {
            std::cout << "Failed to read zone definition file for zone \"" << zoneName << "\"." << std::endl;
            return nullptr;
        }

        if (!IncludeAdditionalZoneDefinitions(zoneName, *zoneDefinition, sourceSearchPath))
            return nullptr;

        return zoneDefinition;
    }

    bool ReadAssetList(const std::string& zoneName, std::vector<AssetListEntry>& assetList, ISearchPath* sourceSearchPath) const
    {
        {
            const auto assetListFileName = "assetlist/" + zoneName + ".csv";
            const auto assetListStream = sourceSearchPath->Open(assetListFileName);

            if (assetListStream.IsOpen())
            {
                const AssetListInputStream stream(*assetListStream.m_stream);
                AssetListEntry entry;

                while (stream.NextEntry(entry))
                {
                    assetList.emplace_back(std::move(entry));
                }
                return true;
            }
        }

        {
            const auto zoneDefinition = ReadZoneDefinition(zoneName, sourceSearchPath);

            if (zoneDefinition)
            {
                for (const auto& entry : zoneDefinition->m_assets)
                {
                    assetList.emplace_back(entry.m_asset_type, entry.m_asset_name);
                }
                return true;
            }
        }

        return false;
    }

    bool ProcessZoneDefinitionIgnores(const std::string& zoneName, ZoneCreationContext& context, ISearchPath* sourceSearchPath) const
    {
        if (context.m_definition->m_ignores.empty())
            return true;

        std::map<std::string, std::reference_wrapper<ZoneDefinitionEntry>> zoneDefinitionAssetsByName;
        for (auto& entry : context.m_definition->m_assets)
        {
            zoneDefinitionAssetsByName.try_emplace(entry.m_asset_name, entry);
        }

        for (const auto& ignore : context.m_definition->m_ignores)
        {
            if (ignore == zoneName)
                continue;

            std::vector<AssetListEntry> assetList;
            if (!ReadAssetList(ignore, context.m_ignored_assets, sourceSearchPath))
            {
                std::cout << "Failed to read asset listing for ignoring assets of zone \"" << ignore << "\"." << std::endl;
                return false;
            }
        }
        return true;
    }

    static bool GetGameNameFromZoneDefinition(std::string& gameName, const std::string& zoneName, const ZoneDefinition& zoneDefinition)
    {
        auto firstGameEntry = true;
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata.equal_range(METADATA_GAME);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            if (firstGameEntry)
            {
                gameName = i->second;
                firstGameEntry = false;
            }
            else
            {
                if (gameName != i->second)
                {
                    std::cout << "Conflicting game names in zone \"" << zoneName << "\": " << gameName << " != " << i->second << std::endl;
                    return false;
                }
            }
        }

        if (firstGameEntry)
        {
            std::cout << "No game name was specified for zone \"" << zoneName << "\"" << std::endl;
            return false;
        }

        return true;
    }

    static bool LoadGdtFilesFromZoneDefinition(std::vector<std::unique_ptr<Gdt>>& gdtList, const std::string& zoneName, const ZoneDefinition& zoneDefinition, ISearchPath* gdtSearchPath)
    {
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata.equal_range(METADATA_GDT);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            const auto gdtFile = gdtSearchPath->Open(i->second + ".gdt");
            if (!gdtFile.IsOpen())
            {
                std::cout << "Failed to open file for gdt \"" << i->second << "\"" << std::endl;
                return false;
            }

            GdtReader gdtReader(*gdtFile.m_stream);
            auto gdt = std::make_unique<Gdt>();
            if (!gdtReader.Read(*gdt))
            {
                std::cout << "Failed to read gdt file \"" << i->second << "\"" << std::endl;
                return false;
            }

            gdtList.emplace_back(std::move(gdt));
        }

        return true;
    }

    std::unique_ptr<Zone> CreateZoneForDefinition(const std::string& zoneName, ZoneDefinition& zoneDefinition, ISearchPath* assetSearchPath, ISearchPath* gdtSearchPath,
                                                  ISearchPath* sourceSearchPath) const
    {
        auto context = std::make_unique<ZoneCreationContext>(zoneName, assetSearchPath, &zoneDefinition);
        if (!ProcessZoneDefinitionIgnores(zoneName, *context, sourceSearchPath))
            return nullptr;
        if (!GetGameNameFromZoneDefinition(context->m_game_name, zoneName, zoneDefinition))
            return nullptr;
        if (!LoadGdtFilesFromZoneDefinition(context->m_gdt_files, zoneName, zoneDefinition, gdtSearchPath))
            return nullptr;

        for (const auto* assetLoader : ZONE_CREATORS)
        {
            if (assetLoader->SupportsGame(context->m_game_name))
                return assetLoader->CreateZoneForDefinition(*context);
        }

        return nullptr;
    }

    bool WriteZoneToFile(Zone* zone)
    {
        const fs::path zoneFolderPath(m_args.GetOutputFolderPathForZone(zone->m_name));
        auto zoneFilePath(zoneFolderPath);
        zoneFilePath.append(zone->m_name + ".ff");

        fs::create_directories(zoneFolderPath);

        std::ofstream stream(zoneFilePath);
        if (!stream.is_open())
            return false;

        if(!ZoneWriting::WriteZone(stream, zone))
        {
            std::cout << "Writing zone failed." << std::endl;
            stream.close();
            return false;
        }

        stream.close();
        return true;
    }

    bool BuildZone(const std::string& zoneName)
    {
        auto assetSearchPaths = GetAssetSearchPathsForZone(zoneName);
        auto gdtSearchPaths = GetGdtSearchPathsForZone(zoneName);
        auto sourceSearchPaths = GetSourceSearchPathsForZone(zoneName);

        const auto zoneDefinition = ReadZoneDefinition(zoneName, &sourceSearchPaths);
        if (!zoneDefinition)
            return false;

        const auto zone = CreateZoneForDefinition(zoneName, *zoneDefinition, &assetSearchPaths, &gdtSearchPaths, &sourceSearchPaths);
        auto result = zone != nullptr;
        if (zone)
            result = WriteZoneToFile(zone.get());

        for (const auto& loadedSearchPath : m_loaded_zone_search_paths)
        {
            UnloadSearchPath(loadedSearchPath.get());
        }
        m_loaded_zone_search_paths.clear();

        return result;
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

            zones.emplace_back(std::move(zone));
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
