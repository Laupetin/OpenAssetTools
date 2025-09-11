#pragma once
#include "Utils/Arguments/ArgumentParser.h"

#include <set>
#include <vector>

class LinkerArgs
{
public:
    static constexpr auto DEFAULT_BASE_FOLDER = ".";
    static constexpr auto DEFAULT_CACHE_FOLDER = "?base?/.oat/cache/?project?";
    static constexpr auto DEFAULT_OUTPUT_FOLDER = "?base?/zone_out/?project?";
    static constexpr auto DEFAULT_ASSET_SEARCH_PATH = "?bin?/raw/?game?;?base?/raw;?base?/raw/?game?;?base?/zone_raw/?project?";
    static constexpr auto DEFAULT_GDT_SEARCH_PATH = "?base?/source_data;?base?/zone_raw/?project?/source_data";
    static constexpr auto DEFAULT_SOURCE_SEARCH_PATH = "?base?/zone_source;?base?/zone_raw/?project?;?base?/zone_raw/?project?/zone_source";

    LinkerArgs();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    std::vector<std::string> m_zones_to_load;
    std::vector<std::string> m_project_specifiers_to_build;

    std::string m_bin_folder;
    std::string m_base_folder;
    std::string m_out_folder;

    std::set<std::string> m_asset_search_paths;
    std::set<std::string> m_gdt_search_paths;
    std::set<std::string> m_source_search_paths;

private:
    /**
     * \brief Prints a command line usage help text for the Linker tool to stdout.
     */
    void PrintUsage() const;
    static void PrintVersion();

    void SetBinFolder();

    ArgumentParser m_argument_parser;
};
