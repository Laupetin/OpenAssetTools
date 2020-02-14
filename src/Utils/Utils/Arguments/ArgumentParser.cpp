#include "ArgumentParser.h"
#include <sstream>

const std::string PREFIX_LONG = "--";
const std::string PREFIX_SHORT = "-";

ArgumentParser::ArgumentParser(const CommandLineOption* const* options, const size_t optionCount)
{
    for(unsigned optionIndex = 0; optionIndex < optionCount; optionIndex++)
    {
        m_command_line_options.push_back(options[optionIndex]);
    }
}

bool ArgumentParser::ParseArguments(const int argc, const char** argv)
{
    std::vector<std::string> args(argc);
    
    for(int arg = 0; arg < argc; arg++)
    {
        args[arg] = argv[arg];
    }

    return ParseArguments(args);
}

bool ArgumentParser::ParseArguments(std::vector<std::string>& args)
{
    m_matched_arguments.clear();
    m_matched_options.clear();

    const size_t argCount = args.size();
    for (unsigned argIndex = 0; argIndex < argCount; argIndex++)
    {
        std::string& arg = args[argIndex];

        if(arg.compare(0, PREFIX_SHORT.size(), PREFIX_SHORT) == 0)
        {
            // Options should be case insensitive. So before comparing we make the argument lower case.
            const size_t argStrLen = arg.size();
            for(unsigned argStrIndex = 0; argStrIndex < argStrLen; argStrIndex++)
            {
                arg[argStrIndex] = tolower(arg[argStrIndex]);
            }

            const CommandLineOption* matchedOption = nullptr;
            if(arg.compare(0, PREFIX_LONG.size(), PREFIX_LONG) == 0)
            {
                std::string longName = arg.substr(2);

                for (auto option : m_command_line_options)
                {
                    if(option->m_long_name == longName)
                    {
                        matchedOption = option;
                        break;
                    }
                }
            }
            else
            {
                std::string shortName = arg.substr(1);

                for (auto option : m_command_line_options)
                {
                    if(option->m_short_name == shortName)
                    {
                        matchedOption = option;
                        break;
                    }
                }
            }

            if(matchedOption == nullptr)
            {
                printf("Unknown option '%s'.\n", arg.c_str());
                return false;
            }

            if(m_matched_options.find(matchedOption) != m_matched_options.end())
            {
                if(!matchedOption->m_multi_use)
                {
                    printf("Option '%s' already specified.\n", arg.c_str());
                    return false;
                }
            }
            else
            {
                m_matched_options[matchedOption] = std::vector<std::string>();
            }
            
            const size_t parameterCount = matchedOption->m_parameters.size();
            if(argIndex + parameterCount >= argCount)
            {
                printf("Not enough parameters for option '%s'.\n", arg.c_str());
                return false;
            }

            std::vector<std::string>& parameters = m_matched_options[matchedOption];
            for(unsigned parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++)
            {
                std::string& param = args[argIndex + parameterIndex + 1];

                if(param.compare(0, PREFIX_SHORT.size(), PREFIX_SHORT) == 0)
                {
                    printf("Not enough parameters for option '%s'.\n", arg.c_str());
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

std::string ArgumentParser::GetValueForOption(const CommandLineOption* option)
{
    if(!IsOptionSpecified(option))
        return "";

    std::stringstream value;
    bool firstMatch = true;
    for (const auto& match : m_matched_options[option])
    {
        if(!firstMatch)
        {
            value << " " << match;
        }
        else
        {
            value << match;
            firstMatch = false;
        }
    }

    return value.str();
}

std::vector<std::string> ArgumentParser::GetParametersForOption(const CommandLineOption* option)
{
    if(!IsOptionSpecified(option))
        return std::vector<std::string>();

    return m_matched_options[option];
}

bool ArgumentParser::IsOptionSpecified(const CommandLineOption* option)
{
    return m_matched_options.find(option) != m_matched_options.end();
}