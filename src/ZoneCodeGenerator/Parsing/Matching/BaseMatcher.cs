namespace ZoneCodeGenerator.Parsing.Matching
{
    abstract class BaseMatcher : TokenMatcher
    {
        protected class MatchingResult
        {
            public bool Successful { get; }
            public int TokensConsumed { get; }

            public MatchingResult(bool successful, int tokensConsumed)
            {
                Successful = successful;
                TokensConsumed = tokensConsumed;
            }
        }

        private string name;
        private string output;
        private bool consuming;

        protected BaseMatcher()
        {
            name = null;
            consuming = true;
            output = "";
        }

        protected abstract MatchingResult Matches(MatchingContext context, int tokenOffset);

        protected void SetMatcherOutput(string text)
        {
            output = text;
        }

        public BaseMatcher WithName(string tokenName)
        {
            name = tokenName;
            return this;
        }

        public BaseMatcher NonConsuming()
        {
            consuming = false;
            return this;
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            var match = Matches(context, tokenOffset);

            var success = match.Successful;
            var consumedTokens = success && consuming ? match.TokensConsumed : 0;

            var result = new TokenMatchingResult(success, consumedTokens);

            if (name != null && success)
                result.AddNamedMatch(name, output);
            result.AppendTag(Tag);

            return result;
        }
    }
}
