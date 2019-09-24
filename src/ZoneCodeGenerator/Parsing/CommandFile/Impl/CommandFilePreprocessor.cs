using System.Text.RegularExpressions;
using ZoneCodeGenerator.Parsing.Impl;

namespace ZoneCodeGenerator.Parsing.CommandFile.Impl
{
    class CommandFilePreprocessor : IParsingFileStream
    {
        private static readonly Regex includeRegex = new Regex(@"^\s*include\s*(?:\""(.*)\""|\<(.*)\>)\s*$");

        private readonly IIncludingParsingStream parsingStream;
        private readonly ICommentProcessor commentProcessor;

        public CommandFilePreprocessor(IIncludingParsingStream parsingStream)
        {
            this.parsingStream = parsingStream;
            commentProcessor = new CommentProcessor();
        }

        public void Dispose()
        {
            parsingStream.Close();
        }

        public bool EndOfStream => parsingStream.EndOfStream;
        public string Filename => parsingStream.Filename;
        public int Line => parsingStream.Line;

        public string ReadLine()
        {
            return Preprocess(parsingStream.ReadLine());
        }

        public void Close()
        {
            parsingStream.Close();
        }

        private string Preprocess(string line)
        {
            line = commentProcessor.RemoveComments(line);

            var includeMatch = includeRegex.Match(line);

            if (includeMatch.Success)
            {
                var filename = includeMatch.Groups[1].Success ? includeMatch.Groups[1].Value : includeMatch.Groups[2].Value;

                parsingStream.IncludeFile(filename);

                return "";
            }

            return line;
        }
    }
}
