#pragma once
#include "CommandLineOption.h"
#include <string>
#include <vector>

class ArgumentParser
{
public:
    ArgumentParser(const CommandLineOption** options, size_t optionCount);
    
    bool ParseArguments(std::vector<std::string>& args);
    bool ParseArguments(int argc, const char** argv);

    std::vector<std::string> GetArguments();

    bool IsOptionSpecified(const CommandLineOption* option);
    std::string GetValueForOption(const CommandLineOption* option);
    std::vector<std::string> GetParametersForOption(const CommandLineOption* option);
};