using System;
using System.Collections.Generic;
using System.Text;

namespace ZoneCodeGenerator.Parsing.Matching
{
    abstract class GroupMatcher : TokenMatcher
    {
        public TokenMatcher[] Matchers { get; }
        protected GroupMatcher(TokenMatcher[] matchers)
        {
            Matchers = matchers;
        }

        protected abstract override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset);
    }
}
