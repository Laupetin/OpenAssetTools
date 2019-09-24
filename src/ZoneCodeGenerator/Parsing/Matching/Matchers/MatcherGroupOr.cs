using System;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherGroupOr : GroupMatcher
    {
        private readonly TokenMatcher[] matchers;

        public MatcherGroupOr(params TokenMatcher[] matchers) : base(matchers)
        {
            if(matchers.Length == 0)
                throw new ArgumentException("A matcher group is supposed to have matchers.");

            this.matchers = matchers;
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            foreach(var matcher in matchers)
            {
                var matcherResult = matcher.Test(context, tokenOffset);

                if (matcherResult.Successful)
                    return matcherResult;
            }

            return new TokenMatchingResult(false, 0);
        }

        protected override string GetIdentifier()
        {
            return "GroupOr";
        }
    }
}
