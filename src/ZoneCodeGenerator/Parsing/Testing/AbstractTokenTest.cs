using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Parsing.Testing
{
    abstract class AbstractTokenTest<TState> : ITokenTest<TState> where TState : IParserState<TState>
    {
        private TokenMatchingResult lastResult;
        private int tagOffset;
        private readonly Dictionary<string, int> matchOffset;

        public int ConsumedTokenCount => lastResult?.ConsumedTokenCount ?? 0;

        private readonly TokenMatcher matcherEntryPoint;
        private readonly Dictionary<string, TokenMatcher> taggedMatchers;

        protected AbstractTokenTest(TokenMatcher[] matcherEntryPoint)
        {
            this.matcherEntryPoint = new MatcherGroupAnd(matcherEntryPoint);
            taggedMatchers = new Dictionary<string, TokenMatcher>();

            lastResult = null;
            tagOffset = 0;
            matchOffset = new Dictionary<string, int>();

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

        private void Reset()
        {
            lastResult = null;
            tagOffset = 0;
            matchOffset.Clear();
        }

        protected void AddTaggedMatcher(params TokenMatcher[] taggedMatcher)
        {
            BuildTaggedMatcherList(taggedMatcher);
        }

        protected string PeekTag()
        {
            return lastResult?.MatchedTags.ElementAtOrDefault(tagOffset) ?? "";
        }

        protected string NextTag()
        {
            return lastResult?.MatchedTags.ElementAtOrDefault(tagOffset++) ?? "";
        }

        protected bool HasMatcherTokens(string matcherName)
        {
            return GetMatcherTokenCount(matcherName) > 0;
        }

        protected int GetMatcherTokenCount(string matcherName)
        {
            if (lastResult == null || !lastResult.NamedMatches.TryGetValue(matcherName, out var matches))
            {
                return 0;
            }

            return matches.Count;
        }

        protected string PeekMatch(string matcherName)
        {
            if (!lastResult.NamedMatches.TryGetValue(matcherName, out var matches))
            {
                return null;
            }

            if (!matchOffset.TryGetValue(matcherName, out var offset))
            {
                offset = 0;
            }

            return matches.ElementAtOrDefault(offset);
        }

        protected string NextMatch(string matcherName)
        {
            if (!lastResult.NamedMatches.TryGetValue(matcherName, out var matches))
            {
                return null;
            }

            if (!matchOffset.TryGetValue(matcherName, out var offset))
            {
                offset = 0;
            }

            var result = matches.ElementAtOrDefault(offset++);

            matchOffset[matcherName] = offset;

            return result;
        }

        protected abstract void ProcessMatch(TState state);

        public TokenTestResult PerformTest(TState state, ILexer lexer, bool verbose = false)
        {
            var context = new MatchingContext(lexer, taggedMatchers)
            {
                Verbose = verbose
            };

            if (context.Verbose)
            {
                Console.WriteLine($"Test {GetType().Name} start");
            }

            Reset();
            lastResult = matcherEntryPoint.Test(context, 0);

            if (!lastResult.Successful)
            {
                if (context.Verbose)
                {
                    Console.WriteLine($"Test {GetType().Name} failed");
                }

                return TokenTestResult.NoMatch;
            }

            ProcessMatch(state);

            if (context.Verbose)
            {
                Console.WriteLine($"Test {GetType().Name} successful");
            }

            return TokenTestResult.Match;
        }
    }
}
