using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ZoneCodeGenerator.Parsing.Matching
{
    class MatchingContext
    {
        public ILexer Lexer { get; }

        public bool Verbose { get; set; }

        private readonly Dictionary<string, TokenMatcher> taggedMatchers;

        public MatchingContext(ILexer lexer, Dictionary<string, TokenMatcher> taggedMatchers)
        {
            Lexer = lexer;
            this.taggedMatchers = taggedMatchers;
            Verbose = false;
        }

        public TokenMatcher GetMatcherByTag(string tag)
        {
            return taggedMatchers.ContainsKey(tag) ? taggedMatchers[tag] : null;
        }
    }
}
