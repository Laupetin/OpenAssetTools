#include "Utils/Arguments/ArgumentParser.h"
#include "Utils/Arguments/UsageInformation.h"
#include "ZoneLoading.h"
#include "ObjWriting.h"
#include "ContentPrinter.h"
#include "Utils/PathUtils.h"
#include "Utils/FileAPI.h"

#include <regex>
#include "ObjLoading.h"

const CommandLineOption* optionHelp = CommandLineOption::Builder::Create()
                                      .WithShortName("?")
                                      .WithLongName("help")
                                      .WithDescription("Displays usage information.")
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

const CommandLineOption* commandLineOptions[]
{
    optionHelp,
    optionMinimalZoneFile,
    optionList,
    optionOutputFolder
};

void PrintUsage()
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

std::string ResolveOutputFolderPath(const std::string& path, Zone* zone)
{
    return std::regex_replace(path, std::regex("%zoneName%"), zone->m_name);
}

bool HandleZone(Zone* zone, ArgumentParser* argumentParser)
{
    if (argumentParser->IsOptionSpecified(optionList))
    {
        const ContentPrinter printer(zone);
        printer.PrintContent();
    }
    else
    {
        const bool minimalisticZoneDefinition = argumentParser->IsOptionSpecified(optionMinimalZoneFile);

        std::string outputFolderPath;

        if (argumentParser->IsOptionSpecified(optionOutputFolder))
        {
            outputFolderPath = ResolveOutputFolderPath(argumentParser->GetValueForOption(optionOutputFolder), zone);
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
            printf("Failed to open file for zone definition file of zone '%s'.\n", zone->m_name.c_str());
            return false;
        }

        zoneDefinitionFile.Close();
    }

    return true;
}

int main(const int argc, const char** argv)
{
    ArgumentParser argumentParser(commandLineOptions, _countof(commandLineOptions));

    if (!argumentParser.ParseArguments(argc, argv))
    {
        PrintUsage();
        return 1;
    }

    if (argumentParser.IsOptionSpecified(optionHelp))
    {
        PrintUsage();
        return 0;
    }

    const std::vector<std::string> arguments = argumentParser.GetArguments();
    const size_t argCount = arguments.size();
    if (argCount <= 1)
    {
        PrintUsage();
        return 1;
    }

    for (unsigned argIndex = 1; argIndex < argCount; argIndex++)
    {
        const std::string& zonePath = arguments[argIndex];

        Zone* zone = ZoneLoading::LoadZone(zonePath);
        if (zone == nullptr)
        {
            printf("Failed to load zone '%s'.\n", zonePath.c_str());
            return 1;
        }

        ObjLoading::LoadReferencedContainersForZone(zone);
        ObjLoading::LoadObjDataForZone(zone);

        if(!HandleZone(zone, &argumentParser))
        {
            return 1;
        }

        delete zone;
    }

    return 0;
}
