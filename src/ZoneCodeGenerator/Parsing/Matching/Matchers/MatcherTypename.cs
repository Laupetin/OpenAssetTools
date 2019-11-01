using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherTypename : BaseMatcher
    {
        private static readonly Regex nameRegex = new Regex(@"^[a-zA-Z_$][a-zA-Z0-9_$]*$");

        private int MatchTypenameExtension(MatchingContext context, int tokenOffset)
        {
            if (!":".Equals(context.Lexer.PeekToken(tokenOffset++)))
                return 0;
            if (!":".Equals(context.Lexer.PeekToken(tokenOffset++)))
                return 0;
            
            return nameRegex.IsMatch(context.Lexer.PeekToken(tokenOffset)) ? 3 : 0;
        }

        protected override MatchingResult Matches(MatchingContext context, int tokenOffset)
        {
            var lexer = context.Lexer;
            var typename = new StringBuilder();
            var startTokenOffset = tokenOffset;

            var currentPart = lexer.PeekToken(tokenOffset++);
            if ("unsigned".Equals(currentPart)
                || "signed".Equals(currentPart))
            {
                typename.Append(currentPart);
                typename.Append(' ');
                currentPart = lexer.PeekToken(tokenOffset++);
            }

            if (!nameRegex.IsMatch(currentPart))
                return new MatchingResult(false, 0);
            typename.Append(currentPart);

            if ("long".Equals(currentPart))
            {
                currentPart = lexer.PeekToken(tokenOffset);
                if (currentPart.Equals("long"))
                {
                    tokenOffset++;
                    typename.Append(' ');
                    typename.Append(currentPart);
                }
            }
            else
            {
                var extensionLength = MatchTypenameExtension(context, tokenOffset);
                while (extensionLength > 0)
                {
                    for (var i = 0; i < extensionLength; i++)
                    {
                        typename.Append(lexer.PeekToken(tokenOffset + i));
                    }

                    tokenOffset += extensionLength;
                    extensionLength = MatchTypenameExtension(context, tokenOffset);
                }
            }

            SetMatcherOutput(typename.ToString());
            return new MatchingResult(true, tokenOffset - startTokenOffset);
        }

        protected override string GetIdentifier()
        {
            return "Typename";
        }
    }
}
