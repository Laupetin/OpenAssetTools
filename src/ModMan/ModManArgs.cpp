#include "ModManArgs.h"

#include "GitVersion.h"
#include "Utils/Arguments/UsageInformation.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <type_traits>

namespace
{
    // clang-format off
    const CommandLineOption* const OPTION_HELP =
        CommandLineOption::Builder::Create()
        .WithShortName("?")
        .WithLongName("help")
        .WithDescription("Displays usage information.")
        .Build();

    const CommandLineOption* const OPTION_VERSION =
        CommandLineOption::Builder::Create()
        .WithLongName("version")
        .WithDescription("Prints the application version.")
        .Build();

    const CommandLineOption* const OPTION_VERBOSE =
        CommandLineOption::Builder::Create()
        .WithShortName("v")
        .WithLongName("verbose")
        .WithDescription("Outputs a lot more and more detailed messages.")
        .Build();

    const CommandLineOption* const OPTION_NO_COLOR =
        CommandLineOption::Builder::Create()
        .WithLongName("no-color")
        .WithDescription("Disables colored terminal output.")
        .Build();
    // clang-format on

    const CommandLineOption* const COMMAND_LINE_OPTIONS[]{
        OPTION_HELP,
        OPTION_VERSION,
        OPTION_VERBOSE,
        OPTION_NO_COLOR,
    };
} // namespace

ModManArgs::ModManArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>)
{
}

void ModManArgs::PrintUsage() const
{
    UsageInformation usage(m_argument_parser.GetExecutableName());

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.Print();
}

void ModManArgs::PrintVersion()
{
    con::info("OpenAssetTools ModMan {}", GIT_VERSION);
}

bool ModManArgs::ParseArgs(const int argc, const char** argv, bool& shouldContinue)
{
    shouldContinue = true;
    if (!m_argument_parser.ParseArguments(argc, argv))
    {
        PrintUsage();
        return false;
    }

    // Check if the user requested help
    if (m_argument_parser.IsOptionSpecified(OPTION_HELP))
    {
        PrintUsage();
        shouldContinue = false;
        return true;
    }

    // Check if the user wants to see the version
    if (m_argument_parser.IsOptionSpecified(OPTION_VERSION))
    {
        PrintVersion();
        shouldContinue = false;
        return true;
    }

    // -v; --verbose
    if (m_argument_parser.IsOptionSpecified(OPTION_VERBOSE))
        con::set_log_level(con::LogLevel::DEBUG);
    else
        con::set_log_level(con::LogLevel::INFO);

    // --no-color
    con::set_use_color(!m_argument_parser.IsOptionSpecified(OPTION_NO_COLOR));

    return true;
}
