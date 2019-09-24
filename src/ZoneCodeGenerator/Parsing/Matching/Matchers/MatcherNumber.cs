using System.Globalization;
using System.Text.RegularExpressions;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherNumber : BaseMatcher
    {
        private static readonly Regex decimalNumber = new Regex(@"^\d+$");
        private static readonly Regex hexNumber = new Regex(@"^0x([0-9a-fA-F]+)$");

        protected override MatchingResult Matches(MatchingContext context, int tokenOffset)
        {
            var token = context.Lexer.PeekToken(tokenOffset);

            var currentMatch = decimalNumber.Match(token);

            if (currentMatch.Success)
            {
                SetMatcherOutput(token);
                return new MatchingResult(true, 1);
            }

            currentMatch = hexNumber.Match(token);

            if (!currentMatch.Success) return new MatchingResult(false, 0);

            SetMatcherOutput(int.Parse( currentMatch.Groups[1].Value, NumberStyles.HexNumber).ToString());
            return new MatchingResult(true, 1);
        }

        protected override string GetIdentifier()
        {
            return "Number";
        }
    }
}
