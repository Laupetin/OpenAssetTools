#include "UsageInformation.h"

#include <iomanip>
#include <iostream>
#include <map>

UsageInformation::ArgumentUsage::ArgumentUsage(std::string name, const bool optional)
{
    m_name = std::move(name);
    m_optional = optional;
}

UsageInformation::UsageInformation(std::string applicationName)
{
    m_application_name = std::move(applicationName);
    m_var_args = false;
}

void UsageInformation::AddCommandLineOption(const CommandLineOption* option)
{
    m_command_line_options.push_back(option);
}

void UsageInformation::AddArgument(std::string argumentName)
{
    m_arguments.emplace_back(std::move(argumentName), false);
}

void UsageInformation::AddOptionalArgument(std::string argumentName)
{
    m_arguments.emplace_back(std::move(argumentName), true);
}

void UsageInformation::SetVariableArguments(const bool enabled)
{
    m_var_args = enabled;
}

void UsageInformation::Print()
{
    std::stringstream str;
    std::map<std::string, std::vector<const CommandLineOption*>> categories;

    str << "Usage:" << std::endl;

    PrintUsageOverview(str);

    str << "The following options are available:" << std::endl;

    str.fill(' ');

    for (auto commandLineOption : m_command_line_options)
    {
        if (categories.find(commandLineOption->m_category) == categories.end())
            categories[commandLineOption->m_category] = std::vector<const CommandLineOption*>();

        categories[commandLineOption->m_category].push_back(commandLineOption);
    }

    size_t longestShortName = 0;
    size_t longestLongName = 0;
    size_t longestArgumentLength = 0;

    for (auto option : m_command_line_options)
    {
        if (option->m_short_name.length() > longestShortName)
            longestShortName = option->m_short_name.length();

        if (option->m_long_name.length() > longestLongName)
            longestLongName = option->m_long_name.length();

        const size_t argumentLength = GetOptionArgumentLength(option);
        if (argumentLength > longestArgumentLength)
            longestArgumentLength = argumentLength;
    }

    bool firstCategory = true;
    for (auto& category : categories)
    {
        if (!firstCategory)
            str << std::endl;
        else
            firstCategory = false;

        if (!category.first.empty())
        {
            str << "== " << category.first << " ==" << std::endl;
        }

        for (auto option : category.second)
        {
            str << std::setw(static_cast<std::streamsize>(longestShortName) + 1) << (!option->m_short_name.empty() ? "-" + option->m_short_name : "");

            if (!option->m_short_name.empty() && !option->m_long_name.empty())
                str << ", ";
            else
                str << "  ";

            str << std::setw(static_cast<std::streamsize>(longestLongName) + 2) << (!option->m_long_name.empty() ? "--" + option->m_long_name : "");

            str << " ";
            str << std::setw(longestArgumentLength) << GetOptionArgument(option);

            str << " " << option->m_description << std::endl;
        }
    }

    std::cout << str.str() << std::endl;
}

void UsageInformation::PrintUsageOverview(std::stringstream& str)
{
    str << m_application_name << " [options]";
    for (ArgumentUsage& argument : m_arguments)
    {
        if (argument.m_optional)
        {
            str << " [" << argument.m_name << "]";
        }
        else
        {
            str << " <" << argument.m_name << ">";
        }
    }
    if (m_var_args)
    {
        str << "...";
    }
    str << std::endl << std::endl;
}

std::string UsageInformation::GetOptionArgument(const CommandLineOption* option)
{
    std::stringstream str;

    bool firstParam = true;
    for (const std::string& param : option->m_parameters)
    {
        if (!firstParam)
            str << " ";
        else
            firstParam = false;

        str << "<" << param << ">";
    }

    return str.str();
}

size_t UsageInformation::GetOptionArgumentLength(const CommandLineOption* option)
{
    const size_t parameterCount = option->m_parameters.size();
    size_t parameterCombinedStringLength = 0;

    for (const std::string& param : option->m_parameters)
    {
        parameterCombinedStringLength += param.length();
    }

    return parameterCount * 2                                                               // < and >
           + parameterCombinedStringLength + (parameterCount > 1 ? parameterCount - 1 : 0); // One space between each argument
}
