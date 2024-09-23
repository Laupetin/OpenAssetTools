#include "ArgumentParser.h"

#include "Utils/StringUtils.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

constexpr auto PREFIX_LONG = "--";
constexpr auto PREFIX_SHORT = "-";

ArgumentParser::ArgumentParser(const CommandLineOption* const* options, const size_t optionCount)
{
    for (unsigned optionIndex = 0; optionIndex < optionCount; optionIndex++)
    {
        m_command_line_options.push_back(options[optionIndex]);
    }
}

bool ArgumentParser::ParseArguments(const int argc, const char** argv)
{
    std::vector<std::string> args(argc);

    for (int arg = 0; arg < argc; arg++)
        args[arg] = argv[arg];

    return ParseArguments(args);
}

bool ArgumentParser::ParseArguments(std::vector<std::string>& args)
{
    m_matched_arguments.clear();
    m_matched_options.clear();

    if (args.empty())
        return false;

    m_path = args[0];

    const auto argCount = args.size();
    for (unsigned argIndex = 1u; argIndex < argCount; argIndex++)
    {
        auto& arg = args[argIndex];

        if (arg.compare(0, std::char_traits<char>::length(PREFIX_SHORT), PREFIX_SHORT) == 0)
        {
            // Options should be case-insensitive. So before comparing we make the argument lower case.
            utils::MakeStringLowerCase(arg);

            const CommandLineOption* matchedOption = nullptr;
            if (arg.compare(0, std::char_traits<char>::length(PREFIX_LONG), PREFIX_LONG) == 0)
            {
                const auto longName = arg.substr(2);

                for (const auto& option : m_command_line_options)
                {
                    if (option->m_long_name == longName)
                    {
                        matchedOption = option;
                        break;
                    }
                }
            }
            else
            {
                const auto shortName = arg.substr(1);

                for (const auto& option : m_command_line_options)
                {
                    if (option->m_short_name == shortName)
                    {
                        matchedOption = option;
                        break;
                    }
                }
            }

            if (matchedOption == nullptr)
            {
                std::cout << std::format("Unknown option '{}'.\n", arg);
                return false;
            }

            if (m_matched_options.find(matchedOption) != m_matched_options.end())
            {
                if (!matchedOption->m_multi_use)
                {
                    std::cout << std::format("Option '{}' already specified.\n", arg);
                    return false;
                }
            }
            else
            {
                m_matched_options[matchedOption] = std::vector<std::string>();
            }

            const auto parameterCount = matchedOption->m_parameters.size();
            if (argIndex + parameterCount >= argCount)
            {
                std::cout << std::format("Not enough parameters for option '{}'.\n", arg);
                return false;
            }

            auto& parameters = m_matched_options[matchedOption];
            for (unsigned parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++)
            {
                std::string& param = args[argIndex + parameterIndex + 1];

                if (param.compare(0, std::char_traits<char>::length(PREFIX_SHORT), PREFIX_SHORT) == 0)
                {
                    std::cout << std::format("Not enough parameters for option '{}'.\n", arg);
                    return false;
                }

                parameters.push_back(param);
            }

            argIndex += parameterCount;
        }
        else
        {
            m_matched_arguments.push_back(arg);
        }
    }

    return true;
}

std::vector<std::string> ArgumentParser::GetArguments() const
{
    return m_matched_arguments;
}

std::string ArgumentParser::GetValueForOption(const CommandLineOption* option) const
{
    const auto existingOption = m_matched_options.find(option);
    if (existingOption == m_matched_options.end())
        return "";

    std::stringstream value;
    bool firstMatch = true;
    for (const auto& match : existingOption->second)
    {
        if (!firstMatch)
        {
            value << ' ' << match;
        }
        else
        {
            value << match;
            firstMatch = false;
        }
    }

    return value.str();
}

std::vector<std::string> ArgumentParser::GetParametersForOption(const CommandLineOption* option) const
{
    const auto existingOption = m_matched_options.find(option);
    if (existingOption == m_matched_options.end())
        return std::vector<std::string>();

    return existingOption->second;
}

bool ArgumentParser::IsOptionSpecified(const CommandLineOption* option) const
{
    return m_matched_options.find(option) != m_matched_options.end();
}

std::string ArgumentParser::GetExecutableName() const
{
    return fs::path(m_path).filename().string();
}
