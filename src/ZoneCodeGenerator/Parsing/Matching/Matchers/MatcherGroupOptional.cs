namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherGroupOptional : GroupMatcher
    {
        private readonly TokenMatcher matcher;

        public MatcherGroupOptional(TokenMatcher matcher) : base(new []{matcher})
        {
            this.matcher = matcher;
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            var result = matcher.Test(context, tokenOffset);

            if (!result.Successful)
            {
                result = new TokenMatchingResult(true, 0);
            }

            result.PrependTag(Tag);
            return result;
        }

        protected override string GetIdentifier()
        {
            return "GroupOptional";
        }
    }
}
