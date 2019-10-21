using System.Text;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherLiteral : BaseMatcher
    { 
        private readonly string[] literals;

        public MatcherLiteral(params string[] literals)
        {
            this.literals = literals;
        }

        protected override MatchingResult Matches(MatchingContext context, int tokenOffset)
        {
            var matcherOutputBuilder = new StringBuilder();

            foreach (var literal in literals)
            {
                var token = context.Lexer.PeekToken(tokenOffset);
                var isMatch = string.Equals(token, literal);

                if (!isMatch) return new MatchingResult(false, 0);

                matcherOutputBuilder.Append(literal);
            }

            SetMatcherOutput(matcherOutputBuilder.ToString());
            return new MatchingResult(true, literals.Length);
        }

        protected override string GetIdentifier()
        {
            return $"Literal(\"{string.Join("", literals)}\")";
        }
    }
}
