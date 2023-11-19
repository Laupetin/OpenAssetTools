#include "RawTemplaterArguments.h"

#include "Utils/Arguments/CommandLineOption.h"
#include "Utils/Arguments/UsageInformation.h"

const CommandLineOption* const OPTION_HELP =
    CommandLineOption::Builder::Create().WithShortName("?").WithLongName("help").WithDescription("Displays usage information.").Build();

const CommandLineOption* const OPTION_VERBOSE =
    CommandLineOption::Builder::Create().WithShortName("v").WithLongName("verbose").WithDescription("Outputs a lot more and more detailed messages.").Build();

const CommandLineOption* const OPTION_OUTPUT_FOLDER = CommandLineOption::Builder::Create()
                                                          .WithShortName("o")
                                                          .WithLongName("output")
                                                          .WithDescription("Specify the folder to save the generated files. Defaults to the current directory.")
                                                          .WithParameter("outputPath")
                                                          .Build();

const CommandLineOption* const OPTION_BUILD_LOG = CommandLineOption::Builder::Create()
                                                      .WithLongName("build-log")
                                                      .WithDescription("Specify a file to write a build log to.")
                                                      .WithParameter("logFilePath")
                                                      .Build();

const CommandLineOption* const OPTION_DEFINE = CommandLineOption::Builder::Create()
                                                   .WithShortName("d")
                                                   .WithLongName("define")
                                                   .WithDescription("Adds a define for the templating process. Can be of format define or define=value.")
                                                   .WithParameter("defineValue")
                                                   .Reusable()
                                                   .Build();

const CommandLineOption* const COMMAND_LINE_OPTIONS[]{OPTION_HELP, OPTION_VERBOSE, OPTION_OUTPUT_FOLDER, OPTION_BUILD_LOG, OPTION_DEFINE};

RawTemplaterArguments::RawTemplaterArguments()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>),
      m_verbose(false)
{
}

void RawTemplaterArguments::PrintUsage()
{
    UsageInformation usage("RawTemplater.exe");

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.Print();
}

bool RawTemplaterArguments::Parse(const int argc, const char** argv)
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

    m_input_files = m_argument_parser.GetArguments();
    if (m_input_files.empty())
    {
        PrintUsage();
        return false;
    }

    // -v; --verbose
    m_verbose = m_argument_parser.IsOptionSpecified(OPTION_VERBOSE);

    // -o; --output
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_output_directory = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);
    else
        m_output_directory = ".";

    // --build-log
    if (m_argument_parser.IsOptionSpecified(OPTION_BUILD_LOG))
        m_build_log_file = m_argument_parser.GetValueForOption(OPTION_BUILD_LOG);

    // -d; --define
    if (m_argument_parser.IsOptionSpecified(OPTION_DEFINE))
    {
        for (const auto& arg : m_argument_parser.GetParametersForOption(OPTION_DEFINE))
        {
            const auto separator = arg.find('=');

            if (separator != std::string::npos)
                m_defines.emplace_back(std::make_pair(arg.substr(0, separator), arg.substr(separator + 1)));
            else
                m_defines.emplace_back(std::make_pair(arg, std::string()));
        }
    }

    return true;
}
