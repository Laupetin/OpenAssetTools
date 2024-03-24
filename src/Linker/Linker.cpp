#include "Linker.h"

#include "Game/IW3/ZoneCreatorIW3.h"
#include "Game/IW4/ZoneCreatorIW4.h"
#include "Game/IW5/ZoneCreatorIW5.h"
#include "Game/T5/ZoneCreatorT5.h"
#include "Game/T6/ZoneCreatorT6.h"
#include "LinkerArgs.h"
#include "LinkerSearchPaths.h"
#include "ObjContainer/IPak/IPakWriter.h"
#include "ObjContainer/IWD/IWD.h"
#include "ObjContainer/SoundBank/SoundBankWriter.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/Arguments/ArgumentParser.h"
#include "Utils/ClassUtils.h"
#include "Utils/ObjFileStream.h"
#include "Utils/StringUtils.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/AssetList/AssetListStream.h"
#include "Zone/Definition/ZoneDefinitionStream.h"
#include "ZoneCreation/IZoneCreator.h"
#include "ZoneCreation/ZoneCreationContext.h"
#include "ZoneLoading.h"
#include "ZoneWriting.h"

#include <deque>
#include <filesystem>
#include <fstream>
#include <regex>
#include <set>

namespace fs = std::filesystem;

const IZoneCreator* const ZONE_CREATORS[]{
    new IW3::ZoneCreator(),
    new IW4::ZoneCreator(),
    new IW5::ZoneCreator(),
    new T5::ZoneCreator(),
    new T6::ZoneCreator(),
};

enum class ProjectType
{
    NONE,
    FASTFILE,
    IPAK,

    MAX
};

constexpr const char* PROJECT_TYPE_NAMES[static_cast<unsigned>(ProjectType::MAX)]{
    "none",
    "fastfile",
    "ipak",
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
                        std::cout << "Could not find zone definition file for project \"" << source << "\".\n";
                        return false;
                    }

                    ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
                    includeDefinition = zoneDefinitionInputStream.ReadDefinition();
                }

                if (!includeDefinition)
                {
                    std::cout << "Failed to read zone definition file for project \"" << source << "\".\n";
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
                    assetList.m_entries.emplace_back(entry.m_asset_type, entry.m_asset_name, entry.m_is_reference);
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

    static bool GetNameFromZoneDefinition(std::string& name, const std::string& targetName, const ZoneDefinition& zoneDefinition)
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
                    std::cout << "Conflicting names in target \"" << targetName << "\": " << name << " != " << i->second << "\n";
                    return false;
                }
            }
        }

        if (firstNameEntry)
            name = targetName;

        return true;
    }

    std::unique_ptr<ZoneDefinition> ReadZoneDefinition(const std::string& targetName, ISearchPath* sourceSearchPath) const
    {
        std::unique_ptr<ZoneDefinition> zoneDefinition;
        {
            const auto definitionFileName = targetName + ".zone";
            const auto definitionStream = sourceSearchPath->Open(definitionFileName);
            if (!definitionStream.IsOpen())
            {
                std::cout << "Could not find zone definition file for target \"" << targetName << "\".\n";
                return nullptr;
            }

            ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
            zoneDefinition = zoneDefinitionInputStream.ReadDefinition();
        }

        if (!zoneDefinition)
        {
            std::cout << "Failed to read zone definition file for target \"" << targetName << "\".\n";
            return nullptr;
        }

        if (!GetNameFromZoneDefinition(zoneDefinition->m_name, targetName, *zoneDefinition))
            return nullptr;

        if (!IncludeAdditionalZoneDefinitions(targetName, *zoneDefinition, sourceSearchPath))
            return nullptr;

        if (!IncludeAssetLists(*zoneDefinition, sourceSearchPath))
            return nullptr;

        return zoneDefinition;
    }

    bool ProcessZoneDefinitionIgnores(const std::string& targetName, ZoneCreationContext& context, ISearchPath* sourceSearchPath) const
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
            if (ignore == targetName)
                continue;

            std::vector<AssetListEntry> assetList;
            if (!ReadAssetList(ignore, context.m_ignored_assets, sourceSearchPath))
            {
                std::cout << "Failed to read asset listing for ignoring assets of project \"" << ignore << "\".\n";
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

    static bool GetProjectTypeFromZoneDefinition(ProjectType& projectType, const std::string& targetName, const ZoneDefinition& zoneDefinition)
    {
        auto firstTypeEntry = true;
        const auto [rangeBegin, rangeEnd] = zoneDefinition.m_metadata_lookup.equal_range(METADATA_TYPE);
        for (auto i = rangeBegin; i != rangeEnd; ++i)
        {
            ProjectType parsedProjectType;
            if (!ProjectTypeByName(parsedProjectType, i->second->m_value))
            {
                std::cerr << "Not a valid project type: \"" << i->second->m_value << "\"\n";
                return false;
            }

            if (firstTypeEntry)
            {
                projectType = parsedProjectType;
                firstTypeEntry = false;
            }
            else
            {
                if (projectType != parsedProjectType)
                {
                    std::cerr << "Conflicting types in target \"" << targetName << "\": " << PROJECT_TYPE_NAMES[static_cast<unsigned>(projectType)]
                              << " != " << PROJECT_TYPE_NAMES[static_cast<unsigned>(parsedProjectType)] << "\n";
                    return false;
                }
            }
        }

        if (firstTypeEntry)
        {
            if (zoneDefinition.m_assets.empty())
                projectType = ProjectType::NONE;
            else
                projectType = ProjectType::FASTFILE;
        }

        return true;
    }

    static bool GetGameNameFromZoneDefinition(std::string& gameName, const std::string& targetName, const ZoneDefinition& zoneDefinition)
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
                    std::cout << "Conflicting game names in target \"" << targetName << "\": " << gameName << " != " << i->second << "\n";
                    return false;
                }
            }
        }

        if (firstGameEntry)
        {
            std::cout << "No game name was specified for target \"" << targetName << "\"\n";
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
                std::cout << "Failed to open file for gdt \"" << i->second->m_value << "\"\n";
                return false;
            }

            GdtReader gdtReader(*gdtFile.m_stream);
            auto gdt = std::make_unique<Gdt>();
            if (!gdtReader.Read(*gdt))
            {
                std::cout << "Failed to read gdt file \"" << i->second << "\"\n";
                return false;
            }

            gdtList.emplace_back(std::move(gdt));
        }

        return true;
    }

    std::unique_ptr<Zone> CreateZoneForDefinition(const std::string& targetName,
                                                  ZoneDefinition& zoneDefinition,
                                                  ISearchPath* assetSearchPath,
                                                  ISearchPath* gdtSearchPath,
                                                  ISearchPath* sourceSearchPath) const
    {
        const auto context = std::make_unique<ZoneCreationContext>(assetSearchPath, &zoneDefinition);
        if (!ProcessZoneDefinitionIgnores(targetName, *context, sourceSearchPath))
            return nullptr;
        if (!GetGameNameFromZoneDefinition(context->m_game_name, targetName, zoneDefinition))
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
            std::cout << "Writing zone failed.\n";
            stream.close();
            return false;
        }

        std::cout << "Created zone \"" << zoneFilePath.string() << "\"\n";

        stream.close();
        return true;
    }

    bool BuildFastFile(const std::string& projectName,
                       const std::string& targetName,
                       ZoneDefinition& zoneDefinition,
                       SearchPaths& assetSearchPaths,
                       SearchPaths& gdtSearchPaths,
                       SearchPaths& sourceSearchPaths) const
    {
        SoundBankWriter::OutputPath = fs::path(m_args.GetOutputFolderPathForProject(projectName));

        const auto zone = CreateZoneForDefinition(targetName, zoneDefinition, &assetSearchPaths, &gdtSearchPaths, &sourceSearchPaths);
        auto result = zone != nullptr;
        if (zone)
            result = WriteZoneToFile(projectName, zone.get());

        return result;
    }

    bool BuildIPak(const std::string& projectName, const ZoneDefinition& zoneDefinition, SearchPaths& assetSearchPaths) const
    {
        const fs::path ipakFolderPath(m_args.GetOutputFolderPathForProject(projectName));
        auto ipakFilePath(ipakFolderPath);
        ipakFilePath.append(zoneDefinition.m_name + ".ipak");

        fs::create_directories(ipakFolderPath);

        std::ofstream stream(ipakFilePath, std::fstream::out | std::fstream::binary);
        if (!stream.is_open())
            return false;

        const auto ipakWriter = IPakWriter::Create(stream, &assetSearchPaths);
        for (const auto& assetEntry : zoneDefinition.m_assets)
        {
            if (assetEntry.m_is_reference)
                continue;

            if (assetEntry.m_asset_type == "image")
                ipakWriter->AddImage(assetEntry.m_asset_name);
        }

        if (!ipakWriter->Write())
        {
            std::cout << "Writing ipak failed.\n";
            stream.close();
            return false;
        }

        std::cout << "Created ipak \"" << ipakFilePath.string() << "\"\n";

        stream.close();
        return true;
    }

    bool BuildReferencedTargets(const std::string& projectName, const std::string& targetName, const ZoneDefinition& zoneDefinition)
    {
        return std::all_of(zoneDefinition.m_targets_to_build.begin(),
                           zoneDefinition.m_targets_to_build.end(),
                           [this, &projectName, &targetName](const std::string& buildTargetName)
                           {
                               if (buildTargetName == targetName)
                               {
                                   std::cerr << "Cannot build target with same name: \"" << targetName << "\"\n";
                                   return false;
                               }

                               std::cout << "Building referenced target \"" << buildTargetName << "\"\n";
                               return BuildProject(projectName, buildTargetName);
                           });
    }

    bool BuildProject(const std::string& projectName, const std::string& targetName)
    {
        auto sourceSearchPaths = m_search_paths.GetSourceSearchPathsForProject(projectName);

        const auto zoneDefinition = ReadZoneDefinition(targetName, &sourceSearchPaths);
        if (!zoneDefinition)
            return false;

        ProjectType projectType;
        if (!GetProjectTypeFromZoneDefinition(projectType, targetName, *zoneDefinition))
            return false;

        auto result = true;
        if (projectType != ProjectType::NONE)
        {
            std::string gameName;
            if (!GetGameNameFromZoneDefinition(gameName, targetName, *zoneDefinition))
                return false;
            utils::MakeStringLowerCase(gameName);

            auto assetSearchPaths = m_search_paths.GetAssetSearchPathsForProject(gameName, projectName);
            auto gdtSearchPaths = m_search_paths.GetGdtSearchPathsForProject(gameName, projectName);

            switch (projectType)
            {
            case ProjectType::FASTFILE:
                result = BuildFastFile(projectName, targetName, *zoneDefinition, assetSearchPaths, gdtSearchPaths, sourceSearchPaths);
                break;

            case ProjectType::IPAK:
                result = BuildIPak(projectName, *zoneDefinition, assetSearchPaths);
                break;

            default:
                assert(false);
                result = false;
                break;
            }
        }

        m_search_paths.UnloadProjectSpecificSearchPaths();

        result = result && BuildReferencedTargets(projectName, targetName, *zoneDefinition);

        return result;
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

    static bool GetProjectAndTargetFromProjectSpecifier(const std::string& projectSpecifier, std::string& projectName, std::string& targetName)
    {
        const auto targetNameSeparatorIndex = projectSpecifier.find_first_of('/');
        if (targetNameSeparatorIndex == std::string::npos)
        {
            projectName = projectSpecifier;
            targetName = projectSpecifier;
        }
        else if (projectSpecifier.find_first_of('/', targetNameSeparatorIndex + 1) != std::string::npos)
        {
            std::cerr << "Project specifier cannot have more than one target name: \"" << projectSpecifier << "\"\n";
            return false;
        }
        else
        {
            projectName = projectSpecifier.substr(0, targetNameSeparatorIndex);
            targetName = projectSpecifier.substr(targetNameSeparatorIndex + 1);
        }

        if (projectName.empty())
        {
            std::cerr << "Project name cannot be empty: \"" << projectSpecifier << "\"\n";
            return false;
        }

        if (targetName.empty())
        {
            std::cerr << "Target name cannot be empty: \"" << projectSpecifier << "\"\n";
            return false;
        }

        return true;
    }

public:
    LinkerImpl()
        : m_search_paths(m_args)
    {
    }

    bool Start(const int argc, const char** argv) override
    {
        auto shouldContinue = true;
        if (!m_args.ParseArgs(argc, argv, shouldContinue))
            return false;

        if (!shouldContinue)
            return true;

        if (!m_search_paths.BuildProjectIndependentSearchPaths())
            return false;

        if (!LoadZones())
            return false;

        auto result = true;
        for (const auto& projectSpecifier : m_args.m_project_specifiers_to_build)
        {
            std::string projectName;
            std::string targetName;
            if (!GetProjectAndTargetFromProjectSpecifier(projectSpecifier, projectName, targetName))
            {
                result = false;
                break;
            }

            if (!BuildProject(projectName, targetName))
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
