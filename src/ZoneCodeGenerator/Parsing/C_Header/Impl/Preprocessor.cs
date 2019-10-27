using System.Collections.Generic;
using System.Text.RegularExpressions;
using ZoneCodeGenerator.Parsing.Impl;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Parsing.C_Header.Impl
{
    class Preprocessor : IParsingFileStream
    {
        private static readonly Regex packPushRegex = new Regex(@"^\s*#pragma\s+pack\s*\(\s*push\s*,\s*(\d+)\s*\)\s*$");
        private static readonly Regex packPopRegex = new Regex(@"^\s*#pragma\s+pack\s*\(\s*pop\s*\)\s*$");
        private static readonly Regex defineRegex = new Regex(@"^\s*#define\s*(\w+)(?:\s*(.*))?$");
        private static readonly Regex undefRegex = new Regex(@"^\s*#undef\s*(\w+)\s*$");
        private static readonly Regex includeRegex = new Regex(@"^\s*#include\s*(?:\""(.*)\""|\<(.*)\>)\s*$");

        private readonly IIncludingParsingStream streamFileSystem;
        private readonly IHeaderParserState state;
        private readonly ICommentProcessor commentProcessor;

        private readonly Dictionary<string, string> defines;

        public bool EndOfStream => streamFileSystem.EndOfStream;
        public string Filename => streamFileSystem.Filename;
        public int Line => streamFileSystem.Line;

        public Preprocessor(IIncludingParsingStream includingParsingStream, IHeaderParserState state)
        {
            streamFileSystem = includingParsingStream;
            this.state = state;
            defines = new Dictionary<string, string>();
            commentProcessor = new CommentProcessor();
        }

        private void ParseCompilerExpression(string line)
        {
            var packPush = packPushRegex.Match(line.ToLower());
            if(packPush.Success)
            {
                state.PushPack(int.Parse(packPush.Groups[1].Value));
                return;
            }

            var packPop = packPopRegex.Match(line.ToLower());
            if(packPop.Success)
            {
                state.PopPack();
                return;
            }

            var define = defineRegex.Match(line);
            if (define.Success)
            {
                var key = define.Groups[1].Value;
                var value = define.Groups[2].Value.Trim();

                if (defines.ContainsKey(key))
                    defines[key] = value;
                else
                    defines.Add(key, value);
                return;
            }

            var undef = undefRegex.Match(line);
            if (undef.Success)
            {
                var key = undef.Groups[1].Value;

                if(defines.ContainsKey(key))
                    defines.Remove(key);
                return;
            }

            var include = includeRegex.Match(line);
            if (include.Success)
            {
                var filename = include.Groups[1].Success ? include.Groups[1].Value : include.Groups[2].Value;

                streamFileSystem.IncludeFile(filename);
                return;
            }
        }

        private string Preprocess(string line)
        {
            bool defineMatched;
            line = commentProcessor.RemoveComments(line);

            if (line.StartsWith("#"))
            {
                ParseCompilerExpression(line);
                return "";
            }

            do
            {
                defineMatched = false;
                foreach (var (defineKey, defineValue) in defines)
                {
                    var match = Regex.Match(line, $@"^(.*\W)?{defineKey}(\W.*)?$");

                    if (!match.Success) continue;

                    line = match.Groups[1].Value + defineValue + match.Groups[2].Value;
                    defineMatched = true;
                    break;
                }

            } while (defineMatched);

            return line;
        }

        public string ReadLine()
        {
            return Preprocess(streamFileSystem.ReadLine());
        }

        public void Dispose()
        {
            Close();
        }

        public void Close()
        {
            streamFileSystem.Close();
        }
    }
}
