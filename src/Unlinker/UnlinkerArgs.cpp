#include "UnlinkerArgs.h"

#include <regex>
#include <type_traits>

#include "Utils/Arguments/UsageInformation.h"
#include "ObjLoading.h"
#include "ObjWriting.h"

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

const CommandLineOption* const OPTION_LIST =
    CommandLineOption::Builder::Create()
    .WithShortName("l")
    .WithLongName("list")
    .WithDescription("Lists the contents of a zone instead of writing them to the disk.")
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

const CommandLineOption* const OPTION_IMAGE_FORMAT =
    CommandLineOption::Builder::Create()
    .WithLongName("image-format")
    .WithDescription("Specifies the format of dumped image files. Valid values are: DDS, IWI")
    .WithParameter("imageFormatValue")
    .Build();

const CommandLineOption* const OPTION_RAW =
    CommandLineOption::Builder::Create()
    .WithLongName("raw")
    .WithDescription("Prevents generation of a GDT and dumps assets as raw whenever possible.")
    .Build();

const CommandLineOption* const COMMAND_LINE_OPTIONS[]
{
    OPTION_HELP,
    OPTION_VERBOSE,
    OPTION_MINIMAL_ZONE_FILE,
    OPTION_LIST,
    OPTION_OUTPUT_FOLDER,
    OPTION_SEARCH_PATH,
    OPTION_IMAGE_FORMAT,
    OPTION_RAW
};

UnlinkerArgs::UnlinkerArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent<decltype(COMMAND_LINE_OPTIONS)>::value)
{
    m_task = ProcessingTask::DUMP;
    m_output_folder = "./%zoneName%";

    m_verbose = false;
}

bool UnlinkerArgs::ParsePathsString(const std::string& pathsString, std::set<std::string>& output)
{
    std::ostringstream currentPath;
    bool pathStart = true;
    int quotationPos = 0; // 0 = before quotations, 1 = in quotations, 2 = after quotations

    for (auto character : pathsString)
    {
        switch (character)
        {
        case '"':
            if (quotationPos == 0 && pathStart)
            {
                quotationPos = 1;
            }
            else if (quotationPos == 1)
            {
                quotationPos = 2;
                pathStart = false;
            }
            else
            {
                return false;
            }
            break;

        case ';':
            if (quotationPos != 1)
            {
                std::string path = currentPath.str();
                if (!path.empty())
                {
                    output.insert(path);
                    currentPath = std::ostringstream();
                }

                pathStart = true;
                quotationPos = 0;
            }
            else
            {
                currentPath << character;
            }
            break;

        default:
            currentPath << character;
            pathStart = false;
            break;
        }
    }

    if (currentPath.tellp() > 0)
    {
        output.insert(currentPath.str());
    }

    return true;
}

void UnlinkerArgs::PrintUsage()
{
    UsageInformation usage("unlinker.exe");

    for (auto commandLineOption : COMMAND_LINE_OPTIONS)
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
    std::string specifiedValue = m_argument_parser.GetValueForOption(OPTION_IMAGE_FORMAT);
    for (auto& c : specifiedValue)
        c = tolower(c);

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

    m_zones_to_load = m_argument_parser.GetArguments();
    const size_t zoneCount = m_zones_to_load.size();
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

    // -l; --list
    if (m_argument_parser.IsOptionSpecified(OPTION_LIST))
        m_task = ProcessingTask::LIST;

    // -o; --output-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_output_folder = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);

    // --search-path
    if (m_argument_parser.IsOptionSpecified(OPTION_SEARCH_PATH))
    {
        if (!ParsePathsString(m_argument_parser.GetValueForOption(OPTION_SEARCH_PATH), m_user_search_paths))
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

    // --raw
    m_raw = m_argument_parser.IsOptionSpecified(OPTION_RAW);

    return true;
}

std::string UnlinkerArgs::GetOutputFolderPathForZone(Zone* zone) const
{
    return std::regex_replace(m_output_folder, std::regex("%zoneName%"), zone->m_name);
}
