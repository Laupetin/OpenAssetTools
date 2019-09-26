#pragma once
#include "CommandLineOption.h"
#include <string>
#include <vector>
#include <map>

class ArgumentParser
{
    std::vector<const CommandLineOption*> m_command_line_options;
    std::map<const CommandLineOption*, std::vector<std::string>> m_matched_options;
    std::vector<std::string> m_matched_arguments;

public:
    ArgumentParser(const CommandLineOption** options, size_t optionCount);
    
    bool ParseArguments(std::vector<std::string>& args);
    bool ParseArguments(int argc, const char** argv);

    std::vector<std::string> GetArguments() const;

    bool IsOptionSpecified(const CommandLineOption* option);
    std::string GetValueForOption(const CommandLineOption* option);
    std::vector<std::string> GetParametersForOption(const CommandLineOption* option);
};
