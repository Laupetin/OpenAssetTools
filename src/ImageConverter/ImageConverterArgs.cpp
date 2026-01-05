#include "ImageConverterArgs.h"

#include "GitVersion.h"
#include "Utils/Arguments/UsageInformation.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>
#include <type_traits>

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

constexpr auto CATEGORY_GAME = "Game";

const CommandLineOption* const OPTION_GAME_IW3 =
    CommandLineOption::Builder::Create()
    .WithLongName("iw3")
    .WithCategory(CATEGORY_GAME)
    .WithDescription("Converts images for IW3.")
    .Build();

const CommandLineOption* const OPTION_GAME_IW4 =
    CommandLineOption::Builder::Create()
    .WithLongName("iw4")
    .WithCategory(CATEGORY_GAME)
    .WithDescription("Converts images for IW4.")
    .Build();

const CommandLineOption* const OPTION_GAME_IW5 =
    CommandLineOption::Builder::Create()
    .WithLongName("iw5")
    .WithCategory(CATEGORY_GAME)
    .WithDescription("Converts images for IW5.")
    .Build();

const CommandLineOption* const OPTION_GAME_T5 =
    CommandLineOption::Builder::Create()
    .WithLongName("t5")
    .WithCategory(CATEGORY_GAME)
    .WithDescription("Converts images for T5.")
    .Build();

const CommandLineOption* const OPTION_GAME_T6 =
    CommandLineOption::Builder::Create()
    .WithLongName("t6")
    .WithCategory(CATEGORY_GAME)
    .WithDescription("Converts images for T6.")
    .Build();
// clang-format on

const CommandLineOption* const COMMAND_LINE_OPTIONS[]{
    OPTION_HELP,
    OPTION_VERSION,
    OPTION_VERBOSE,
    OPTION_NO_COLOR,
    OPTION_GAME_IW3,
    OPTION_GAME_IW4,
    OPTION_GAME_IW5,
    OPTION_GAME_T5,
    OPTION_GAME_T6,
};

ImageConverterArgs::ImageConverterArgs()
    : m_game_to_convert_to(std::nullopt),
      m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>)
{
}

void ImageConverterArgs::PrintUsage()
{
    UsageInformation usage("ImageConverter.exe");

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.AddArgument("fileToConvert");
    usage.SetVariableArguments(true);

    usage.Print();
}

void ImageConverterArgs::PrintVersion()
{
    con::info("OpenAssetTools ImageConverter {}", GIT_VERSION);
}

bool ImageConverterArgs::ParseArgs(const int argc, const char** argv, bool& shouldContinue)
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

    m_files_to_convert = m_argument_parser.GetArguments();
    if (m_files_to_convert.empty())
    {
        // No files to convert specified...
        PrintUsage();
        return false;
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
