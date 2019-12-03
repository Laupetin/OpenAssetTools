using System.Collections.Generic;
using System.Linq;

namespace ZoneCodeGenerator.Parsing.Impl
{
    class Lexer : ILexer
    {
        private class LexerToken
        {
            public string Token { get; set; }
            public int Row { get; set; }
            public string Filename { get; set; }
        }

        private static readonly char[] charTokens = {
            ';',
            ':',
            '{',
            '}',
            ',',
            '.',
            '=',
            '(',
            ')',
            '*',
            '[',
            ']',
            '<',
            '>',
            '#',
            '&',
            '%',
            '\"',
            '?',
            '!',
            '+',
            '-',
            '/',
            '\\',
            '|'
        };

        private readonly IParsingFileStream fileStream;

        private readonly List<LexerToken> tokenCache;

        public bool IsEndOfStream => fileStream.EndOfStream && tokenCache.Count == 0;
        public int CachedTokenCount => tokenCache.Count;

        public Lexer(IParsingFileStream fileStream)
        {
            this.fileStream = fileStream;
            tokenCache = new List<LexerToken>();
        }

        public int CurrentLine => tokenCache.Count == 0 ? fileStream.Line : tokenCache[0].Row;

        public string CurrentFile => tokenCache.Count == 0 ? fileStream.Filename : tokenCache[0].Filename;

        private static bool IsCharToken(char c)
        {
            return charTokens.Contains(c);
        }

        private void TokenizeLine(string filename, int lineNr, string line)
        {
            line = line.Trim();

            while(line.Length > 0)
            {
                var token = "";

                while (line.Length > 0)
                {
                    var c = line[0];
                    line = line.Substring(1);

                    if (char.IsWhiteSpace(c))
                        break;

                    if (IsCharToken(c))
                    {
                        if(token == "")
                        {
                            token += c;
                            break;
                        }
                        else
                        {
                            line = c + line;
                            break;
                        }
                    }

                    token += c;
                }

                tokenCache.Add(new LexerToken()
                {
                    Token = token,
                    Row = lineNr,
                    Filename = filename
                });
                line = line.TrimStart();
            }
        }

        public string NextToken()
        {
            while(tokenCache.Count == 0)
            {
                if (fileStream.EndOfStream)
                {
                    return "";
                }

                TokenizeLine(fileStream.Filename, fileStream.Line, fileStream.ReadLine());
            }

            var token = tokenCache[0].Token;
            tokenCache.RemoveAt(0);
            return token;
        }

        public string PeekToken(int index = 0)
        {
            if (index < 0)
                return "";

            while (tokenCache.Count <= index)
            {
                if (fileStream.EndOfStream)
                {
                    return "";
                }

                TokenizeLine(fileStream.Filename, fileStream.Line, fileStream.ReadLine());
            }
            
            return tokenCache[index].Token;
        }

        public void SkipTokens(int count)
        {
            if (count < 0)
                return;

            while (tokenCache.Count < count)
            {
                if (fileStream.EndOfStream)
                {
                    tokenCache.Clear();
                    return;
                }

                TokenizeLine(fileStream.Filename, fileStream.Line, fileStream.ReadLine());
            }

            tokenCache.RemoveRange(0, count);
        }
    }
}
