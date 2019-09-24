using System;

namespace ZoneCodeGenerator.Utils
{
    static class RandomName
    {
        private const int NameLen = 32;

        private static readonly Random random = new Random();
        private static readonly char[] generatorChars = {
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
        };

        public static string GenerateName()
        {
            var name = "";
            for (var i = 0; i < NameLen; i++)
                name += generatorChars[random.Next(generatorChars.Length)];

            return name;
        }
    }
}
