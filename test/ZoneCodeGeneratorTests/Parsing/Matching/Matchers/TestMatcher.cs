using ZoneCodeGenerator.Parsing.Matching;

namespace ZoneCodeGeneratorTests.Parsing.Matching.Matchers
{
    class TestMatcher : TokenMatcher
    {
        private readonly int tokenCount;
        private readonly string tokenName;

        public bool Successful { get; set; }
        public int TestTokenOffset { get; private set; }
        public bool WasTested { get; private set; }

        public TestMatcher(bool successful, int tokenCount, string tokenName = "")
        {
            Successful = successful;
            this.tokenCount = tokenCount;
            this.tokenName = tokenName;
            TestTokenOffset = 0;
            WasTested = false;
        }

        protected override string GetIdentifier()
        {
            return "TestMatcher";
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            var result = new TokenMatchingResult(Successful, Successful ? tokenCount : 0);

            if(result.Successful)
                result.AppendTag(Tag);

            if(!string.IsNullOrEmpty(tokenName))
                result.AddNamedMatch(tokenName, "test");

            TestTokenOffset = tokenOffset;
            WasTested = true;

            return result;
        }
    }
}
