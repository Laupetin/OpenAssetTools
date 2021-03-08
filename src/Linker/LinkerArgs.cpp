#include "LinkerArgs.h"

#include <regex>
#include <type_traits>

#include "Utils/Arguments/UsageInformation.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "Utils/FileUtils.h"

const CommandLineOption* const OPTION_HELP =
    CommandLineOption::Builder::Create()
    .WithShortName("?")
    .WithLongName("help")
    .WithDescription("Displays usage information.")
    .Build();

const CommandLineOption* const OPTION_VERBOSE =
    CommandLineOption::Builder::Create()
    .WithShortName("v")
    .WithLongName("verbose")
    .WithDescription("Outputs a lot more and more detailed messages.")
    .Build();

const CommandLineOption* const OPTION_OUTPUT_FOLDER =
    CommandLineOption::Builder::Create()
    .WithShortName("o")
    .WithLongName("output-folder")
    .WithDescription("Specifies the output folder containing the contents of the unlinked zones. Defaults to ./%zoneName%")
    .WithParameter("outputFolderPath")
    .Build();

const CommandLineOption* const OPTION_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("search-path")
    .WithDescription("Specifies a semi-colon separated list of paths to search for additional game files.")
    .WithParameter("searchPathString")
    .Build();

const CommandLineOption* const OPTION_LOAD =
    CommandLineOption::Builder::Create()
    .WithShortName("l")
    .WithLongName("load")
    .WithDescription("Loads an existing zone to be able to use its assets when building.")
    .WithParameter("zonePath")
    .Reusable()
    .Build();

const CommandLineOption* const COMMAND_LINE_OPTIONS[]
{
    OPTION_HELP,
    OPTION_VERBOSE,
    OPTION_OUTPUT_FOLDER,
    OPTION_SEARCH_PATH,
    OPTION_LOAD
};

LinkerArgs::LinkerArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent<decltype(COMMAND_LINE_OPTIONS)>::value),
      m_output_folder("zone_out/%zoneName%"),
      m_verbose(false)
{
}

void LinkerArgs::PrintUsage()
{
    UsageInformation usage("Linker.exe");

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.AddArgument("zoneName");
    usage.SetVariableArguments(true);

    usage.Print();
}

void LinkerArgs::SetVerbose(const bool isVerbose)
{
    m_verbose = isVerbose;
    ObjLoading::Configuration.Verbose = isVerbose;
    ObjWriting::Configuration.Verbose = isVerbose;
}

bool LinkerArgs::ParseArgs(const int argc, const char** argv)
{
    if (!m_argument_parser.ParseArguments(argc - 1, &argv[1]))
    {
        PrintUsage();
        return false;
    }

    // Check if the user requested help
    if (m_argument_parser.IsOptionSpecified(OPTION_HELP))
    {
        PrintUsage();
        return false;
    }

    m_zones_to_build = m_argument_parser.GetArguments();
    if (m_zones_to_build.empty())
    {
        // No zones to build specified...
        PrintUsage();
        return false;
    }

    // -v; --verbose
    SetVerbose(m_argument_parser.IsOptionSpecified(OPTION_VERBOSE));

    // -o; --output-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_output_folder = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);

    // --search-path
    if (m_argument_parser.IsOptionSpecified(OPTION_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(m_argument_parser.GetValueForOption(OPTION_SEARCH_PATH), m_user_search_paths))
            return false;
    }

    if(m_argument_parser.IsOptionSpecified(OPTION_LOAD))
        m_zones_to_load = m_argument_parser.GetParametersForOption(OPTION_LOAD);

    return true;
}

std::string LinkerArgs::GetOutputFolderPathForZone(const std::string& zoneName) const
{
    return std::regex_replace(m_output_folder, std::regex("%zoneName%"), zoneName);
}
