#include "ZoneCodeGeneratorArguments.h"

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

const CommandLineOption* const OPTION_CREATE = CommandLineOption::Builder::Create()
                                               .WithShortName("h")
                                               .WithLongName("header")
                                               .WithDescription("Create a new database from the specified header file.")
                                               .WithCategory(CATEGORY_INPUT)
                                               .WithParameter("headerFile")
                                               .Build();

// ------
// EDITING
// ------
constexpr const char* CATEGORY_EDITING = "Editing";

const CommandLineOption* const OPTION_EDITING_COMMANDS = CommandLineOption::Builder::Create()
                                                         .WithShortName("e")
                                                         .WithLongName("editing-commands")
                                                         .WithDescription("Specifies the editing command file. Defaults to stdin.")
                                                         .WithCategory(CATEGORY_EDITING)
                                                         .WithParameter("commandFile")
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
                                                 .WithDescription("Generates a specified asset/preset combination. Can be used multiple times. Available presets: "
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
    OPTION_CREATE,
    OPTION_EDITING_COMMANDS,
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

    // -h; --header
    if (m_argument_parser.IsOptionSpecified(OPTION_CREATE))
        m_header_path = m_argument_parser.GetValueForOption(OPTION_CREATE);

    // -e; --editing-commands
    if (m_argument_parser.IsOptionSpecified(OPTION_EDITING_COMMANDS))
        m_commands_path = m_argument_parser.GetValueForOption(OPTION_EDITING_COMMANDS);

    if (m_task == ProcessingTask::GENERATE_CODE)
    {
        if (!m_argument_parser.IsOptionSpecified(OPTION_GENERATE))
        {
            printf("A generate parameter needs to be specified when generating code\n");
            PrintUsage();
            return false;
        }

        const auto generateParameterValues = m_argument_parser.GetParametersForOption(OPTION_GENERATE);
        const auto generateCount = generateParameterValues.size() / 2;
        for(auto i = 0u; i < generateCount; i++)
            m_generation_tasks.emplace_back(generateParameterValues[i], generateParameterValues[i + 1]);
    }
    else if (m_argument_parser.IsOptionSpecified(OPTION_GENERATE))
    {
        printf("Cannot specify generate parameter when not generating code\n");
        PrintUsage();
        return false;
    }

    return true;
}
