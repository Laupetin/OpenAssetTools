#pragma once
#include <vector>
#include <set>

#include "Utils/ClassUtils.h"
#include "Utils/Arguments/ArgumentParser.h"
#include "Zone/Zone.h"

class LinkerArgs
{
    ArgumentParser m_argument_parser;

    /**
     * \brief Prints a command line usage help text for the Linker tool to stdout.
     */
    static void PrintUsage();

    void SetVerbose(bool isVerbose);

public:
    std::vector<std::string> m_zones_to_load;
    std::vector<std::string> m_zones_to_build;
    std::set<std::string> m_user_search_paths;
    
    std::string m_output_folder;

    bool m_verbose;

    LinkerArgs();
    bool ParseArgs(int argc, const char** argv);

    /**
     * \brief Converts the output path specified by command line arguments to a path applies for the specified zone.
     * \param zoneName The name of the zone to resolve the path input for.
     * \return An output path for the zone based on the user input.
     */
    _NODISCARD std::string GetOutputFolderPathForZone(const std::string& zoneName) const;
};
