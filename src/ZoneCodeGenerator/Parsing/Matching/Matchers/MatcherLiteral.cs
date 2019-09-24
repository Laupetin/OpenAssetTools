namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherLiteral : BaseMatcher
    { 
        private readonly string literal;

        public MatcherLiteral(string literal)
        {
            this.literal = literal;
        }

        protected override MatchingResult Matches(MatchingContext context, int tokenOffset)
        {
            var token = context.Lexer.PeekToken(tokenOffset);
            var isMatch = string.Equals(token, literal);

            if (!isMatch) return new MatchingResult(false, 0);

            SetMatcherOutput(literal);
            return new MatchingResult(true, 1);
        }

        protected override string GetIdentifier()
        {
            return $"Literal(\"{literal}\")";
        }
    }
}
