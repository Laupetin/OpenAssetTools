using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestGame : AbstractTokenTest<ICommandParserState>
    {
        private const string GameNameToken = "name";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("game"), 
            new MatcherName().WithName(GameNameToken),
            new MatcherLiteral(";")
        };

        public TestGame() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            if (!string.IsNullOrEmpty(state.Game))
            {
                throw new TestFailedException($"Game has already been set with value '{state.Game}'");
            }

            state.Game = NextMatch(GameNameToken);
        }
    }
}
