#include "Unlinker.h"

#include "Utils/Arguments/ArgumentParser.h"
#include "Utils/Arguments/UsageInformation.h"
#include "ZoneLoading.h"
#include "ObjWriting.h"
#include "ContentPrinter.h"
#include "Utils/PathUtils.h"
#include "Utils/FileAPI.h"
#include "ObjLoading.h"
#include "SearchPath/SearchPaths.h"
#include "SearchPath/SearchPathFilesystem.h"

#include <set>
#include <regex>
#include <filesystem>

const CommandLineOption* optionHelp = CommandLineOption::Builder::Create()
                                      .WithShortName("?")
                                      .WithLongName("help")
                                      .WithDescription("Displays usage information.")
                                      .Build();

const CommandLineOption* optionVerbose = CommandLineOption::Builder::Create()
                                         .WithShortName("v")
                                         .WithLongName("verbose")
                                         .WithDescription("Outputs a lot more and more detailed messages.")
                                         .Build();

const CommandLineOption* optionMinimalZoneFile = CommandLineOption::Builder::Create()
                                                 .WithShortName("min")
                                                 .WithLongName("minimal-zone")
                                                 .WithDescription(
                                                     "Minimizes the size of the zone file output by only including assets that are not a dependency of another asset.")
                                                 .Build();

const CommandLineOption* optionList = CommandLineOption::Builder::Create()
                                      .WithShortName("l")
                                      .WithLongName("list")
                                      .WithDescription(
                                          "Lists the contents of a zone instead of writing them to the disk.")
                                      .Build();

const CommandLineOption* optionOutputFolder = CommandLineOption::Builder::Create()
                                              .WithShortName("o")
                                              .WithLongName("output-folder")
                                              .WithDescription(
                                                  "Specifies the output folder containing the contents of the unlinked zones. Defaults to ./%zoneName%")
                                              .WithParameter("outputFolderPath")
                                              .Build();

const CommandLineOption* optionSearchPath = CommandLineOption::Builder::Create()
                                            .WithLongName("search-path")
                                            .WithDescription(
                                                "Specifies a semi-colon separated list of paths to search for additional game files.")
                                            .WithParameter("searchPathString")
                                            .Build();

const CommandLineOption* commandLineOptions[]
{
    optionHelp,
    optionVerbose,
    optionMinimalZoneFile,
    optionList,
    optionOutputFolder,
    optionSearchPath
};

class Unlinker::Impl
{
    SearchPaths m_search_paths;
    SearchPathFilesystem* m_last_zone_search_path;
    std::set<std::string> m_absolute_search_paths;

    ArgumentParser m_argument_parser;
    bool m_verbose;
    bool m_should_load_obj;

    /**
     * \brief Loads a search path.
     * \param searchPath The search path to load.
     */
    void LoadSearchPath(SearchPathFilesystem* searchPath)
    {
        if(m_should_load_obj)
        {
            if(m_verbose)
            {
                printf("Loading search path: \"%s\"\n", searchPath->GetPath().c_str());
            }
        }
    }

    /**
     * \brief Unloads a search path.
     * \param searchPath The search path to unload.
     */
    void UnloadSearchPath(SearchPathFilesystem* searchPath)
    {
        if(m_should_load_obj)
        {
            if(m_verbose)
            {
                printf("Unloading search path: \"%s\"\n", searchPath->GetPath().c_str());
            }
        }
    }

    /**
     * \brief Loads all search paths that are valid for the specified zone and returns them.
     * \param zonePath The path to the zone file that should be prepared for.
     * \return A \c SearchPaths object that contains all search paths that should be considered when loading the specified zone.
     */
    SearchPaths GetSearchPathsForZone(const std::string& zonePath)
    {
        SearchPaths searchPathsForZone;
        const std::string absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

        if (m_last_zone_search_path != nullptr && m_last_zone_search_path->GetPath() == absoluteZoneDirectory)
        {
            searchPathsForZone.IncludeSearchPath(m_last_zone_search_path);
        }
        else if (m_absolute_search_paths.find(absoluteZoneDirectory) == m_absolute_search_paths.end())
        {
            if (m_last_zone_search_path != nullptr)
            {
                UnloadSearchPath(m_last_zone_search_path);
                delete m_last_zone_search_path;
            }

            m_last_zone_search_path = new SearchPathFilesystem(absoluteZoneDirectory);
            searchPathsForZone.IncludeSearchPath(m_last_zone_search_path);
            LoadSearchPath(m_last_zone_search_path);
        }

        return searchPathsForZone;
    }

    /**
     * \brief Splits a path string as user input into a list of paths.
     * \param pathsString The path string that was taken as user input.
     * \param output A set for strings to save the output to.
     * \return \c true if the user input was valid and could be processed successfully, otherwise \c false.
     */
    static bool ParsePathsString(const std::string& pathsString, std::set<std::string>& output)
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

    /**
     * \brief Initializes the Unlinker object's search paths based on the user's input.
     * \return \c true if building the search paths was successful, otherwise \c false.
     */
    bool BuildSearchPaths()
    {
        if (m_argument_parser.IsOptionSpecified(optionSearchPath))
        {
            std::set<std::string> pathList;
            if (!ParsePathsString(m_argument_parser.GetValueForOption(optionSearchPath), pathList))
            {
                return false;
            }

            for (const auto& path : pathList)
            {
                std::string absolutePath = std::filesystem::absolute(path).string();

                SearchPathFilesystem* searchPath = new SearchPathFilesystem(absolutePath);
                LoadSearchPath(searchPath);
                m_search_paths.CommitSearchPath(searchPath);

                m_absolute_search_paths.insert(absolutePath);
            }
        }

        if (m_verbose)
        {
            printf("%u SearchPaths%s\n", m_absolute_search_paths.size(), !m_absolute_search_paths.empty() ? ":" : "");
            for (const auto& absoluteSearchPath : m_absolute_search_paths)
            {
                printf("  \"%s\"\n", absoluteSearchPath.c_str());
            }

            if (!m_absolute_search_paths.empty())
            {
                puts("");
            }
        }

        return true;
    }

    /**
     * \brief Prints a command line usage help text for the Unlinker tool to stdout.
     */
    static void PrintUsage()
    {
        UsageInformation usage("unlinker.exe");

        for (auto commandLineOption : commandLineOptions)
        {
            usage.AddCommandLineOption(commandLineOption);
        }

        usage.AddArgument("pathToZone");
        usage.SetVariableArguments(true);

        usage.Print();
    }

    /**
     * \brief Converts the output path specified by command line arguments to a path applies for the specified zone.
     * \param path The path that was specified as user input.
     * \param zone The zone to resolve the path input for.
     * \return An output path for the zone based on the user input.
     */
    std::string ResolveOutputFolderPath(const std::string& path, Zone* zone) const
    {
        return std::regex_replace(path, std::regex("%zoneName%"), zone->m_name);
    }

    /**
     * \brief Performs the tasks specified by the command line arguments on the specified zone.
     * \param zone The zone to handle.
     * \return \c true if handling the zone was successful, otherwise \c false.
     */
    bool HandleZone(Zone* zone)
    {
        if (m_argument_parser.IsOptionSpecified(optionList))
        {
            const ContentPrinter printer(zone);
            printer.PrintContent();
        }
        else
        {
            const bool minimalisticZoneDefinition = m_argument_parser.IsOptionSpecified(optionMinimalZoneFile);

            std::string outputFolderPath;
            if (m_argument_parser.IsOptionSpecified(optionOutputFolder))
            {
                outputFolderPath = ResolveOutputFolderPath(m_argument_parser.GetValueForOption(optionOutputFolder),
                                                           zone);
            }
            else
            {
                outputFolderPath = ResolveOutputFolderPath("./%zoneName%", zone);
            }

            FileAPI::DirectoryCreate(outputFolderPath);

            const std::string zoneDefinitionFileFolder = utils::Path::Combine(outputFolderPath, "zone_source");
            FileAPI::DirectoryCreate(zoneDefinitionFileFolder);

            FileAPI::File zoneDefinitionFile = FileAPI::Open(
                utils::Path::Combine(zoneDefinitionFileFolder, zone->m_name + ".zone"),
                FileAPI::Mode::MODE_WRITE);

            if (zoneDefinitionFile.IsOpen())
            {
                ObjWriting::WriteZoneDefinition(zone, &zoneDefinitionFile, minimalisticZoneDefinition);
                ObjWriting::DumpZone(zone, outputFolderPath);
            }
            else
            {
                printf("Failed to open file for zone definition file of zone \"%s\".\n", zone->m_name.c_str());
                return false;
            }

            zoneDefinitionFile.Close();
        }

        return true;
    }

public:
    Impl()
        : m_argument_parser(commandLineOptions, _countof(commandLineOptions))
    {
        m_last_zone_search_path = nullptr;
        m_verbose = false;
        m_should_load_obj = false;
    }

    /**
     * \copydoc Unlinker::Start
     */
    bool Start(const int argc, const char** argv)
    {
        if (!m_argument_parser.ParseArguments(argc, argv))
        {
            PrintUsage();
            return false;
        }

        m_verbose = m_argument_parser.IsOptionSpecified(optionVerbose);
        m_should_load_obj = !m_argument_parser.IsOptionSpecified(optionList);

        // Check if the user requested help
        if (m_argument_parser.IsOptionSpecified(optionHelp))
        {
            PrintUsage();
            return true;
        }

        const std::vector<std::string> arguments = m_argument_parser.GetArguments();
        const size_t argCount = arguments.size();
        if (argCount <= 1)
        {
            // No zones to load specified...
            PrintUsage();
            return false;
        }

        if (!BuildSearchPaths())
        {
            return false;
        }

        for (unsigned argIndex = 1; argIndex < argCount; argIndex++)
        {
            const std::string& zonePath = arguments[argIndex];
            std::string absoluteZoneDirectory = absolute(std::filesystem::path(zonePath).remove_filename()).string();

            SearchPaths searchPathsForZone = GetSearchPathsForZone(absoluteZoneDirectory);

            Zone* zone = ZoneLoading::LoadZone(zonePath);
            if (zone == nullptr)
            {
                printf("Failed to load zone \"%s\".\n", zonePath.c_str());
                return false;
            }
            
            if(m_verbose)
            {
                printf("Loaded zone \"%s\"\n", zone->m_name.c_str());
            }

            if (m_should_load_obj)
            {
                ObjLoading::LoadReferencedContainersForZone(&searchPathsForZone, zone);
                ObjLoading::LoadObjDataForZone(&searchPathsForZone, zone);
            }

            if (!HandleZone(zone))
            {
                return false;
            }

            if (m_should_load_obj)
            {
                ObjLoading::UnloadContainersOfZone(zone);
            }

            delete zone;
        }

        return true;
    }
};

Unlinker::Unlinker()
{
    m_impl = new Impl();
}

Unlinker::~Unlinker()
{
    delete m_impl;
    m_impl = nullptr;
}

bool Unlinker::Start(const int argc, const char** argv) const
{
    return m_impl->Start(argc, argv);
}
