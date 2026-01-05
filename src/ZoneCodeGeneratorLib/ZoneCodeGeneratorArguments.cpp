#include "ZoneCodeGeneratorArguments.h"

#include "GitVersion.h"
#include "Utils/Arguments/CommandLineOption.h"
#include "Utils/Arguments/UsageInformation.h"
#include "Utils/Logging/Log.h"

#include <format>
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

constexpr auto CATEGORY_INPUT = "Input";

const CommandLineOption* const OPTION_HEADER =
    CommandLineOption::Builder::Create()
    .WithShortName("h")
    .WithLongName("header")
    .WithDescription("Reads from the specified header file.")
    .WithCategory(CATEGORY_INPUT)
    .WithParameter("headerFile")
    .Reusable()
    .Build();

const CommandLineOption* const OPTION_COMMANDS_FILE =
    CommandLineOption::Builder::Create()
    .WithShortName("c")
    .WithLongName("commands-file")
    .WithDescription("Specifies the commands file. Defaults to stdin.")
    .WithCategory(CATEGORY_INPUT)
    .WithParameter("commandFile")
    .Reusable()
    .Build();

constexpr auto CATEGORY_OUTPUT = "Output";

const CommandLineOption* const OPTION_OUTPUT_FOLDER =
    CommandLineOption::Builder::Create()
    .WithShortName("o")
    .WithLongName("output")
    .WithDescription("Specify the folder to save the generate code files to. Defaults to the current directory.")
    .WithCategory(CATEGORY_OUTPUT)
    .WithParameter("outputPath")
    .Build();

const CommandLineOption* const OPTION_PRINT =
    CommandLineOption::Builder::Create()
    .WithShortName("p")
    .WithLongName("print")
    .WithDescription("Print the loaded data.")
    .WithCategory(CATEGORY_OUTPUT)
    .Build();

const CommandLineOption* const OPTION_GENERATE =
    CommandLineOption::Builder::Create()
    .WithShortName("g")
    .WithLongName("generate")
    .WithDescription("Generates a specified preset. Can be used multiple times. Available presets: ZoneLoad, ZoneWrite, AssetStructTests")
    .WithCategory(CATEGORY_OUTPUT)
    .WithParameter("preset")
    .Reusable()
    .Build();
// clang-format on

const CommandLineOption* const COMMAND_LINE_OPTIONS[]{
    OPTION_HELP,
    OPTION_VERSION,
    OPTION_VERBOSE,
    OPTION_NO_COLOR,
    OPTION_HEADER,
    OPTION_COMMANDS_FILE,
    OPTION_OUTPUT_FOLDER,
    OPTION_PRINT,
    OPTION_GENERATE,
};

namespace
{
    constexpr unsigned FLAG_TASK_GENERATE = 1 << 0;
    constexpr unsigned FLAG_TASK_PRINT = 1 << 1;
} // namespace

ZoneCodeGeneratorArguments::ZoneCodeGeneratorArguments()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>),
      m_task_flags(0u)
{
}

void ZoneCodeGeneratorArguments::PrintUsage() const
{
    UsageInformation usage(m_argument_parser.GetExecutableName());

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
        usage.AddCommandLineOption(commandLineOption);

    usage.Print();
}

void ZoneCodeGeneratorArguments::PrintVersion()
{
    con::info("OpenAssetTools ZoneCodeGenerator {}", GIT_VERSION);
}

bool ZoneCodeGeneratorArguments::ParseArgs(const int argc, const char** argv, bool& shouldContinue)
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

    // -p; --print
    if (m_argument_parser.IsOptionSpecified(OPTION_PRINT))
        m_task_flags |= FLAG_TASK_PRINT;

    // -o; --output
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_output_directory = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);
    else
        m_output_directory = ".";

    // -h; --header
    if (m_argument_parser.IsOptionSpecified(OPTION_HEADER))
    {
        for (const auto& arg : m_argument_parser.GetParametersForOption(OPTION_HEADER))
            m_header_paths.push_back(arg);
    }
    else
    {
        con::error("At least one header file must be specified via -h / --header.");
        return false;
    }

    // -c; --commands-file
    if (m_argument_parser.IsOptionSpecified(OPTION_COMMANDS_FILE))
    {
        for (const auto& arg : m_argument_parser.GetParametersForOption(OPTION_COMMANDS_FILE))
            m_command_paths.push_back(arg);
    }
    else
    {
        con::error("At least one commands file must be specified via -c / --commands-file.");
        return false;
    }

    if (m_argument_parser.IsOptionSpecified(OPTION_GENERATE))
    {
        m_task_flags |= FLAG_TASK_GENERATE;
        m_template_names = m_argument_parser.GetParametersForOption(OPTION_GENERATE);
    }

    if (m_task_flags == 0)
    {
        con::warn("There was no output task specified.");
        PrintUsage();
        return false;
    }

    return true;
}

bool ZoneCodeGeneratorArguments::ShouldGenerate() const
{
    return m_task_flags & FLAG_TASK_GENERATE;
}

bool ZoneCodeGeneratorArguments::ShouldPrint() const
{
    return m_task_flags & FLAG_TASK_PRINT;
}
