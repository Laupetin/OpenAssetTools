#include "LinkerArgs.h"

#include "GitVersion.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "Utils/Arguments/UsageInformation.h"
#include "Utils/FileUtils.h"

#include <filesystem>
#include <iostream>
#include <regex>
#include <type_traits>

namespace fs = std::filesystem;

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
    .WithDescription("Specifies the output folder containing the build artifacts. Defaults to \"" + std::string(LinkerArgs::DEFAULT_OUTPUT_FOLDER) + "\".")
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

const CommandLineOption* const OPTION_MENU_PERMISSIVE =
    CommandLineOption::Builder::Create()
    .WithLongName("menu-permissive")
    .WithDescription("Allows the usage of unknown script commands that can be compiled.")
    .Build();

const CommandLineOption* const OPTION_MENU_NO_OPTIMIZATION =
    CommandLineOption::Builder::Create()
    .WithLongName("menu-no-optimization")
    .WithDescription("Refrain from applying optimizations to parsed menus. (Optimizations increase menu performance and size. May result in less source "
                        "information when dumped though.)")
    .Build();

// clang-format on

const CommandLineOption* const COMMAND_LINE_OPTIONS[]{
    OPTION_HELP,
    OPTION_VERSION,
    OPTION_VERBOSE,
    OPTION_BASE_FOLDER,
    OPTION_OUTPUT_FOLDER,
    OPTION_ASSET_SEARCH_PATH,
    OPTION_GDT_SEARCH_PATH,
    OPTION_SOURCE_SEARCH_PATH,
    OPTION_LOAD,
    OPTION_MENU_PERMISSIVE,
    OPTION_MENU_NO_OPTIMIZATION,
};

LinkerArgs::LinkerArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>),
      m_base_pattern(R"(\?base\?)"),
      m_game_pattern(R"(\?game\?)"),
      m_project_pattern(R"(\?project\?)"),
      m_base_folder_depends_on_project(false),
      m_out_folder_depends_on_project(false),
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

    usage.AddArgument("projectName");
    usage.SetVariableArguments(true);

    usage.Print();
}

void LinkerArgs::PrintVersion()
{
    std::cout << "OpenAssetTools Linker " << std::string(GIT_VERSION) << "\n";
}

void LinkerArgs::SetVerbose(const bool isVerbose)
{
    m_verbose = isVerbose;
    ObjLoading::Configuration.Verbose = isVerbose;
    ObjWriting::Configuration.Verbose = isVerbose;
}

std::string LinkerArgs::GetBasePathForProject(const std::string& projectName) const
{
    return std::regex_replace(m_base_folder, m_project_pattern, projectName);
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

std::set<std::string> LinkerArgs::GetProjectIndependentSearchPaths(const std::set<std::string>& set) const
{
    std::set<std::string> out;

    for (const auto& path : set)
    {
        if (path.find(PATTERN_GAME) != std::string::npos)
            continue;
        if (path.find(PATTERN_PROJECT) != std::string::npos)
            continue;

        if (path.find(PATTERN_BASE) != std::string::npos)
        {
            if (m_base_folder_depends_on_project)
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

std::set<std::string> LinkerArgs::GetSearchPathsForProject(const std::set<std::string>& set, const std::string& gameName, const std::string& projectName) const
{
    std::set<std::string> out;
    const auto basePath = GetBasePathForProject(projectName);

    for (const auto& path : set)
    {
        if (path.find(PATTERN_GAME) == std::string::npos && path.find(PATTERN_PROJECT) == std::string::npos
            && (!m_base_folder_depends_on_project || path.find(PATTERN_BASE) == std::string::npos))
        {
            continue;
        }

        out.emplace(std::regex_replace(
            std::regex_replace(std::regex_replace(path, m_project_pattern, projectName), m_game_pattern, gameName), m_base_pattern, basePath));
    }

    return out;
}

bool LinkerArgs::ParseArgs(const int argc, const char** argv, bool& shouldContinue)
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

    m_project_specifiers_to_build = m_argument_parser.GetArguments();
    if (m_project_specifiers_to_build.empty())
    {
        // No projects to build specified...
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
    m_base_folder_depends_on_project = m_base_folder.find(PATTERN_GAME) != std::string::npos || m_base_folder.find(PATTERN_PROJECT) != std::string::npos;

    // --output-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_out_folder = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);
    else
        m_out_folder = DEFAULT_OUTPUT_FOLDER;
    m_out_folder_depends_on_project = m_out_folder.find(PATTERN_PROJECT) != std::string::npos;

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

    // --menu-permissive
    if (m_argument_parser.IsOptionSpecified(OPTION_MENU_PERMISSIVE))
        ObjLoading::Configuration.MenuPermissiveParsing = true;

    // --menu-no-optimization
    if (m_argument_parser.IsOptionSpecified(OPTION_MENU_NO_OPTIMIZATION))
        ObjLoading::Configuration.MenuNoOptimization = true;

    return true;
}

std::string LinkerArgs::GetOutputFolderPathForProject(const std::string& projectName) const
{
    return std::regex_replace(std::regex_replace(m_out_folder, m_project_pattern, projectName), m_base_pattern, GetBasePathForProject(projectName));
}

std::set<std::string> LinkerArgs::GetProjectIndependentAssetSearchPaths() const
{
    return GetProjectIndependentSearchPaths(m_asset_search_paths);
}

std::set<std::string> LinkerArgs::GetProjectIndependentGdtSearchPaths() const
{
    return GetProjectIndependentSearchPaths(m_gdt_search_paths);
}

std::set<std::string> LinkerArgs::GetProjectIndependentSourceSearchPaths() const
{
    return GetProjectIndependentSearchPaths(m_source_search_paths);
}

std::set<std::string> LinkerArgs::GetAssetSearchPathsForProject(const std::string& gameName, const std::string& projectName) const
{
    return GetSearchPathsForProject(m_asset_search_paths, gameName, projectName);
}

std::set<std::string> LinkerArgs::GetGdtSearchPathsForProject(const std::string& gameName, const std::string& projectName) const
{
    return GetSearchPathsForProject(m_gdt_search_paths, gameName, projectName);
}

std::set<std::string> LinkerArgs::GetSourceSearchPathsForProject(const std::string& projectName) const
{
    return GetSearchPathsForProject(m_source_search_paths, "", projectName);
}
