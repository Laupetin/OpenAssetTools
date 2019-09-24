using System;
using System.Collections.Generic;
using System.Text;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherWithTag : TokenMatcher
    {
        private readonly string tag;

        public MatcherWithTag(string tag)
        {
            this.tag = tag;
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            var matcherWithTag = context.GetMatcherByTag(tag);

            if (matcherWithTag == null)
            {
                throw new Exception($"Could not find matcher with tag '{tag}'");
            }

            return matcherWithTag.Test(context, tokenOffset);
        }

        protected override string GetIdentifier()
        {
            return $"WithTag(\"{tag}\")";
        }
    }
}
