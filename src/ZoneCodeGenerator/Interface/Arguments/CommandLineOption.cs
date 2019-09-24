using System.Collections.Generic;

namespace ZoneCodeGenerator.Interface.Arguments
{
    class CommandLineOption
    {
        public string ShortName { get; private set; }
        public string LongName { get; private set; }
        public string Description { get; private set; }
        public string Category { get; private set; }
        public bool MultiUse { get; private set; }

        private readonly List<string> parameters;
        public IReadOnlyList<string> Parameters => parameters.AsReadOnly();
        public int ParameterCount => parameters.Count;

        private CommandLineOption()
        {
            ShortName = null;
            LongName = null;
            Description = "";
            Category = "";
            MultiUse = false;
            parameters = new List<string>();
        }

        public class CommandLineOptionBuilder
        {
            private readonly CommandLineOption option;

            private CommandLineOptionBuilder()
            {
                option = new CommandLineOption();
            }

            public static CommandLineOptionBuilder Create()
            {
                return new CommandLineOptionBuilder();
            }

            public CommandLineOptionBuilder WithShortName(string shortName)
            {
                option.ShortName = shortName;
                return this;
            }

            public CommandLineOptionBuilder WithLongName(string longName)
            {
                option.LongName = longName;
                return this;
            }

            public CommandLineOptionBuilder WithDescription(string description)
            {
                option.Description = description;
                return this;
            }

            public CommandLineOptionBuilder WithCategory(string category)
            {
                option.Category = category;
                return this;
            }

            public CommandLineOptionBuilder WithParameter(string argumentName)
            {
                option.parameters.Add(argumentName);
                return this;
            }

            public CommandLineOptionBuilder Reusable()
            {
                option.MultiUse = true;
                return this;
            }

            public CommandLineOption Build()
            {
                return option;
            }
        }
    }
}
