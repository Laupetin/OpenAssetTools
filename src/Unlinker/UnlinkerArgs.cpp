#include "UnlinkerArgs.h"

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

const CommandLineOption* const OPTION_MINIMAL_ZONE_FILE =
    CommandLineOption::Builder::Create()
    .WithShortName("min")
    .WithLongName("minimal-zone")
    .WithDescription("Minimizes the size of the zone file output by only including assets that are not a dependency of another asset.")
    .Build();

const CommandLineOption* const OPTION_LOAD =
    CommandLineOption::Builder::Create()
    .WithShortName("l")
    .WithLongName("load")
    .WithDescription("Loads an existing zone before trying to unlink any zone.")
    .WithParameter("zonePath")
    .Reusable()
    .Build();

const CommandLineOption* const OPTION_LIST =
    CommandLineOption::Builder::Create()
    .WithLongName("list")
    .WithDescription("Lists the contents of a zone instead of writing them to the disk.")
    .Build();

const CommandLineOption* const OPTION_OUTPUT_FOLDER =
    CommandLineOption::Builder::Create()
    .WithShortName("o")
    .WithLongName("output-folder")
    .WithDescription("Specifies the output folder containing the contents of the unlinked zones. Defaults to \"" + std::string(UnlinkerArgs::DEFAULT_OUTPUT_FOLDER) + "\"")
    .WithParameter("outputFolderPath")
    .Build();

const CommandLineOption* const OPTION_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("search-path")
    .WithDescription("Specifies a semi-colon separated list of paths to search for additional game files.")
    .WithParameter("searchPathString")
    .Build();

const CommandLineOption* const OPTION_IMAGE_FORMAT =
    CommandLineOption::Builder::Create()
    .WithLongName("image-format")
    .WithDescription("Specifies the format of dumped image files. Valid values are: DDS, IWI")
    .WithParameter("imageFormatValue")
    .Build();

const CommandLineOption* const OPTION_MODEL_FORMAT =
    CommandLineOption::Builder::Create()
    .WithLongName("model-format")
    .WithDescription("Specifies the format of dumped model files. Valid values are: XMODEL_EXPORT, OBJ")
    .WithParameter("modelFormatValue")
    .Build();

const CommandLineOption* const OPTION_GDT =
    CommandLineOption::Builder::Create()
    .WithLongName("gdt")
    .WithDescription("Dumps assets in a GDT whenever possible.")
    .Build();

const CommandLineOption* const COMMAND_LINE_OPTIONS[]
{
    OPTION_HELP,
    OPTION_VERBOSE,
    OPTION_MINIMAL_ZONE_FILE,
    OPTION_LOAD,
    OPTION_LIST,
    OPTION_OUTPUT_FOLDER,
    OPTION_SEARCH_PATH,
    OPTION_IMAGE_FORMAT,
    OPTION_MODEL_FORMAT,
    OPTION_GDT
};

UnlinkerArgs::UnlinkerArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent<decltype(COMMAND_LINE_OPTIONS)>::value),
      m_zone_pattern(R"(\?zone\?)"),
      m_task(ProcessingTask::DUMP),
      m_minimal_zone_def(false),
      m_use_gdt(false),
      m_verbose(false)
{
}

void UnlinkerArgs::PrintUsage()
{
    UsageInformation usage("Unlinker.exe");

    for (const auto* commandLineOption : COMMAND_LINE_OPTIONS)
    {
        usage.AddCommandLineOption(commandLineOption);
    }

    usage.AddArgument("pathToZone");
    usage.SetVariableArguments(true);

    usage.Print();
}

void UnlinkerArgs::SetVerbose(const bool isVerbose)
{
    m_verbose = isVerbose;
    ObjLoading::Configuration.Verbose = isVerbose;
    ObjWriting::Configuration.Verbose = isVerbose;
}

bool UnlinkerArgs::SetImageDumpingMode()
{
    auto specifiedValue = m_argument_parser.GetValueForOption(OPTION_IMAGE_FORMAT);
    for (auto& c : specifiedValue)
        c = static_cast<char>(tolower(c));

    if (specifiedValue == "dds")
    {
        ObjWriting::Configuration.ImageOutputFormat = ObjWriting::Configuration_t::ImageOutputFormat_e::DDS;
        return true;
    }

    if (specifiedValue == "iwi")
    {
        ObjWriting::Configuration.ImageOutputFormat = ObjWriting::Configuration_t::ImageOutputFormat_e::IWI;
        return true;
    }

    const std::string originalValue = m_argument_parser.GetValueForOption(OPTION_IMAGE_FORMAT);
    printf("Illegal value: \"%s\" is not a valid image output format. Use -? to see usage information.\n", originalValue.c_str());
    return false;
}

bool UnlinkerArgs::SetModelDumpingMode()
{
    auto specifiedValue = m_argument_parser.GetValueForOption(OPTION_MODEL_FORMAT);
    for (auto& c : specifiedValue)
        c = static_cast<char>(tolower(c));

    if (specifiedValue == "xmodel_export")
    {
        ObjWriting::Configuration.ModelOutputFormat = ObjWriting::Configuration_t::ModelOutputFormat_e::XMODEL_EXPORT;
        return true;
    }

    if (specifiedValue == "obj")
    {
        ObjWriting::Configuration.ModelOutputFormat = ObjWriting::Configuration_t::ModelOutputFormat_e::XMODEL_EXPORT;
        return true;
    }

    const std::string originalValue = m_argument_parser.GetValueForOption(OPTION_MODEL_FORMAT);
    printf("Illegal value: \"%s\" is not a valid model output format. Use -? to see usage information.\n", originalValue.c_str());
    return false;
}

bool UnlinkerArgs::ParseArgs(const int argc, const char** argv)
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

    m_zones_to_unlink = m_argument_parser.GetArguments();
    const size_t zoneCount = m_zones_to_unlink.size();
    if (zoneCount < 1)
    {
        // No zones to load specified...
        PrintUsage();
        return false;
    }


    // -v; --verbose
    SetVerbose(m_argument_parser.IsOptionSpecified(OPTION_VERBOSE));

    // -min; --minimal-zone
    m_minimal_zone_def = m_argument_parser.IsOptionSpecified(OPTION_MINIMAL_ZONE_FILE);

    // -l; --load
    if (m_argument_parser.IsOptionSpecified(OPTION_LOAD))
        m_zones_to_load = m_argument_parser.GetParametersForOption(OPTION_LOAD);

    // --list
    if (m_argument_parser.IsOptionSpecified(OPTION_LIST))
        m_task = ProcessingTask::LIST;

    // -o; --output-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_output_folder = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);
    else
        m_output_folder = DEFAULT_OUTPUT_FOLDER;

    // --search-path
    if (m_argument_parser.IsOptionSpecified(OPTION_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(m_argument_parser.GetValueForOption(OPTION_SEARCH_PATH), m_user_search_paths))
        {
            return false;
        }
    }

    // --image-format
    if (m_argument_parser.IsOptionSpecified(OPTION_IMAGE_FORMAT))
    {
        if (!SetImageDumpingMode())
        {
            return false;
        }
    }

    // --model-format
    if (m_argument_parser.IsOptionSpecified(OPTION_MODEL_FORMAT))
    {
        if (!SetModelDumpingMode())
        {
            return false;
        }
    }

    // --gdt
    m_use_gdt = m_argument_parser.IsOptionSpecified(OPTION_GDT);

    return true;
}

std::string UnlinkerArgs::GetOutputFolderPathForZone(Zone* zone) const
{
    return std::regex_replace(m_output_folder, m_zone_pattern, zone->m_name);
}
