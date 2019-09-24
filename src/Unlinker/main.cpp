#include "Utils/Arguments/ArgumentParser.h"

CommandLineOption* optionMinimalZoneFile = CommandLineOption::Builder::Create()
    .WithShortName("min")
    .WithLongName("minimal-zone")
    .WithDescription("Minimizes the size of the zone file output by only including assets that are not a dependency of another asset.")
    .Build();

CommandLineOption* optionOutputFolder = CommandLineOption::Builder::Create()
    .WithShortName("o")
    .WithLongName("output-folder")
    .WithDescription("Specifies the output folder containing the contents of the unlinked zones. Defaults to ./%zoneName%")
    .WithParameter("outputFolderPath")
    .Build();

CommandLineOption* commandLineOptions[]
{
    optionMinimalZoneFile,
    optionOutputFolder
};

int main(int argc, const char** argv)
{
    return 0;
}