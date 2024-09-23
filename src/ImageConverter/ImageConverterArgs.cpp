#include "ImageConverterArgs.h"

#include "GitVersion.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "Utils/Arguments/UsageInformation.h"

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
// clang-format on

const CommandLineOption* const COMMAND_LINE_OPTIONS[]{
    OPTION_HELP,
    OPTION_VERSION,
    OPTION_VERBOSE,
};

ImageConverterArgs::ImageConverterArgs()
    : m_verbose(false),
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
    std::cout << std::format("OpenAssetTools ImageConverter {}\n", GIT_VERSION);
}

void ImageConverterArgs::SetVerbose(const bool isVerbose)
{
    m_verbose = isVerbose;
    ObjLoading::Configuration.Verbose = isVerbose;
    ObjWriting::Configuration.Verbose = isVerbose;
}

bool ImageConverterArgs::ParseArgs(const int argc, const char** argv, bool& shouldContinue)
{
    shouldContinue = true;
    if (!m_argument_parser.ParseArguments(argc - 1, &argv[1]))
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
    SetVerbose(m_argument_parser.IsOptionSpecified(OPTION_VERBOSE));

    return true;
}
