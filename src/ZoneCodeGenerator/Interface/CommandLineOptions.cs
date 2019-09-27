using ZoneCodeGenerator.Generating;
using ZoneCodeGenerator.Interface.Arguments;

namespace ZoneCodeGenerator.Interface
{
    static class CommandLineOptions
    {
        // ------
        // GENERAL
        // ------

        public static readonly CommandLineOption OPTION_HELP = CommandLineOption.CommandLineOptionBuilder.Create()
            .WithShortName("?")
            .WithLongName("help")
            .WithDescription("Show usage.")
            .Build();

        public static readonly CommandLineOption OPTION_VERBOSE = CommandLineOption.CommandLineOptionBuilder.Create()
            .WithShortName("v")
            .WithLongName("verbose")
            .WithDescription("Gives a lot and detailed output.")
            .Build();

        // ------
        // INPUT
        // ------
        private const string CategoryInput = "Input";

        public static readonly CommandLineOption OPTION_CREATE = CommandLineOption.CommandLineOptionBuilder.Create()
                .WithShortName("h")
                .WithLongName("header")
                .WithDescription("Create a new database from the specified header file.")
                .WithCategory(CategoryInput)
                .WithParameter("headerFile")
                .Build();

        // ------
        // EDITING
        // ------
        private const string CategoryEditing = "Editing";

        public static readonly CommandLineOption OPTION_EDITING_COMMANDS = CommandLineOption.CommandLineOptionBuilder.Create()
            .WithShortName("e")
            .WithLongName("editing-commands")
            .WithDescription("Specifies the editing command file. Defaults to stdin.")
            .WithCategory(CategoryEditing)
            .WithParameter("commandFile")
            .Build();

        // ------
        // OUTPUT
        // ------
        private const string CategoryOutput = "Output";

        public static readonly CommandLineOption OPTION_OUTPUT_FOLDER = CommandLineOption.CommandLineOptionBuilder.Create()
            .WithShortName("o")
            .WithLongName("output")
            .WithDescription("Specify the folder to save the generate code files to. Defaults to the current directory.")
            .WithCategory(CategoryOutput)
            .WithParameter("outputPath")
            .Build();

        public static readonly CommandLineOption OPTION_PRINT = CommandLineOption.CommandLineOptionBuilder.Create()
            .WithShortName("p")
            .WithLongName("print")
            .WithDescription("Print the loaded data.")
            .WithCategory(CategoryOutput)
            .Build();

        public static readonly CommandLineOption OPTION_GENERATE = CommandLineOption.CommandLineOptionBuilder.Create()
            .WithShortName("g")
            .WithLongName("generate")
            .WithDescription($"Generates a specified asset/preset combination. Can be used multiple times. Available presets: {string.Join(", ", CodeGenerator.Presets)}")
            .WithCategory(CategoryOutput)
            .WithParameter("assetName")
            .WithParameter("preset")
            .Reusable()
            .Build();
        
        public static readonly CommandLineOption[] ALL_OPTIONS =
        {
            // GENERAL
            OPTION_HELP,
            OPTION_VERBOSE,

            // INPUT
            OPTION_CREATE,

            // EDITING
            OPTION_EDITING_COMMANDS,

            // OUTPUT
            OPTION_OUTPUT_FOLDER,
            OPTION_PRINT,
            OPTION_GENERATE,
        };
    }
}
