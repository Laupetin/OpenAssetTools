using System.Linq;
using System.Text.RegularExpressions;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherName : BaseMatcher
    {
        private static readonly Regex nameRegex = new Regex(@"^[a-zA-Z_$][a-zA-Z0-9_$]*$");

        private static readonly string[] reservedWords = {
            "const",
            "int",
            "char",
            "short",
            "long",
            "unsigned",
            "typedef",
            "void",
            "static",
            "class",
            "struct",
            "enum",
            "union",
            "namespace",
            "using",
            "public",
            "protected",
            "private",
            "alignas",
            "and",
            "auto",
            "bool",
            "catch",
            "concept",
            "default",
            "double",
            "float",
            "extern",
            "for",
            "if",
            "mutable",
            "noexcept",
            "nullptr",
            "switch",
            "this",
            "true",
            "false",
            "typeid",
            "while",
            "alignof",
            "break",
            "continue",
            "dynamic_cast",
            "static_cast",
            "explicit",
            "friend",
            "not",
            "operator",
            "return",
            "sizeof",
            "thread_local",
            "try",
            "typename",
            "volatile",
            "asm",
            "case",
            "constexpr",
            "decltype",
            "do",
            "else",
            "export",
            "goto",
            "inline",
            "new",
            "reinterpret_cast",
            "static",
            "template",
            "throw",
            "virtual"
        };

        protected override MatchingResult Matches(MatchingContext context, int tokenOffset)
        {
            var token = context.Lexer.PeekToken(tokenOffset);
            var isMatch = nameRegex.IsMatch(token) && !reservedWords.Contains(token);

            if (!isMatch) return new MatchingResult(false, 0);

            SetMatcherOutput(token);
            return new MatchingResult(true, 1);
        }

        protected override string GetIdentifier()
        {
            return "Name";
        }
    }
}
