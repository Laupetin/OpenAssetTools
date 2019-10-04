using System;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherGroupAnd : GroupMatcher
    {
        public MatcherGroupAnd(params TokenMatcher[] matchers) : base(matchers)
        {
            if(matchers.Length == 0)
                throw new ArgumentException("A matcher group is supposed to have matchers.");
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            var result = new TokenMatchingResult(true, 0);
            result.AppendTag(Tag);

            foreach(var matcher in Matchers)
            {
                var matcherResult = matcher.Test(context, tokenOffset + result.ConsumedTokenCount);

                if (matcherResult.Successful)
                {
                    matcherResult.CopyTo(result);
                }
                else
                {
                    return new TokenMatchingResult(false, 0);
                }
            }

            return result;
        }

        protected override string GetIdentifier()
        {
            return "GroupAnd";
        }
    }
}
