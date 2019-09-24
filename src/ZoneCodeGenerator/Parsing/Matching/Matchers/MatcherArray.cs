using System.Globalization;
using System.Text.RegularExpressions;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherArray : BaseMatcher
    {
        private static readonly Regex decimalNumberRegex = new Regex(@"^[0-9]+$");
        private static readonly Regex hexNumberRegex = new Regex(@"^0x([0-9a-fA-F]+)$");
        private static readonly Regex nameRegex = new Regex(@"^[a-zA-Z_$][a-zA-Z0-9_$]*$");

        protected override MatchingResult Matches(MatchingContext context, int tokenOffset)
        {
            var lexer = context.Lexer;

            if (lexer.PeekToken(tokenOffset++) != "[")
                return new MatchingResult(false, 0);

            var token = lexer.PeekToken(tokenOffset++);

            if (decimalNumberRegex.IsMatch(token))
            {
                SetMatcherOutput(token);
            }
            else if (hexNumberRegex.IsMatch(token))
            {
                SetMatcherOutput(int.Parse(hexNumberRegex.Match(token).Groups[1].Value, NumberStyles.HexNumber).ToString());
            }
            else if (nameRegex.IsMatch(token))
            {
                SetMatcherOutput(token);
            }
            else
            {
                return new MatchingResult(false, 0);
            }

            return lexer.PeekToken(tokenOffset) != "]" ? new MatchingResult(false, 0) 
                : new MatchingResult(true, 3);
        }

        protected override string GetIdentifier()
        {
            return "Array";
        }
    }
}
