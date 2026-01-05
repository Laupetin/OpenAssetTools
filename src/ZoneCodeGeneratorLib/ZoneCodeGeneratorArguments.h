#pragma once

#include "Utils/Arguments/ArgumentParser.h"

#include <string>
#include <vector>

class ZoneCodeGeneratorArguments
{
public:
    ZoneCodeGeneratorArguments();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    [[nodiscard]] bool ShouldGenerate() const;
    [[nodiscard]] bool ShouldPrint() const;

    std::vector<std::string> m_header_paths;
    std::vector<std::string> m_command_paths;
    std::string m_output_directory;

    std::vector<std::string> m_template_names;

private:
    void PrintUsage() const;
    static void PrintVersion();

    ArgumentParser m_argument_parser;
    unsigned m_task_flags;
};
