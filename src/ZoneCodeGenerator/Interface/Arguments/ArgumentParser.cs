using System;
using System.Collections.Generic;
using System.Linq;

namespace ZoneCodeGenerator.Interface.Arguments
{
    class ArgumentParser
    {
        private readonly CommandLineOption[] options;

        private readonly List<string> noOptionArgs;
        private readonly Dictionary<CommandLineOption, List<string>> optionArgs;

        public IReadOnlyList<string> Arguments => noOptionArgs.AsReadOnly();

        public ArgumentParser(CommandLineOption[] options)
        {
            this.options = options;
            noOptionArgs = new List<string>();
            optionArgs = new Dictionary<CommandLineOption, List<string>>();
        }

        public bool ParseArguments(string[] args)
        {
            noOptionArgs.Clear();
            optionArgs.Clear();

            for (var i = 0; i < args.Length; i++)
            {
                var arg = args[i];

                if (arg.StartsWith("-"))
                {
                    CommandLineOption matchedOption;

                    if (arg.StartsWith("--"))
                    {
                        var longNameToFind = arg.Substring(2);
                        matchedOption = options.FirstOrDefault(option => longNameToFind.Equals(option.LongName));
                    }
                    else
                    {
                        var shortNameToFind = arg.Substring(1);
                        matchedOption = options.FirstOrDefault(option => shortNameToFind.Equals(option.ShortName));
                    }

                    if (matchedOption == null)
                    {
                        Console.WriteLine($"Unknown option '{arg}'.");
                        return false;
                    }

                    List<string> parameters;
                    if (optionArgs.ContainsKey(matchedOption))
                    {
                        if (!matchedOption.MultiUse)
                        {
                            Console.WriteLine($"Option '{arg}' already specified.");
                            return false;
                        }

                        parameters = optionArgs[matchedOption];
                    }
                    else
                    {
                        parameters = new List<string>();
                        optionArgs.Add(matchedOption, parameters);
                    }

                    if (i + matchedOption.ParameterCount >= args.Length)
                    {
                        Console.WriteLine($"Not enough parameters for option '{arg}'.");
                        return false;
                    }
                    
                    for (var parameter = 0; parameter < matchedOption.ParameterCount; parameter++)
                    {
                        if (args[i + parameter + 1].StartsWith("-"))
                        {
                            Console.WriteLine($"Not enough parameters for option '{arg}'.");
                            return false;
                        }

                        parameters.Add(args[i + parameter + 1]);
                    }

                    i += matchedOption.ParameterCount;
                }
                else
                {
                    noOptionArgs.Add(arg);
                }
            }

            return true;
        }

        public bool IsOptionSpecified(CommandLineOption option)
        {
            return optionArgs.ContainsKey(option);
        }

        public string GetValueForOption(CommandLineOption option)
        {
            return IsOptionSpecified(option) ? string.Join(" ", optionArgs[option]) : null;
        }

        public IReadOnlyList<string> GetParametersForOption(CommandLineOption option)
        {
            return IsOptionSpecified(option) ? optionArgs[option].AsReadOnly() : null;
        }
    }
}
