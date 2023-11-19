#pragma once

#include "Utils/Arguments/ArgumentParser.h"

#include <string>
#include <utility>
#include <vector>

class RawTemplaterArguments
{
    ArgumentParser m_argument_parser;

    /**
     * \brief Prints a command line usage help text for the RawTemplater tool to stdout.
     */
    static void PrintUsage();

public:
    bool m_verbose;

    std::vector<std::string> m_input_files;
    std::string m_output_directory;

    std::string m_build_log_file;

    std::vector<std::pair<std::string, std::string>> m_defines;

    RawTemplaterArguments();

    bool Parse(int argc, const char** argv);
};
