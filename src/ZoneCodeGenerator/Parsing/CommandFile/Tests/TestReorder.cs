using System;
using System.Collections.Generic;
using System.Text;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestReorder : AbstractTokenTest<ICommandParserState>
    {
        private const string TypeNameToken = "typeName";
        private const string ReorderMemberNameToken = "member";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("reorder"),
            new MatcherGroupOptional(new MatcherTypename().WithName(TypeNameToken)), 
            new MatcherLiteral(":"),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, new MatcherName().WithName(ReorderMemberNameToken)),
            new MatcherLiteral(";")
        };

        public TestReorder() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {

        }
    }
}
