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
#include "ObjContainer/IWD/IWD.h"
#include "LinkerArgs.h"
#include "LinkerSearchPaths.h"
#include "ZoneWriting.h"
#include "Game/IW3/ZoneCreatorIW3.h"
#include "ZoneCreation/ZoneCreationContext.h"
#include "ZoneCreation/IZoneCreator.h"
#include "Game/IW4/ZoneCreatorIW4.h"
#include "Game/IW5/ZoneCreatorIW5.h"
#include "Game/T5/ZoneCreatorT5.h"
#include "Game/T6/ZoneCreatorT6.h"

#include "Utils/ObjFileStream.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/AssetList/AssetListStream.h"
#include "Zone/Definition/ZoneDefinitionStream.h"

namespace fs = std::filesystem;

const IZoneCreator* const ZONE_CREATORS[]
{
    new IW3::ZoneCreator(),
    new IW4::ZoneCreator(),
    new IW5::ZoneCreator(),
    new T5::ZoneCreator(),
    new T6::ZoneCreator()
};

enum class ProjectType
{
    FASTFILE,
    IPAK,

    MAX
};

constexpr const char* PROJECT_TYPE_NAMES[static_cast<unsigned>(ProjectType::MAX)]
{
    "fastfile",
    "ipak"
};

class LinkerImpl final : public Linker
{
    static constexpr const char* METADATA_GAME = "game";
    static constexpr const char* METADATA_GDT = "gdt";
    static constexpr const char* METADATA_NAME = "name";
    static constexpr const char* METADATA_TYPE = "type";

    LinkerArgs m_args;
    LinkerSearchPaths m_search_paths;
    std::vector<std::unique_ptr<Zone>> m_loaded_zones;

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
                        std::cout << "Could not find zone definition file for project \"" << source << "\"." << std::endl;
                        return false;
                    }

                    ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
                    includeDefinition = zoneDefinitionInputStream.ReadDefinition();
                }

                if (!includeDefinition)
                {
                    std::cout << "Failed to read zone definition file for project \"" << source << "\"." << std::endl;
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

    bool ReadAssetList(const std::string& zoneName, AssetList& assetList, ISearchPath* sourceSearchPath) const
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
                    assetList.m_entries.emplace_back(std::move(entry));
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
                    assetList.m_entries.emplace_back(entry.m_asset_type, entry.m_asset_name);
                }
                return true;
            }
        }

        return false;
    }

    bool IncludeAssetLists(ZoneDefinition& zoneDefinition, ISearchPath* sourceSearchPath) const
    {
        for (const auto& assetListName : zoneDefinition.m_asset_lists)
        {
            AssetList assetList;
            if (!ReadAssetList(assetListName, assetList, sourceSearchPath))
            {
                std::cerr << "Failed to read asset list \"" << assetListName << "\"\n";
                return false;
            }

            zoneDefinition.Include(assetList);
        }

        return true;
    }

    static bool GetNameFromZoneDefinition(std::string& name, const std::string& projectName, const ZoneDefinition& zoneDefinition)
    {
        auto firstNameEntry = true;
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata_lookup.equal_range(METADATA_NAME);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            if (firstNameEntry)
            {
                name = i->second->m_value;
                firstNameEntry = false;
            }
            else
            {
                if (name != i->second->m_value)
                {
                    std::cout << "Conflicting names in project \"" << projectName << "\": " << name << " != " << i->second << std::endl;
                    return false;
                }
            }
        }

        if (firstNameEntry)
            name = projectName;

        return true;
    }

    std::unique_ptr<ZoneDefinition> ReadZoneDefinition(const std::string& projectName, ISearchPath* sourceSearchPath) const
    {
        std::unique_ptr<ZoneDefinition> zoneDefinition;
        {
            const auto definitionFileName = projectName + ".zone";
            const auto definitionStream = sourceSearchPath->Open(definitionFileName);
            if (!definitionStream.IsOpen())
            {
                std::cout << "Could not find zone definition file for project \"" << projectName << "\"." << std::endl;
                return nullptr;
            }

            ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
            zoneDefinition = zoneDefinitionInputStream.ReadDefinition();
        }

        if (!zoneDefinition)
        {
            std::cout << "Failed to read zone definition file for project \"" << projectName << "\"." << std::endl;
            return nullptr;
        }

        if (!GetNameFromZoneDefinition(zoneDefinition->m_name, projectName, *zoneDefinition))
            return nullptr;

        if (!IncludeAdditionalZoneDefinitions(projectName, *zoneDefinition, sourceSearchPath))
            return nullptr;

        if (!IncludeAssetLists(*zoneDefinition, sourceSearchPath))
            return nullptr;

        return zoneDefinition;
    }

    bool ProcessZoneDefinitionIgnores(const std::string& projectName, ZoneCreationContext& context, ISearchPath* sourceSearchPath) const
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
            if (ignore == projectName)
                continue;

            std::vector<AssetListEntry> assetList;
            if (!ReadAssetList(ignore, context.m_ignored_assets, sourceSearchPath))
            {
                std::cout << "Failed to read asset listing for ignoring assets of project \"" << ignore << "\"." << std::endl;
                return false;
            }
        }
        return true;
    }

    static bool ProjectTypeByName(ProjectType& projectType, const std::string& projectTypeName)
    {
        for (auto i = 0u; i < static_cast<unsigned>(ProjectType::MAX); i++)
        {
            if (projectTypeName == PROJECT_TYPE_NAMES[i])
            {
                projectType = static_cast<ProjectType>(i);
                return true;
            }
        }

        return false;
    }

    static bool GetProjectTypeFromZoneDefinition(ProjectType& projectType, const std::string& projectName, const ZoneDefinition& zoneDefinition)
    {
        auto firstGameEntry = true;
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata_lookup.equal_range(METADATA_TYPE);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            ProjectType parsedProjectType;
            if (!ProjectTypeByName(parsedProjectType, i->second->m_value))
            {
                std::cerr << "Not a valid project type: \"" << i->second->m_value << "\"\n";
                return false;
            }

            if (firstGameEntry)
            {
                projectType = parsedProjectType;
                firstGameEntry = false;
            }
            else
            {
                if (projectType != parsedProjectType)
                {
                    std::cerr << "Conflicting types in project \"" << projectName << "\": " << PROJECT_TYPE_NAMES[static_cast<unsigned>(projectType)]
                        << " != " << PROJECT_TYPE_NAMES[static_cast<unsigned>(parsedProjectType)] << std::endl;
                    return false;
                }
            }
        }

        if (firstGameEntry)
            projectType = ProjectType::FASTFILE;

        return true;
    }

    static bool GetGameNameFromZoneDefinition(std::string& gameName, const std::string& projectName, const ZoneDefinition& zoneDefinition)
    {
        auto firstGameEntry = true;
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata_lookup.equal_range(METADATA_GAME);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            if (firstGameEntry)
            {
                gameName = i->second->m_value;
                firstGameEntry = false;
            }
            else
            {
                if (gameName != i->second->m_value)
                {
                    std::cout << "Conflicting game names in project \"" << projectName << "\": " << gameName << " != " << i->second << std::endl;
                    return false;
                }
            }
        }

        if (firstGameEntry)
        {
            std::cout << "No game name was specified for project \"" << projectName << "\"" << std::endl;
            return false;
        }

        return true;
    }

    static bool LoadGdtFilesFromZoneDefinition(std::vector<std::unique_ptr<Gdt>>& gdtList, const ZoneDefinition& zoneDefinition, ISearchPath* gdtSearchPath)
    {
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata_lookup.equal_range(METADATA_GDT);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            const auto gdtFile = gdtSearchPath->Open(i->second->m_value + ".gdt");
            if (!gdtFile.IsOpen())
            {
                std::cout << "Failed to open file for gdt \"" << i->second->m_value << "\"" << std::endl;
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

    std::unique_ptr<Zone> CreateZoneForDefinition(const std::string& projectName, ZoneDefinition& zoneDefinition, ISearchPath* assetSearchPath, ISearchPath* gdtSearchPath,
                                                  ISearchPath* sourceSearchPath) const
    {
        const auto context = std::make_unique<ZoneCreationContext>(assetSearchPath, &zoneDefinition);
        if (!ProcessZoneDefinitionIgnores(projectName, *context, sourceSearchPath))
            return nullptr;
        if (!GetGameNameFromZoneDefinition(context->m_game_name, projectName, zoneDefinition))
            return nullptr;
        if (!LoadGdtFilesFromZoneDefinition(context->m_gdt_files, zoneDefinition, gdtSearchPath))
            return nullptr;

        for (const auto* assetLoader : ZONE_CREATORS)
        {
            if (assetLoader->SupportsGame(context->m_game_name))
                return assetLoader->CreateZoneForDefinition(*context);
        }

        return nullptr;
    }

    bool WriteZoneToFile(const std::string& projectName, Zone* zone) const
    {
        const fs::path zoneFolderPath(m_args.GetOutputFolderPathForProject(projectName));
        auto zoneFilePath(zoneFolderPath);
        zoneFilePath.append(zone->m_name + ".ff");

        fs::create_directories(zoneFolderPath);

        std::ofstream stream(zoneFilePath, std::fstream::out | std::fstream::binary);
        if (!stream.is_open())
            return false;

        if (!ZoneWriting::WriteZone(stream, zone))
        {
            std::cout << "Writing zone failed." << std::endl;
            stream.close();
            return false;
        }

        std::cout << "Created zone \"" << zoneFilePath.string() << "\"\n";

        stream.close();
        return true;
    }

    bool BuildFastFile(const std::string& projectName, ZoneDefinition& zoneDefinition, SearchPaths& sourceSearchPaths)
    {
        std::string gameName;
        if (!GetGameNameFromZoneDefinition(gameName, projectName, zoneDefinition))
            return false;

        for (auto& c : gameName)
            c = static_cast<char>(std::tolower(c));

        auto assetSearchPaths = m_search_paths.GetAssetSearchPathsForProject(gameName, projectName);
        auto gdtSearchPaths = m_search_paths.GetGdtSearchPathsForProject(gameName, projectName);

        const auto zone = CreateZoneForDefinition(projectName, zoneDefinition, &assetSearchPaths, &gdtSearchPaths, &sourceSearchPaths);
        auto result = zone != nullptr;
        if (zone)
            result = WriteZoneToFile(projectName, zone.get());

        m_search_paths.UnloadProjectSpecificSearchPaths();

        return result;
    }

    bool BuildProject(const std::string& projectName)
    {
        auto sourceSearchPaths = m_search_paths.GetSourceSearchPathsForProject(projectName);

        const auto zoneDefinition = ReadZoneDefinition(projectName, &sourceSearchPaths);
        if (!zoneDefinition)
            return false;

        ProjectType projectType;
        if (!GetProjectTypeFromZoneDefinition(projectType, projectName, *zoneDefinition))
            return false;

        switch (projectType)
        {
        case ProjectType::FASTFILE:
            return BuildFastFile(projectName, *zoneDefinition, sourceSearchPaths);

        case ProjectType::IPAK:
        default:
            assert(false);
            break;
        }

        return false;
    }

    bool LoadZones()
    {
        for (const auto& zonePath : m_args.m_zones_to_load)
        {
            if (!fs::is_regular_file(zonePath))
            {
                std::cout << "Could not find zone file to load \"" << zonePath << "\".\n";
                return false;
            }

            auto absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

            auto zone = std::unique_ptr<Zone>(ZoneLoading::LoadZone(zonePath));
            if (zone == nullptr)
            {
                std::cout << "Failed to load zone \"" << zonePath << "\".\n";
                return false;
            }

            if (m_args.m_verbose)
            {
                std::cout << "Load zone \"" << zone->m_name << "\"\n";
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
            std::string zoneName = loadedZone->m_name;

            loadedZone.reset();

            if (m_args.m_verbose)
                std::cout << "Unloaded zone \"" << zoneName << "\"\n";
        }
        m_loaded_zones.clear();
    }

public:
    LinkerImpl()
        : m_search_paths(m_args)
    {
    }

    bool Start(const int argc, const char** argv) override
    {
        if (!m_args.ParseArgs(argc, argv))
            return false;

        if (!m_search_paths.BuildProjectIndependentSearchPaths())
            return false;

        if (!LoadZones())
            return false;

        auto result = true;
        for (const auto& projectName : m_args.m_projects_to_build)
        {
            if (!BuildProject(projectName))
            {
                result = false;
                break;
            }
        }

        UnloadZones();

        return result;
    }
};

std::unique_ptr<Linker> Linker::Create()
{
    return std::make_unique<LinkerImpl>();
}
