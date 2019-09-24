using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using ZoneCodeGenerator.Parsing.Matching;

namespace ZoneCodeGenerator.Parsing.Testing
{
    abstract class AbstractTokenTest<TState> : ITokenTest<TState> where TState : IParserState<TState>
    {
        public int ConsumedTokenCount { get; private set; }

        private readonly TokenMatcher[] matcherEntryPoint;
        private readonly Dictionary<string, List<string>> matchedEntries;
        private readonly Dictionary<string, TokenMatcher> taggedMatchers;
        private bool tested;

        protected AbstractTokenTest(TokenMatcher[] matcherEntryPoint)
        {
            this.matcherEntryPoint = matcherEntryPoint;
            matchedEntries = new Dictionary<string, List<string>>();
            taggedMatchers = new Dictionary<string, TokenMatcher>();
            tested = false;

            BuildTaggedMatcherList(matcherEntryPoint);
        }

        private void BuildTaggedMatcherList(IEnumerable<TokenMatcher> matchersWithTags)
        {
            foreach (var tokenMatcher in matchersWithTags)
            {
                if (!string.IsNullOrEmpty(tokenMatcher.Tag))
                {
                    taggedMatchers[tokenMatcher.Tag] = tokenMatcher;
                }

                if (tokenMatcher is GroupMatcher groupMatcher)
                {
                    BuildTaggedMatcherList(groupMatcher.Matchers);
                }
            }
        }

        protected void AddTaggedMatcher(params TokenMatcher[] taggedMatcher)
        {
            BuildTaggedMatcherList(taggedMatcher);
        }

        protected ReadOnlyCollection<string> GetMatcherTokens(string matcherName)
        {
            return tested && matchedEntries.ContainsKey(matcherName) ? matchedEntries[matcherName].AsReadOnly() : new List<string>().AsReadOnly();
        }

        protected bool HasMatcherTokens(string matcherName)
        {
            return tested && matchedEntries.ContainsKey(matcherName);
        }

        protected abstract void ProcessMatch(TState state);

        public TokenTestResult PerformTest(TState state, ILexer lexer)
        {
            var tokenOffset = 0;
            matchedEntries.Clear();

            ConsumedTokenCount = 0;
            tested = true;

            var context = new MatchingContext(lexer, taggedMatchers);

            if (context.Verbose)
            {
                Console.WriteLine($"Test {GetType().Name} start");
            }

            foreach(var matcher in matcherEntryPoint)
            {
                var result = matcher.Test(context, tokenOffset);

                if (!result.Successful)
                {
                    if (context.Verbose)
                    {
                        Console.WriteLine($"Test {GetType().Name} failed");
                    }

                    return TokenTestResult.NoMatch;
                }

                tokenOffset += result.ConsumedTokenCount;

                foreach (var entry in result)
                {
                    matchedEntries.Add(entry.Key, entry.Value);
                }
            }

            ConsumedTokenCount = tokenOffset;

            ProcessMatch(state);

            if (context.Verbose)
            {
                Console.WriteLine($"Test {GetType().Name} successful");
            }

            return TokenTestResult.Match;
        }
    }
}
