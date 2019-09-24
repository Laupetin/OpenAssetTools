using System;
using System.Collections.Generic;

namespace ZoneCodeGenerator.Parsing.Matching
{
    abstract class TokenMatcher
    {
        private const int DebugPeekMin = 3;

        public string Tag { get; private set; }

        protected TokenMatcher()
        {
            Tag = "";
        }

        public TokenMatcher WithTag(string tag)
        {
            Tag = tag;
            return this;
        }

        protected abstract string GetIdentifier();

        public override string ToString()
        {
            return string.IsNullOrEmpty(Tag) ? GetIdentifier() : Tag;
        }

        protected abstract TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset);

        public virtual TokenMatchingResult Test(MatchingContext context, int tokenOffset)
        {
            var result = PerformTest(context, tokenOffset);

            if (context.Verbose)
            {
                if (result.Successful)
                {
                    var matchedTokens = new List<string>();

                    for (var i = 0; i < result.ConsumedTokenCount; i++)
                    {
                        matchedTokens.Add(context.Lexer.PeekToken(tokenOffset + i));
                    }

                    Console.WriteLine($"Matcher {ToString()} matched: {string.Join(" ", matchedTokens)}");
                }
                else
                {
                    var debugPeekCount = Math.Max(context.Lexer.CachedTokenCount - tokenOffset, DebugPeekMin);
                    var nextTokens = new List<string>();

                    for (var i = 0; i < debugPeekCount; i++)
                    {
                        nextTokens.Add(context.Lexer.PeekToken(tokenOffset + i));
                    }

                    Console.WriteLine($"Matcher {ToString()} failed @ {string.Join(" ", nextTokens)}");
                }
            }

            return result;
        }
    }
}
