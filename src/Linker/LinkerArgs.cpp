#include "LinkerArgs.h"

#include "GitVersion.h"
#include "ObjLoading.h"
#include "ObjWriting.h"
#include "Utils/Arguments/UsageInformation.h"
#include "Utils/FileUtils.h"
#include "Utils/Logging/Log.h"
#include "Utils/PathUtils.h"

#include <filesystem>
#include <format>
#include <iostream>
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
    .WithDescription(std::format("Specifies the output folder containing the build artifacts. Defaults to \"{}\".", LinkerArgs::DEFAULT_OUTPUT_FOLDER))
    .WithParameter("outputFolderPath")
    .Build();

const CommandLineOption* const OPTION_ADD_ASSET_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("add-asset-search-path")
    .WithDescription("Adds a search paths used for assets. This does not override the default search paths.")
    .WithParameter("assetSearchPathString")
    .Reusable()
    .Build();

const CommandLineOption* const OPTION_ASSET_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("asset-search-path")
    .WithDescription(std::format("Specifies the search paths used for assets. Defaults to \"{}\".", LinkerArgs::DEFAULT_ASSET_SEARCH_PATH))
    .WithParameter("assetSearchPathString")
    .Build();

const CommandLineOption* const OPTION_GDT_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("gdt-search-path")
    .WithDescription(std::format("Specifies the search paths used for gdt files. Defaults to \"{}\".", LinkerArgs::DEFAULT_GDT_SEARCH_PATH))
    .WithParameter("gdtSearchPathString")
    .Build();

const CommandLineOption* const OPTION_ADD_SOURCE_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("add-source-search-path")
    .WithDescription("Adds a search paths used for source files. This does not override the default search paths.")
    .WithParameter("sourceSearchPathString")
    .Reusable()
    .Build();

const CommandLineOption* const OPTION_SOURCE_SEARCH_PATH =
    CommandLineOption::Builder::Create()
    .WithLongName("source-search-path")
    .WithDescription(std::format("Specifies the search paths used for source files. Defaults to \"{}\".", LinkerArgs::DEFAULT_SOURCE_SEARCH_PATH))
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
    OPTION_ADD_ASSET_SEARCH_PATH,
    OPTION_ASSET_SEARCH_PATH,
    OPTION_GDT_SEARCH_PATH,
    OPTION_ADD_SOURCE_SEARCH_PATH,
    OPTION_SOURCE_SEARCH_PATH,
    OPTION_LOAD,
    OPTION_MENU_PERMISSIVE,
    OPTION_MENU_NO_OPTIMIZATION,
};

LinkerArgs::LinkerArgs()
    : m_argument_parser(COMMAND_LINE_OPTIONS, std::extent_v<decltype(COMMAND_LINE_OPTIONS)>)
{
}

void LinkerArgs::PrintUsage() const
{
    UsageInformation usage(m_argument_parser.GetExecutableName());

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
    con::info("OpenAssetTools Linker {}", GIT_VERSION);
}

void LinkerArgs::SetBinFolder()
{
    const fs::path path(utils::GetExecutablePath());
    m_bin_folder = path.parent_path().string();
}

bool LinkerArgs::ParseArgs(const int argc, const char** argv, bool& shouldContinue)
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

    SetBinFolder();

    m_project_specifiers_to_build = m_argument_parser.GetArguments();
    if (m_project_specifiers_to_build.empty())
    {
        // No projects to build specified...
        PrintUsage();
        return false;
    }

    // -v; --verbose
    if (m_argument_parser.IsOptionSpecified(OPTION_VERBOSE))
        con::globalLogLevel = con::LogLevel::DEBUG;
    else
        con::globalLogLevel = con::LogLevel::INFO;

    // b; --base-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_BASE_FOLDER))
        m_base_folder = m_argument_parser.GetValueForOption(OPTION_BASE_FOLDER);
    else
        m_base_folder = DEFAULT_BASE_FOLDER;

    // --output-folder
    if (m_argument_parser.IsOptionSpecified(OPTION_OUTPUT_FOLDER))
        m_out_folder = m_argument_parser.GetValueForOption(OPTION_OUTPUT_FOLDER);
    else
        m_out_folder = DEFAULT_OUTPUT_FOLDER;

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

    // --add-assets-search-path
    for (const auto& specifiedValue : m_argument_parser.GetParametersForOption(OPTION_ADD_ASSET_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(specifiedValue, m_asset_search_paths))
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

    // --add-source-search-path
    for (const auto& specifiedValue : m_argument_parser.GetParametersForOption(OPTION_ADD_SOURCE_SEARCH_PATH))
    {
        if (!FileUtils::ParsePathsString(specifiedValue, m_source_search_paths))
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
