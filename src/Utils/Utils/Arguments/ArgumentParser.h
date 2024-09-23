#pragma once
#include "CommandLineOption.h"

#include <map>
#include <string>
#include <vector>

class ArgumentParser
{
public:
    ArgumentParser(const CommandLineOption* const* options, size_t optionCount);

    bool ParseArguments(std::vector<std::string>& args);
    bool ParseArguments(int argc, const char** argv);

    [[nodiscard]] std::vector<std::string> GetArguments() const;

    bool IsOptionSpecified(const CommandLineOption* option) const;
    std::string GetValueForOption(const CommandLineOption* option) const;
    std::vector<std::string> GetParametersForOption(const CommandLineOption* option) const;

private:
    std::vector<const CommandLineOption*> m_command_line_options;
    std::map<const CommandLineOption*, std::vector<std::string>> m_matched_options;
    std::vector<std::string> m_matched_arguments;
    std::string m_path;
};
