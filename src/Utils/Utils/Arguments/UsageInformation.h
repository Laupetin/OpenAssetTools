#pragma once
#include "CommandLineOption.h"

#include <sstream>
#include <string>
#include <vector>

class UsageInformation
{
    class ArgumentUsage
    {
    public:
        std::string m_name;
        bool m_optional;

        ArgumentUsage(std::string name, bool optional);
    };

    std::string m_application_name;
    std::vector<const CommandLineOption*> m_command_line_options;
    std::vector<ArgumentUsage> m_arguments;
    bool m_var_args;

    void PrintUsageOverview(std::stringstream& str);

    static std::string GetOptionArgument(const CommandLineOption* option);
    static size_t GetOptionArgumentLength(const CommandLineOption* option);

public:
    UsageInformation(std::string applicationName);

    void AddCommandLineOption(const CommandLineOption* option);
    void AddArgument(std::string argumentName);
    void AddOptionalArgument(std::string argumentName);
    void SetVariableArguments(bool enabled);

    void Print();
};
