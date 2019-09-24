using System;
using System.Linq;
using ZoneCodeGenerator.Generating;
using ZoneCodeGenerator.Interface.Arguments;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.CommandFile;

namespace ZoneCodeGenerator.Interface
{
    class CUI
    {
        public bool Start(string[] args)
        {
            var argumentParser = new ArgumentParser(CommandLineOptions.ALL_OPTIONS);

            if (args.Length == 0 || !argumentParser.ParseArguments(args))
            {
                PrintUsage();
                return false;
            }

            if (argumentParser.IsOptionSpecified(CommandLineOptions.OPTION_HELP))
            {
                PrintUsage();
                return true;
            }

            var session = new CUISession();

            if (argumentParser.IsOptionSpecified(CommandLineOptions.OPTION_OUTPUT_FOLDER))
            {
                session.GeneratorOutputPath = argumentParser.GetValueForOption(CommandLineOptions.OPTION_OUTPUT_FOLDER);
            }

            if (argumentParser.IsOptionSpecified(CommandLineOptions.OPTION_CREATE))
            {
                session.SourceFilePath = argumentParser.GetValueForOption(CommandLineOptions.OPTION_CREATE);
                session.Repository = HeaderReader.ReadFile(session.SourceFilePath);

                if (session.Repository == null)
                {
                    Console.WriteLine($"Creating a database from header file '{argumentParser.GetValueForOption(CommandLineOptions.OPTION_CREATE)}' failed.");
                    return false;
                }
            }
            else
            {
                Console.WriteLine("Parsing a c header is needed to perform any other action.");
                return false;
            }

            if (argumentParser.IsOptionSpecified(CommandLineOptions.OPTION_EDITING_COMMANDS))
            {
                if (!CommandFileReader.ReadFile(argumentParser.GetValueForOption(CommandLineOptions.OPTION_EDITING_COMMANDS), session))
                {
                    return false;
                }
            }

            if (argumentParser.IsOptionSpecified(CommandLineOptions.OPTION_PRINT))
            {
                var printer = new PrettyPrinter(session.Repository);
                printer.PrintAll();
            }

            if (argumentParser.IsOptionSpecified(CommandLineOptions.OPTION_GENERATE))
            {
                var generationArgs = argumentParser.GetParametersForOption(CommandLineOptions.OPTION_GENERATE);
                for (var i = 0; i < generationArgs.Count; i += CommandLineOptions.OPTION_GENERATE.ParameterCount)
                {
                    var assetName = generationArgs[i];
                    var preset = generationArgs[i + 1];

                    var assets = session.Repository.GetAllStructureInformation()
                        .Where(inf => inf.IsAsset);

                    if (!assetName.Equals("*"))
                        assets = assets.Where(inf =>
                            inf.Type.FullName.Equals(assetName, StringComparison.CurrentCultureIgnoreCase));

                    if (!assets.Any())
                    {
                        Console.WriteLine($"Could not find asset '{assetName}'.");
                        continue;
                    }

                    foreach (var asset in assets)
                    {
                        if (CodeGenerator.GenerateCodeForPreset(preset, asset, session))
                        {
                            Console.WriteLine($"Successfully generated code for asset '{asset.Type.FullName}' with preset '{preset}'");
                        }
                        else
                        {
                            Console.WriteLine($"Could not generate code for asset '{asset.Type.FullName}' with preset '{preset}'");
                        }
                    }
                }
            }

            return true;
        }

        private static void PrintUsage()
        {
            Console.WriteLine(UsageInformation.FromCommandlineOptions(CommandLineOptions.ALL_OPTIONS));
        }
    }
}
