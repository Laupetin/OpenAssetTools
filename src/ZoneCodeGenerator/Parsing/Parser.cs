using System;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing
{
    class Parser<TState> where TState : IParserState<TState>
    {
        // Amount of consecutive tokens that are output after a parser error to show the region where the error happened.
        private const int ParserErrorTokenPeekCount = 3;

        private readonly ILexer lexer;
        private readonly TState state;

        public Parser(TState state, ILexer lexer)
        {
            this.state = state;
            this.lexer = lexer;
        }

        public bool Parse(bool verbose = false)
        {
            while (!lexer.IsEndOfStream)
            {
                var testSuccessful = false;

                try
                {
                    foreach (var test in state.GetTests())
                    {
                        switch (test.PerformTest(state, lexer, verbose))
                        {
                            case TokenTestResult.Match:
                                lexer.SkipTokens(test.ConsumedTokenCount);
                                testSuccessful = true;
                                break;
                            case TokenTestResult.NoMatch:
                                break;
                            default:
                                throw new ArgumentOutOfRangeException();
                        }

                        if (testSuccessful)
                            break;
                    }
                }
                catch (LoadingException e)
                {
                    PrintParserError(e);
                    return false;
                }

                if (!testSuccessful)
                {
                    PrintParserError(new LoadingException("Could not parse expression"));
                    return false;
                }

                // Peek for the next token here to be able to determine if the stream is at its end or not
                lexer.PeekToken();
            }

            return true;
        }

        private void PrintParserError(LoadingException e)
        {
            Console.WriteLine($"Parsing failed in file '{lexer.CurrentFile}' at line {lexer.CurrentLine}:");
            
            Console.WriteLine(e.Message);

            Console.Write("  ");

            var tokenCountToShow = Math.Max(ParserErrorTokenPeekCount, lexer.CachedTokenCount);
            for (var i = 0; i < tokenCountToShow; i++)
            {
                var token = lexer.PeekToken(i);
                if (string.IsNullOrEmpty(token))
                    break;
                Console.Write($"{token} ");
            }
            Console.WriteLine();
            Console.WriteLine();
        }
    }
}
