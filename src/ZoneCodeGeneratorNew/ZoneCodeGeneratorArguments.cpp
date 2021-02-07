#include "ZoneCodeGeneratorArguments.h"

#include <iostream>

#include "Utils/Arguments/CommandLineOption.h"
#include "Utils/Arguments/UsageInformation.h"

const CommandLineOption* const OPTION_HELP = CommandLineOption::Builder::Create()
                                             .WithShortName("?")
                                             .WithLongName("help")
                                             .WithDescription("Displays usage information.")
                                             .Build();

const CommandLineOption* const OPTION_VERBOSE = CommandLineOption::Builder::Create()
                                                .WithShortName("v")
                                                .WithLongName("verbose")
                                                .WithDescription("Outputs a lot more and more detailed messages.")
                                                .Build();

// ------
// INPUT
// ------

constexpr const char* CATEGORY_INPUT = "Input";

const CommandLineOption* const OPTION_HEADER = CommandLineOption::Builder::Create()
                                               .WithShortName("h")
                                               .WithLongName("header")
                                               .WithDescription("Reads from the specified header file.")
                                               .WithCategory(CATEGORY_INPUT)
                                               .WithParameter("headerFile")
                                               .Reusable()
                                               .Build();

const CommandLineOption* const OPTION_COMMANDS_FILE = CommandLineOption::Builder::Create()
                                                      .WithShortName("c")
                                                      .WithLongName("commands-file")
                                                      .WithDescription("Specifies the commands file. Defaults to stdin.")
                                                      .WithCategory(CATEGORY_INPUT)
                                                      .WithParameter("commandFile")
                                                      .Reusable()
                                                      .Build();

// ------
// OUTPUT
// ------
constexpr const char* CATEGORY_OUTPUT = "Output";

const CommandLineOption* const OPTION_OUTPUT_FOLDER = CommandLineOption::Builder::Create()
                                                      .WithShortName("o")
                                                      .WithLongName("output")
                                                      .WithDescription("Specify the folder to save the generate code files to. Defaults to the current directory.")
                                                      .WithCategory(CATEGORY_OUTPUT)
                                                      .WithParameter("outputPath")
                                                      .Build();

const CommandLineOption* const OPTION_PRINT = CommandLineOption::Builder::Create()
                                              .WithShortName("p")
                                              .WithLongName("print")
                                              .WithDescription("Print the loaded data.")
                                              .WithCategory(CATEGORY_OUTPUT)
                                              .Build();

const CommandLineOption* const OPTION_GENERATE = CommandLineOption::Builder::Create()
                                                 .WithShortName("g")
                                                 .WithLongName("generate")
                                                 .WithDescription(
                                                     "Generates a specified asset/preset combination. Can be used multiple times. Available presets: "
                                                     "ZoneLoad, ZoneWrite, AssetStructTests")
                                                 .WithCategory(CATEGORY_OUTPUT)
                                                 .WithParameter("assetName")
                                                 .WithParameter("preset")
                                                 .Reusable()
                                                 .Build();

const CommandLineOption* const COMMAND_LINE_OPTIONS[]
{
    OPTION_HELP,
    OPTION_VERBOSE,
    OPTION_HEADER,
    OPTION_COMMANDS_FILE,
    OPTION_OUTPUT_FOLDER,
    OPTION_PRINT,
    OPTION_GENERATE
};

ZoneCodeGeneratorArguments::GenerationTask::GenerationTask()
= default;

ZoneCodeGeneratorArguments::GenerationTask::GenerationTask(std::string assetName, std::string presetName)
    : m_asset_name(std::move(assetName)),
      m_preset_name(std::move(presetName))
{
}

ZoneCodeGeneratorArguments::ZoneCodeGeneratorArguments()
    : m_argument_parser(COMMAND_LINE_OPTIONS, _countof(COMMAND_LINE_OPTIONS))
{
    m_verbose = false;
    m_task = ProcessingTask::GENERATE_CODE;
}

void ZoneCodeGeneratorArguments::PrintUsage()
{
    UsageInformation usage("ZoneCodeGenerator.exe");

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.Print();
}

bool ZoneCodeGeneratorArguments::Parse(const int argc, const char** argv)
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

    // -v; --verbose
    m_verbose = m_argument_parser.IsOptionSpecified(OPTION_VERBOSE);

    // -p; --print
    if (m_argument_parser.IsOptionSpecified(OPTION_PRINT))
        m_task = ProcessingTask::PRINT_DATA;

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
        std::cout << "At least one header file must be specified via -h / --header." << std::endl;
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
        std::cout << "At least one commands file must be specified via -c / --commands-file." << std::endl;
        return false;
    }

    if (m_task == ProcessingTask::GENERATE_CODE)
    {
        if (!m_argument_parser.IsOptionSpecified(OPTION_GENERATE))
        {
            std::cout << "A generate parameter needs to be specified when generating code" << std::endl;
            PrintUsage();
            return false;
        }

        const auto generateParameterValues = m_argument_parser.GetParametersForOption(OPTION_GENERATE);
        for (auto i = 0u; i < generateParameterValues.size(); i+=2)
            m_generation_tasks.emplace_back(generateParameterValues[i], generateParameterValues[i + 1]);
    }
    else if (m_argument_parser.IsOptionSpecified(OPTION_GENERATE))
    {
        std::cout << "Cannot specify generate parameter when not generating code" << std::endl;
        PrintUsage();
        return false;
    }

    return true;
}
