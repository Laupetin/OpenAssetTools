#include "Linker.h"

#include "LinkerArgs.h"
#include "LinkerSearchPaths.h"
#include "ObjContainer/IPak/IPakWriter.h"
#include "ObjContainer/IWD/IWD.h"
#include "ObjContainer/SoundBank/SoundBankWriter.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/ObjFileStream.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/AssetList/AssetListStream.h"
#include "Zone/Definition/ZoneDefinitionStream.h"
#include "ZoneCreation/ZoneCreationContext.h"
#include "ZoneCreation/ZoneCreator.h"
#include "ZoneLoading.h"
#include "ZoneWriting.h"

#include <deque>
#include <filesystem>
#include <format>
#include <fstream>
#include <set>

namespace fs = std::filesystem;

class LinkerImpl final : public Linker
{
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
                    const auto definitionFileName = std::format("{}.zone", source);
                    const auto definitionStream = sourceSearchPath->Open(definitionFileName);
                    if (!definitionStream.IsOpen())
                    {
                        std::cerr << std::format("Could not find zone definition file for project \"{}\".\n", source);
                        return false;
                    }

                    ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
                    zoneDefinitionInputStream.SetPreviouslySetGame(zoneDefinition.m_game);
                    includeDefinition = zoneDefinitionInputStream.ReadDefinition();
                }

                if (!includeDefinition)
                {
                    std::cerr << std::format("Failed to read zone definition file for project \"{}\".\n", source);
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

    bool ReadAssetList(const std::string& zoneName, const GameId game, AssetList& assetList, ISearchPath* sourceSearchPath) const
    {
        {
            const auto assetListFileName = std::format("assetlist/{}.csv", zoneName);
            const auto assetListStream = sourceSearchPath->Open(assetListFileName);

            if (assetListStream.IsOpen())
            {
                const AssetListInputStream stream(*assetListStream.m_stream, game);
                AssetListEntry entry;

                bool failure;
                while (stream.NextEntry(entry, &failure))
                {
                    assetList.m_entries.emplace_back(std::move(entry));
                }

                return !failure;
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
            if (!ReadAssetList(assetListName, zoneDefinition.m_game, assetList, sourceSearchPath))
            {
                std::cerr << std::format("Failed to read asset list \"{}\"\n", assetListName);
                return false;
            }

            zoneDefinition.Include(assetList);
        }

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
                std::cerr << std::format("Could not find zone definition file for target \"{}\".\n", targetName);
                return nullptr;
            }

            ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, definitionFileName, m_args.m_verbose);
            zoneDefinition = zoneDefinitionInputStream.ReadDefinition();
        }

        if (!zoneDefinition)
        {
            std::cerr << std::format("Failed to read zone definition file for target \"{}\".\n", targetName);
            return nullptr;
        }

        // If no type was defined explicitly make it fastfile
        if (zoneDefinition->m_type == ProjectType::NONE)
            zoneDefinition->m_type = ProjectType::FASTFILE;

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

        for (const auto& ignore : context.m_definition->m_ignores)
        {
            if (ignore == targetName)
                continue;

            std::vector<AssetListEntry> assetList;
            if (!ReadAssetList(ignore, context.m_definition->m_game, context.m_ignored_assets, sourceSearchPath))
            {
                std::cerr << std::format("Failed to read asset listing for ignoring assets of project \"{}\".\n", ignore);
                return false;
            }
        }
        return true;
    }

    static bool LoadGdtFilesFromZoneDefinition(std::vector<std::unique_ptr<Gdt>>& gdtList, const ZoneDefinition& zoneDefinition, ISearchPath* gdtSearchPath)
    {
        for (const auto& gdtName : zoneDefinition.m_gdts)
        {
            const auto gdtFile = gdtSearchPath->Open(std::format("{}.gdt", gdtName));
            if (!gdtFile.IsOpen())
            {
                std::cerr << std::format("Failed to open file for gdt \"{}\"\n", gdtName);
                return false;
            }

            GdtReader gdtReader(*gdtFile.m_stream);
            auto gdt = std::make_unique<Gdt>();
            if (!gdtReader.Read(*gdt))
            {
                std::cerr << std::format("Failed to read gdt file \"{}\"\n", gdtName);
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
        const auto context = std::make_unique<ZoneCreationContext>(&zoneDefinition, assetSearchPath);
        if (!ProcessZoneDefinitionIgnores(targetName, *context, sourceSearchPath))
            return nullptr;
        if (!LoadGdtFilesFromZoneDefinition(context->m_gdt_files, zoneDefinition, gdtSearchPath))
            return nullptr;

        const auto* creator = IZoneCreator::GetCreatorForGame(zoneDefinition.m_game);
        return creator->CreateZoneForDefinition(*context);
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

        if (m_args.m_verbose)
        {
            std::cout << std::format("Building zone \"{}\"\n", zoneFilePath.string());
        }

        if (!ZoneWriting::WriteZone(stream, zone))
        {
            std::cerr << "Writing zone failed.\n";
            stream.close();
            return false;
        }

        std::cout << std::format("Created zone \"{}\"\n", zoneFilePath.string());

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
        ipakFilePath.append(std::format("{}.ipak", zoneDefinition.m_name));

        fs::create_directories(ipakFolderPath);

        std::ofstream stream(ipakFilePath, std::fstream::out | std::fstream::binary);
        if (!stream.is_open())
            return false;

        const auto ipakWriter = IPakWriter::Create(stream, &assetSearchPaths);
        const auto imageAssetType = IZoneCreator::GetCreatorForGame(zoneDefinition.m_game)->GetImageAssetType();
        for (const auto& assetEntry : zoneDefinition.m_assets)
        {
            if (assetEntry.m_is_reference)
                continue;

            if (assetEntry.m_asset_type == imageAssetType)
                ipakWriter->AddImage(assetEntry.m_asset_name);
        }

        if (!ipakWriter->Write())
        {
            std::cerr << "Writing ipak failed.\n";
            stream.close();
            return false;
        }

        std::cout << std::format("Created ipak \"{}\"\n", ipakFilePath.string());

        stream.close();
        return true;
    }

    bool BuildReferencedTargets(const std::string& projectName, const std::string& targetName, const ZoneDefinition& zoneDefinition)
    {
        return std::ranges::all_of(zoneDefinition.m_targets_to_build,
                                   [this, &projectName, &targetName](const std::string& buildTargetName)
                                   {
                                       if (buildTargetName == targetName)
                                       {
                                           std::cerr << std::format("Cannot build target with same name: \"{}\"\n", targetName);
                                           return false;
                                       }

                                       std::cout << std::format("Building referenced target \"{}\"\n", buildTargetName);
                                       return BuildProject(projectName, buildTargetName);
                                   });
    }

    bool BuildProject(const std::string& projectName, const std::string& targetName)
    {
        auto sourceSearchPaths = m_search_paths.GetSourceSearchPathsForProject(projectName);

        const auto zoneDefinition = ReadZoneDefinition(targetName, &sourceSearchPaths);
        if (!zoneDefinition)
            return false;

        auto result = true;
        if (zoneDefinition->m_type != ProjectType::NONE)
        {
            const auto& gameName = GameId_Names[static_cast<unsigned>(zoneDefinition->m_game)];
            auto assetSearchPaths = m_search_paths.GetAssetSearchPathsForProject(gameName, projectName);
            auto gdtSearchPaths = m_search_paths.GetGdtSearchPathsForProject(gameName, projectName);

            switch (zoneDefinition->m_type)
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
                std::cerr << std::format("Could not find zone file to load \"{}\".\n", zonePath);
                return false;
            }

            auto zoneDirectory = fs::path(zonePath).remove_filename();
            if (zoneDirectory.empty())
                zoneDirectory = fs::current_path();
            auto absoluteZoneDirectory = absolute(zoneDirectory).string();

            auto zone = std::unique_ptr<Zone>(ZoneLoading::LoadZone(zonePath));
            if (zone == nullptr)
            {
                std::cerr << std::format("Failed to load zone \"{}\".\n", zonePath);
                return false;
            }

            if (m_args.m_verbose)
            {
                std::cout << std::format("Load zone \"{}\"\n", zone->m_name);
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
                std::cout << std::format("Unloaded zone \"{}\"\n", zoneName);
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
            std::cerr << std::format("Project specifier cannot have more than one target name: \"{}\"\n", projectSpecifier);
            return false;
        }
        else
        {
            projectName = projectSpecifier.substr(0, targetNameSeparatorIndex);
            targetName = projectSpecifier.substr(targetNameSeparatorIndex + 1);
        }

        if (projectName.empty())
        {
            std::cerr << std::format("Project name cannot be empty: \"{}\"\n", projectSpecifier);
            return false;
        }

        if (targetName.empty())
        {
            std::cerr << std::format("Target name cannot be empty: \"{}\"\n", projectSpecifier);
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
