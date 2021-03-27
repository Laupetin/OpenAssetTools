#include "LinkerArgs.h"

#include <filesystem>
#include <regex>
#include <type_traits>

#include "Utils/Arguments/UsageInformation.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "Utils/FileUtils.h"

namespace fs = std::filesystem;

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

const CommandLineOption* const OPTION_BASE_FOLDER =
    CommandLineOption::Builder::Create()
    .WithShortName("b")
    .WithLongName("base-folder")
    .WithDescription("Specifies the base folder that can be used to specify other folders. Defaults to the current directory.")
    .WithParameter("baseFolderPath")
    .Build();

const CommandLineOption* const OPTION_OUTPUT_FOLDER =
    CommandLineOption::Builder::Create()
    .WithLongName("output-folder")
    .WithDescription("Specifies the output folder containing the contents of the unlinked zones. Defaults to \"" + std::string(LinkerArgs::DEFAULT_OUTPUT_FOLDER) + "\".")
    .WithParameter("outputFolderPath")
    .Build();

const CommandLineOption* const OPTION_ASSET_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("asset-search-path")
    .WithDescription("Specifies the search paths used for assets. Defaults to \"" + std::string(LinkerArgs::DEFAULT_ASSET_SEARCH_PATH) + "\".")
    .WithParameter("assetSearchPathString")
    .Build();

const CommandLineOption* const OPTION_GDT_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("gdt-search-path")
    .WithDescription("Specifies the search paths used for assets. Defaults to \"" + std::string(LinkerArgs::DEFAULT_GDT_SEARCH_PATH) + "\".")
    .WithParameter("gdtSearchPathString")
    .Build();

const CommandLineOption* const OPTION_SOURCE_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("source-search-path")
    .WithDescription("Specifies the search paths used for assets. Defaults to \"" + std::string(LinkerArgs::DEFAULT_SOURCE_SEARCH_PATH) + "\".")
    .WithParameter("sourceSearchPathString")
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
    OPTION_BASE_FOLDER,
    OPTION_OUTPUT_FOLDER,
    OPTION_ASSET_SEARCH_PATH,
    OPTION_GDT_SEARCH_PATH,
    OPTION_SOURCE_SEARCH_PATH,
    OPTION_LOAD
};

LinkerArgs::LinkerArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent<decltype(COMMAND_LINE_OPTIONS)>::value),
      m_base_pattern(R"(\?base\?)"),
      m_zone_pattern(R"(\?zone\?)"),
      m_base_folder_depends_on_zone(false),
      m_out_folder_depends_on_zone(false),
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

std::string LinkerArgs::GetBasePathForZone(const std::string& zoneName) const
{
    return std::regex_replace(m_base_folder, m_zone_pattern, zoneName);
}

void LinkerArgs::SetDefaultBasePath()
{
    const auto currentDir = fs::absolute(fs::current_path());

    if (currentDir.filename() == "bin")
    {
        m_base_folder = currentDir.parent_path().string();
    }
    else
    {
        m_base_folder = currentDir.string();
    }
}

std::set<std::string> LinkerArgs::GetZoneIndependentSearchPaths(const std::set<std::string>& set) const
{
    std::set<std::string> out;

    for(const auto& path : set)
    {
        if(path.find(PATTERN_ZONE) != std::string::npos)
            continue;

        if(path.find(PATTERN_BASE) != std::string::npos)
        {
            if(m_base_folder_depends_on_zone)
                continue;

            out.emplace(std::regex_replace(path, m_base_pattern, m_base_folder));
        }
        else
        {
            out.emplace(path);
        }
    }

    return out;
}

std::set<std::string> LinkerArgs::GetSearchPathsForZone(const std::set<std::string>& set, const std::string& zoneName) const
{
    std::set<std::string> out;
    const auto basePath = GetBasePathForZone(zoneName);

    for (const auto& path : set)
    {
        if (path.find(PATTERN_ZONE) == std::string::npos
            && (!m_base_folder_depends_on_zone || path.find(PATTERN_BASE) == std::string::npos))
        {
            continue;
        }

        out.emplace(std::regex_replace(std::regex_replace(path, m_zone_pattern, zoneName), m_base_pattern, basePath));
    }

    return out;
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

    // b; --base-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_BASE_FOLDER))
        m_base_folder = m_argument_parser.GetValueForOption(OPTION_BASE_FOLDER);
    else
        SetDefaultBasePath();
    m_base_folder_depends_on_zone = m_base_folder.find(PATTERN_ZONE) != std::string::npos;

    // --output-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_out_folder = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);
    else
        m_out_folder = DEFAULT_OUTPUT_FOLDER;
    m_out_folder_depends_on_zone = m_out_folder.find(PATTERN_ZONE) != std::string::npos;

    // --asset-search-path
    if (m_argument_parser.IsOptionSpecified(OPTION_ASSET_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(m_argument_parser.GetValueForOption(OPTION_ASSET_SEARCH_PATH), m_asset_search_paths))
            return false;
    }
    else
    {
        if (!FileUtils::ParsePathsString(DEFAULT_ASSET_SEARCH_PATH, m_asset_search_paths))
            return false;
    }

    // --gdt-search-path
    if (m_argument_parser.IsOptionSpecified(OPTION_GDT_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(m_argument_parser.GetValueForOption(OPTION_GDT_SEARCH_PATH), m_gdt_search_paths))
            return false;
    }
    else
    {
        if (!FileUtils::ParsePathsString(DEFAULT_GDT_SEARCH_PATH, m_gdt_search_paths))
            return false;
    }

    // --source-search-path
    if (m_argument_parser.IsOptionSpecified(OPTION_SOURCE_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(m_argument_parser.GetValueForOption(OPTION_SOURCE_SEARCH_PATH), m_source_search_paths))
            return false;
    }
    else
    {
        if (!FileUtils::ParsePathsString(DEFAULT_SOURCE_SEARCH_PATH, m_source_search_paths))
            return false;
    }

    // -l; --load
    if (m_argument_parser.IsOptionSpecified(OPTION_LOAD))
        m_zones_to_load = m_argument_parser.GetParametersForOption(OPTION_LOAD);

    return true;
}

std::string LinkerArgs::GetOutputFolderPathForZone(const std::string& zoneName) const
{
    return std::regex_replace(std::regex_replace(m_out_folder, m_zone_pattern, zoneName), m_base_pattern, GetBasePathForZone(zoneName));
}

std::set<std::string> LinkerArgs::GetZoneIndependentAssetSearchPaths() const
{
    return GetZoneIndependentSearchPaths(m_asset_search_paths);
}

std::set<std::string> LinkerArgs::GetZoneIndependentGdtSearchPaths() const
{
    return GetZoneIndependentSearchPaths(m_gdt_search_paths);
}

std::set<std::string> LinkerArgs::GetZoneIndependentSourceSearchPaths() const
{
    return GetZoneIndependentSearchPaths(m_source_search_paths);
}

std::set<std::string> LinkerArgs::GetAssetSearchPathsForZone(const std::string& zoneName) const
{
    return GetSearchPathsForZone(m_asset_search_paths, zoneName);
}

std::set<std::string> LinkerArgs::GetGdtSearchPathsForZone(const std::string& zoneName) const
{
    return GetSearchPathsForZone(m_gdt_search_paths, zoneName);
}

std::set<std::string> LinkerArgs::GetSourceSearchPathsForZone(const std::string& zoneName) const
{
    return GetSearchPathsForZone(m_source_search_paths, zoneName);
}
