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
    static constexpr const char* PATTERN_ZONE = "?zone?";

    static constexpr const char* DEFAULT_BASE_FOLDER = ".";
    static constexpr const char* DEFAULT_BASE_FOLDER_MOD_TOOLS = "..";
    static constexpr const char* DEFAULT_OUTPUT_FOLDER = "?base?/zone_out/?zone?";
    static constexpr const char* DEFAULT_ASSET_SEARCH_PATH = "?base?/raw;?base?/zone_raw/?zone?";
    static constexpr const char* DEFAULT_GDT_SEARCH_PATH = "?base?/source_data;?base?/zone_raw/?zone?/source_data";
    static constexpr const char* DEFAULT_SOURCE_SEARCH_PATH = "?base?/zone_source;?base?/zone_raw/?zone?/zone_source";

private:
    ArgumentParser m_argument_parser;
    std::regex m_base_pattern;
    std::regex m_zone_pattern;

    /**
     * \brief Prints a command line usage help text for the Linker tool to stdout.
     */
    static void PrintUsage();

    void SetVerbose(bool isVerbose);

    _NODISCARD std::string GetBasePathForZone(const std::string& zoneName) const;
    void SetDefaultBasePath();
    _NODISCARD std::set<std::string> GetZoneIndependentSearchPaths(const std::set<std::string>& set) const;
    _NODISCARD std::set<std::string> GetSearchPathsForZone(const std::set<std::string>& set, const std::string& zoneName) const;

public:
    std::vector<std::string> m_zones_to_load;
    std::vector<std::string> m_zones_to_build;
    
    std::string m_base_folder;
    std::string m_out_folder;
    bool m_base_folder_depends_on_zone;
    bool m_out_folder_depends_on_zone;

    std::set<std::string> m_asset_search_paths;
    std::set<std::string> m_gdt_search_paths;
    std::set<std::string> m_source_search_paths;

    bool m_verbose;

    LinkerArgs();
    bool ParseArgs(int argc, const char** argv);

    /**
     * \brief Converts the output path specified by command line arguments to a path applies for the specified zone.
     * \param zoneName The name of the zone to resolve the path input for.
     * \return An output path for the zone based on the user input.
     */
    _NODISCARD std::string GetOutputFolderPathForZone(const std::string& zoneName) const;

    _NODISCARD std::set<std::string> GetZoneIndependentAssetSearchPaths() const;
    _NODISCARD std::set<std::string> GetZoneIndependentGdtSearchPaths() const;
    _NODISCARD std::set<std::string> GetZoneIndependentSourceSearchPaths() const;
    
    _NODISCARD std::set<std::string> GetAssetSearchPathsForZone(const std::string& zoneName) const;
    _NODISCARD std::set<std::string> GetGdtSearchPathsForZone(const std::string& zoneName) const;
    _NODISCARD std::set<std::string> GetSourceSearchPathsForZone(const std::string& zoneName) const;
};
