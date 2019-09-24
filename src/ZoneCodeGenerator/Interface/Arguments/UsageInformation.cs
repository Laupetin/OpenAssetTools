using System;
using System.Linq;
using System.Text;

namespace ZoneCodeGenerator.Interface.Arguments
{
    static class UsageInformation
    {
        public static string FromCommandlineOptions(CommandLineOption[] options)
        {
            var usageInformation = new StringBuilder();
            usageInformation.AppendLine("Usage:");
            usageInformation.AppendLine();

            var longestShortName = 0;
            var longestLongName = 0;
            var longestArgumentLength = 0;

            foreach (var option in CommandLineOptions.ALL_OPTIONS)
            {
                if (option.ShortName != null && longestShortName < option.ShortName.Length)
                    longestShortName = option.ShortName.Length;

                if (option.LongName != null && longestLongName < option.LongName.Length)
                    longestLongName = option.LongName.Length;

                var argumentLength = GetOptionArgumentLength(option);
                if (longestArgumentLength < argumentLength)
                    longestArgumentLength = argumentLength;
            }

            var categories = CommandLineOptions.ALL_OPTIONS
                .Select(option => option.Category)
                .Distinct();

            var firstCategory = true;
            foreach (var category in categories)
            {
                var optionsOfCategory = CommandLineOptions.ALL_OPTIONS
                    .Where(option => category.Equals(option.Category));

                if(!firstCategory)
                    usageInformation.AppendLine();

                if (!string.IsNullOrEmpty(category))
                {
                    usageInformation.AppendLine($"== {category} ==");
                }

                foreach (var option in optionsOfCategory)
                {
                    if (option.ShortName != null)
                    {
                        usageInformation.Append($"-{option.ShortName}");
                        usageInformation.Append(' ', longestShortName - option.ShortName.Length);

                        if (option.LongName != null)
                            usageInformation.Append(", ");
                        else
                            usageInformation.Append(' ', 2);
                    }
                    else
                    {
                        usageInformation.Append(' ', longestShortName + 1 + 2);
                    }

                    if (option.LongName != null)
                    {
                        usageInformation.Append($"--{option.LongName}");
                        usageInformation.Append(' ', longestLongName - option.LongName.Length);
                    }
                    else
                    {
                        usageInformation.Append(' ', longestLongName + 2);
                    }
                    
                    usageInformation.Append(' ');

                    var argumentLength = GetOptionArgumentLength(option);
                    for(var i = 0; i < option.ParameterCount; i++)
                    {
                        if (i != 0)
                            usageInformation.Append(' ');

                        usageInformation.Append($"<{option.Parameters[i]}>");
                    }

                    usageInformation.Append(' ', longestArgumentLength - argumentLength + 1);

                    usageInformation.Append(option.Description);

                    usageInformation.AppendLine();
                }

                firstCategory = false;
            }

            return usageInformation.ToString();
        }

        private static int GetOptionArgumentLength(CommandLineOption option)
        {
            return option.ParameterCount * 2 // < and >
                   + option.Parameters.Select(s => s.Length).Sum()  // Length of the argument name
                   + Math.Max(0, option.ParameterCount - 1); // One space between each argument
        }
    }
}
