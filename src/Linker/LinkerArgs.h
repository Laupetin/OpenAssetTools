#pragma once
#include <vector>
#include <set>
#include <regex>

#include "Utils/ClassUtils.h"
#include "Utils/Arguments/ArgumentParser.h"
#include "Zone/Zone.h"

class LinkerArgs
{
public:
    static constexpr const char* PATTERN_BASE = "?base?";
    static constexpr const char* PATTERN_GAME = "?game?";
    static constexpr const char* PATTERN_PROJECT = "?project?";

    static constexpr const char* DEFAULT_BASE_FOLDER = ".";
    static constexpr const char* DEFAULT_BASE_FOLDER_MOD_TOOLS = "..";
    static constexpr const char* DEFAULT_OUTPUT_FOLDER = "?base?/zone_out/?project?";
    static constexpr const char* DEFAULT_ASSET_SEARCH_PATH = "?base?/raw;?base?/raw/?game?;?base?/zone_raw/?project?";
    static constexpr const char* DEFAULT_GDT_SEARCH_PATH = "?base?/source_data;?base?/zone_raw/?project?/source_data";
    static constexpr const char* DEFAULT_SOURCE_SEARCH_PATH = "?base?/zone_source;?base?/zone_raw/?project?/zone_source";

private:
    ArgumentParser m_argument_parser;
    std::regex m_base_pattern;
    std::regex m_game_pattern;
    std::regex m_project_pattern;

    /**
     * \brief Prints a command line usage help text for the Linker tool to stdout.
     */
    static void PrintUsage();

    void SetVerbose(bool isVerbose);

    _NODISCARD std::string GetBasePathForProject(const std::string& projectName) const;
    void SetDefaultBasePath();
    _NODISCARD std::set<std::string> GetProjectIndependentSearchPaths(const std::set<std::string>& set) const;
    _NODISCARD std::set<std::string> GetSearchPathsForProject(const std::set<std::string>& set, const std::string& gameName, const std::string& projectName) const;

public:
    std::vector<std::string> m_zones_to_load;
    std::vector<std::string> m_project_specifiers_to_build;
    
    std::string m_base_folder;
    std::string m_out_folder;
    bool m_base_folder_depends_on_project;
    bool m_out_folder_depends_on_project;

    std::set<std::string> m_asset_search_paths;
    std::set<std::string> m_gdt_search_paths;
    std::set<std::string> m_source_search_paths;

    bool m_verbose;

    LinkerArgs();
    bool ParseArgs(int argc, const char** argv);

    /**
     * \brief Converts the output path specified by command line arguments to a path applies for the specified project.
     * \param projectName The name of the project to resolve the path input for.
     * \return An output path for the project based on the user input.
     */
    _NODISCARD std::string GetOutputFolderPathForProject(const std::string& projectName) const;

    _NODISCARD std::set<std::string> GetProjectIndependentAssetSearchPaths() const;
    _NODISCARD std::set<std::string> GetProjectIndependentGdtSearchPaths() const;
    _NODISCARD std::set<std::string> GetProjectIndependentSourceSearchPaths() const;
    
    _NODISCARD std::set<std::string> GetAssetSearchPathsForProject(const std::string& gameName, const std::string& projectName) const;
    _NODISCARD std::set<std::string> GetGdtSearchPathsForProject(const std::string& gameName, const std::string& projectName) const;
    _NODISCARD std::set<std::string> GetSourceSearchPathsForProject(const std::string& projectName) const;
};
