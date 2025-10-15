#include "Linker.h"

#include "LinkerArgs.h"
#include "LinkerPaths.h"
#include "ObjContainer/SoundBank/SoundBankWriter.h"
#include "ObjWriting.h"
#include "SearchPath/OutputPathFilesystem.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/Logging/Log.h"
#include "Utils/ObjFileStream.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/AssetList/AssetListReader.h"
#include "Zone/Definition/ZoneDefinitionStream.h"
#include "ZoneCreation/ZoneCreationContext.h"
#include "ZoneCreation/ZoneCreator.h"
#include "ZoneLoading.h"
#include "ZoneWriting.h"

#include <deque>
#include <filesystem>
#include <format>
#include <fstream>
#include <unordered_set>

namespace fs = std::filesystem;

namespace
{
    class LinkerSearchPathContext
    {
    public:
        explicit LinkerSearchPathContext(const ILinkerSearchPathBuilder& searchPathBuilder)
            : m_search_path_builder(searchPathBuilder)
        {
            m_independent_search_paths = m_search_path_builder.BuildIndependentSearchPaths();
            if (m_independent_search_paths)
                m_search_paths.IncludeSearchPath(m_independent_search_paths.get());
        }

        [[nodiscard]] ISearchPath& GetSearchPaths()
        {
            return m_search_paths;
        }

        void LoadProjectSpecific(const std::string& projectName)
        {
            m_project_specific_search_paths = m_search_path_builder.BuildSearchPathsSpecificToProject(projectName);
            if (m_project_specific_search_paths)
                m_search_paths.IncludeSearchPath(m_project_specific_search_paths.get());
        }

        void UnloadProjectSpecific()
        {
            if (!m_project_specific_search_paths)
                return;

            m_search_paths.RemoveSearchPath(m_project_specific_search_paths.get());
            m_project_specific_search_paths.reset();
        }

        void LoadGameSpecific(const std::string& projectName, const GameId game)
        {
            m_game_specific_search_paths = m_search_path_builder.BuildSearchPathsSpecificToProjectAndGame(projectName, game);
            if (m_game_specific_search_paths)
                m_search_paths.IncludeSearchPath(m_game_specific_search_paths.get());
        }

        void UnloadGameSpecific()
        {
            if (!m_game_specific_search_paths)
                return;

            m_search_paths.RemoveSearchPath(m_game_specific_search_paths.get());
            m_game_specific_search_paths.reset();
        }

    private:
        const ILinkerSearchPathBuilder& m_search_path_builder;
        std::unique_ptr<ISearchPath> m_independent_search_paths;
        std::unique_ptr<ISearchPath> m_project_specific_search_paths;
        std::unique_ptr<ISearchPath> m_game_specific_search_paths;
        SearchPaths m_search_paths;
    };

    class LinkerPathManager
    {
    public:
        explicit LinkerPathManager(const LinkerArgs& args)
            : m_linker_paths(ILinkerPaths::FromArgs(args)),
              m_asset_paths(m_linker_paths->AssetSearchPaths()),
              m_gdt_paths(m_linker_paths->GdtSearchPaths()),
              m_source_paths(m_linker_paths->SourceSearchPaths())
        {
        }

        std::unique_ptr<ILinkerPaths> m_linker_paths;
        LinkerSearchPathContext m_asset_paths;
        LinkerSearchPathContext m_gdt_paths;
        LinkerSearchPathContext m_source_paths;
    };

    class PathProjectContext
    {
    public:
        PathProjectContext(LinkerPathManager& paths, const std::string& projectName)
            : m_paths(paths)
        {
            m_paths.m_asset_paths.LoadProjectSpecific(projectName);
            m_paths.m_gdt_paths.LoadProjectSpecific(projectName);
            m_paths.m_source_paths.LoadProjectSpecific(projectName);
        }

        ~PathProjectContext()
        {
            m_paths.m_asset_paths.UnloadProjectSpecific();
            m_paths.m_gdt_paths.UnloadProjectSpecific();
            m_paths.m_source_paths.UnloadProjectSpecific();
        }

        PathProjectContext(const PathProjectContext& other) = delete;
        PathProjectContext(PathProjectContext&& other) noexcept = delete;
        PathProjectContext& operator=(const PathProjectContext& other) = delete;
        PathProjectContext& operator=(PathProjectContext&& other) noexcept = delete;

    private:
        LinkerPathManager& m_paths;
    };

    class PathGameContext
    {
    public:
        PathGameContext(LinkerPathManager& paths, const std::string& projectName, const GameId game)
            : m_paths(paths)
        {
            m_paths.m_asset_paths.LoadGameSpecific(projectName, game);
            m_paths.m_gdt_paths.LoadGameSpecific(projectName, game);
            m_paths.m_source_paths.LoadGameSpecific(projectName, game);
        }

        ~PathGameContext()
        {
            m_paths.m_asset_paths.UnloadGameSpecific();
            m_paths.m_gdt_paths.UnloadGameSpecific();
            m_paths.m_source_paths.UnloadGameSpecific();
        }

        PathGameContext(const PathGameContext& other) = delete;
        PathGameContext(PathGameContext&& other) noexcept = delete;
        PathGameContext& operator=(const PathGameContext& other) = delete;
        PathGameContext& operator=(PathGameContext&& other) noexcept = delete;

    private:
        LinkerPathManager& m_paths;
    };
} // namespace

class LinkerImpl final : public Linker
{
    std::unique_ptr<ZoneDefinition> ReadZoneDefinition(LinkerPathManager& paths, const std::string& targetName, bool logMissing = true) const
    {
        auto& sourceSearchPath = paths.m_source_paths.GetSearchPaths();
        std::unique_ptr<ZoneDefinition> zoneDefinition;
        {
            const auto definitionFileName = std::format("{}.zone", targetName);
            const auto definitionStream = sourceSearchPath.Open(definitionFileName);
            if (!definitionStream.IsOpen())
            {
                if (logMissing)
                    con::error("Could not find zone definition file for target \"{}\".", targetName);
                return nullptr;
            }

            ZoneDefinitionInputStream zoneDefinitionInputStream(*definitionStream.m_stream, targetName, definitionFileName, sourceSearchPath);
            zoneDefinition = zoneDefinitionInputStream.ReadDefinition();
        }

        if (!zoneDefinition)
        {
            con::error("Failed to read zone definition file for target \"{}\".", targetName);
            return nullptr;
        }

        return zoneDefinition;
    }

    bool ReadIgnoreEntries(LinkerPathManager& paths, const std::string& zoneName, const GameId game, AssetList& assetList) const
    {
        {
            AssetListReader assetListReader(paths.m_source_paths.GetSearchPaths(), game);
            const auto maybeReadAssetList = assetListReader.ReadAssetList(zoneName, false);
            if (maybeReadAssetList)
            {
                assetList.m_entries.reserve(assetList.m_entries.size() + maybeReadAssetList->m_entries.size());
                for (auto& entry : maybeReadAssetList->m_entries)
                    assetList.m_entries.emplace_back(std::move(entry));

                return true;
            }
        }

        {
            const auto zoneDefinition = ReadZoneDefinition(paths, zoneName, false);

            if (zoneDefinition)
            {
                assetList.m_entries.reserve(assetList.m_entries.size() + zoneDefinition->m_assets.size());
                for (const auto& entry : zoneDefinition->m_assets)
                    assetList.m_entries.emplace_back(entry.m_asset_type, entry.m_asset_name, entry.m_is_reference);

                return true;
            }
        }

        return false;
    }

    bool ProcessZoneDefinitionIgnores(LinkerPathManager& paths, const std::string& targetName, ZoneCreationContext& context) const
    {
        if (context.m_definition->m_ignores.empty())
            return true;

        for (const auto& ignore : context.m_definition->m_ignores)
        {
            if (ignore == targetName)
                continue;

            if (!ReadIgnoreEntries(paths, ignore, context.m_definition->m_game, context.m_ignored_assets))
            {
                con::error("Failed to read asset listing for ignoring assets of project \"{}\".", ignore);
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
                con::error("Failed to open file for gdt \"{}\"", gdtName);
                return false;
            }

            GdtReader gdtReader(*gdtFile.m_stream);
            auto gdt = std::make_unique<Gdt>();
            if (!gdtReader.Read(*gdt))
            {
                con::error("Failed to read gdt file \"{}\"", gdtName);
                return false;
            }

            gdtList.emplace_back(std::move(gdt));
        }

        return true;
    }

    std::unique_ptr<Zone> CreateZoneForDefinition(
        LinkerPathManager& paths, const fs::path& outDir, const fs::path& cacheDir, const std::string& targetName, ZoneDefinition& zoneDefinition) const
    {
        ZoneCreationContext context(&zoneDefinition, &paths.m_asset_paths.GetSearchPaths(), outDir, cacheDir);
        if (!ProcessZoneDefinitionIgnores(paths, targetName, context))
            return nullptr;
        if (!LoadGdtFilesFromZoneDefinition(context.m_gdt_files, zoneDefinition, &paths.m_gdt_paths.GetSearchPaths()))
            return nullptr;

        return zone_creator::CreateZoneForDefinition(zoneDefinition.m_game, context);
    }

    static bool WriteZoneToFile(IOutputPath& outPath, const Zone& zone)
    {
        const auto stream = outPath.Open(std::format("{}.ff", zone.m_name));
        if (!stream)
        {
            con::error("Failed to open file for zone: {}", zone.m_name);
            return false;
        }

        con::info("Building zone \"{}\"", zone.m_name);

        if (!ZoneWriting::WriteZone(*stream, zone))
        {
            con::error("Writing zone failed.");
            return false;
        }

        con::info("Created zone \"{}\"", zone.m_name);

        return true;
    }

    bool BuildFastFile(LinkerPathManager& paths, const std::string& projectName, const std::string& targetName, ZoneDefinition& zoneDefinition) const
    {
        const fs::path outDir(paths.m_linker_paths->BuildOutputFolderPath(projectName, zoneDefinition.m_game));

        OutputPathFilesystem outputPath(outDir);

        const fs::path cacheDir(paths.m_linker_paths->BuildCacheFolderPath(projectName, zoneDefinition.m_game));
        SoundBankWriter::OutputPath = outDir;

        const auto zone = CreateZoneForDefinition(paths, outDir, cacheDir, targetName, zoneDefinition);
        auto result = zone != nullptr;
        if (zone)
            result = WriteZoneToFile(outputPath, *zone);

        return result;
    }

    bool BuildProject(LinkerPathManager& paths, const std::string& projectName, const std::string& targetName) const
    {
        std::deque<std::string> targetsToBuild;
        std::unordered_set<std::string> alreadyBuiltTargets;

        targetsToBuild.emplace_back(targetName);

        while (!targetsToBuild.empty())
        {
            const auto currentTarget = std::move(targetsToBuild.front());
            targetsToBuild.pop_front();
            alreadyBuiltTargets.emplace(currentTarget);

            PathProjectContext projectContext(paths, projectName);

            const auto zoneDefinition = ReadZoneDefinition(paths, targetName);
            if (!zoneDefinition)
                return false;

            PathGameContext gameContext(paths, projectName, zoneDefinition->m_game);

            if (!zoneDefinition->m_assets.empty())
            {
                if (!BuildFastFile(paths, projectName, targetName, *zoneDefinition))
                    return false;

                for (const auto& referencedTarget : zoneDefinition->m_targets_to_build)
                {
                    if (alreadyBuiltTargets.find(referencedTarget) == alreadyBuiltTargets.end())
                    {
                        targetsToBuild.emplace_back(referencedTarget);
                        con::info("Building referenced target \"{}\"", referencedTarget);
                    }
                }
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
                con::error("Could not find zone file to load \"{}\".", zonePath);
                return false;
            }

            auto zoneDirectory = fs::path(zonePath).remove_filename();
            if (zoneDirectory.empty())
                zoneDirectory = fs::current_path();
            auto absoluteZoneDirectory = absolute(zoneDirectory).string();

            auto maybeZone = ZoneLoading::LoadZone(zonePath, std::nullopt);
            if (!maybeZone)
            {
                con::error("Failed to load zone \"{}\": {}", zonePath, maybeZone.error());
                return false;
            }

            auto zone = std::move(*maybeZone);

            con::debug("Loaded zone \"{}\"", zone->m_name);

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

            con::debug("Unloaded zone \"{}\"", zoneName);
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
            con::error("Project specifier cannot have more than one target name: \"{}\"", projectSpecifier);
            return false;
        }
        else
        {
            projectName = projectSpecifier.substr(0, targetNameSeparatorIndex);
            targetName = projectSpecifier.substr(targetNameSeparatorIndex + 1);
        }

        if (projectName.empty())
        {
            con::error("Project name cannot be empty: \"{}\"", projectSpecifier);
            return false;
        }

        if (targetName.empty())
        {
            con::error("Target name cannot be empty: \"{}\"", projectSpecifier);
            return false;
        }

        return true;
    }

public:
    bool Start(const int argc, const char** argv) override
    {
        auto shouldContinue = true;
        if (!m_args.ParseArgs(argc, argv, shouldContinue))
            return false;

        if (!shouldContinue)
            return true;

        LinkerPathManager paths(m_args);

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

            if (!BuildProject(paths, projectName, targetName))
            {
                result = false;
                break;
            }
        }

        UnloadZones();

        return result;
    }

private:
    LinkerArgs m_args;
    std::vector<std::unique_ptr<Zone>> m_loaded_zones;
};

std::unique_ptr<Linker> Linker::Create()
{
    return std::make_unique<LinkerImpl>();
}
