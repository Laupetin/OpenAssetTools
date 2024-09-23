#include "ZoneCodeGeneratorArguments.h"

#include "GitVersion.h"
#include "Utils/Arguments/CommandLineOption.h"
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
    .WithDescription("Generates a specified asset/preset combination. Can be used multiple times. Available presets: ZoneLoad, ZoneWrite, AssetStructTests")
    .WithCategory(CATEGORY_OUTPUT)
    .WithParameter("assetName")
    .WithParameter("preset")
    .Reusable()
    .Build();
// clang-format on

const CommandLineOption* const COMMAND_LINE_OPTIONS[]{
    OPTION_HELP,
    OPTION_VERSION,
    OPTION_VERBOSE,
    OPTION_HEADER,
    OPTION_COMMANDS_FILE,
    OPTION_OUTPUT_FOLDER,
    OPTION_PRINT,
    OPTION_GENERATE,
};

ZoneCodeGeneratorArguments::GenerationTask::GenerationTask()
    : m_all_assets(false)
{
}

ZoneCodeGeneratorArguments::GenerationTask::GenerationTask(std::string templateName)
    : m_all_assets(true),
      m_template_name(std::move(templateName))
{
}

ZoneCodeGeneratorArguments::GenerationTask::GenerationTask(std::string assetName, std::string templateName)
    : m_all_assets(false),
      m_asset_name(std::move(assetName)),
      m_template_name(std::move(templateName))
{
}

ZoneCodeGeneratorArguments::ZoneCodeGeneratorArguments()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>),
      m_task_flags(0)
{
    m_verbose = false;
}

void ZoneCodeGeneratorArguments::PrintUsage() const
{
    UsageInformation usage(m_argument_parser.GetExecutableName());

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.Print();
}

void ZoneCodeGeneratorArguments::PrintVersion()
{
    std::cout << std::format("OpenAssetTools ZoneCodeGenerator {}\n", GIT_VERSION);
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
    m_verbose = m_argument_parser.IsOptionSpecified(OPTION_VERBOSE);

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
        std::cout << "At least one header file must be specified via -h / --header.\n";
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
        std::cout << "At least one commands file must be specified via -c / --commands-file.\n";
        return false;
    }

    if (m_argument_parser.IsOptionSpecified(OPTION_GENERATE))
    {
        m_task_flags |= FLAG_TASK_GENERATE;
        const auto generateParameterValues = m_argument_parser.GetParametersForOption(OPTION_GENERATE);
        for (auto i = 0u; i < generateParameterValues.size(); i += 2)
        {
            const auto& assetName = generateParameterValues[i];
            const auto& templateName = generateParameterValues[i + 1];

            if (assetName == "*")
                m_generation_tasks.emplace_back(templateName);
            else
                m_generation_tasks.emplace_back(assetName, templateName);
        }
    }

    if (m_task_flags == 0)
    {
        std::cout << "There was no output task specified.\n";
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
