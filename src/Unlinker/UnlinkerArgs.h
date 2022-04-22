#pragma once
#include <regex>
#include <set>
#include <vector>
#include <string>
#include <unordered_map>

#include "Utils/Arguments/ArgumentParser.h"
#include "Zone/Zone.h"

class UnlinkerArgs
{
public:
    static constexpr const char* DEFAULT_OUTPUT_FOLDER = "zone_dump/zone_raw/?zone?";

private:
    ArgumentParser m_argument_parser;
    std::regex m_zone_pattern;

    /**
     * \brief Prints a command line usage help text for the Unlinker tool to stdout.
     */
    static void PrintUsage();

    void SetVerbose(bool isVerbose);
    bool SetImageDumpingMode();
    bool SetModelDumpingMode();

    void AddSpecifiedAssetType(std::string value);
    void ParseCommaSeparatedAssetTypeString(const std::string& input);

public:
    enum class ProcessingTask
    {
        DUMP,
        LIST
    };

    enum class AssetTypeHandling
    {
        EXCLUDE,
        INCLUDE
    };

    std::vector<std::string> m_zones_to_load;
    std::vector<std::string> m_zones_to_unlink;
    std::set<std::string> m_user_search_paths;

    ProcessingTask m_task;
    std::string m_output_folder;
    bool m_minimal_zone_def;

    std::vector<std::string> m_specified_asset_types;
    std::unordered_map<std::string, size_t> m_specified_asset_type_map;
    AssetTypeHandling m_asset_type_handling;

    bool m_skip_obj;
    bool m_use_gdt;

    bool m_verbose;

    UnlinkerArgs();
    bool ParseArgs(int argc, const char** argv);

    /**
     * \brief Converts the output path specified by command line arguments to a path applies for the specified zone.
     * \param zone The zone to resolve the path input for.
     * \return An output path for the zone based on the user input.
     */
    std::string GetOutputFolderPathForZone(const Zone* zone) const;
};
