using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestString : AbstractTokenTest<ICommandParserState>
    {
        private const string MemberTypeNameToken = "name";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("set"),
            new MatcherLiteral("string"),
            new MatcherTypename().WithName(MemberTypeNameToken),
            new MatcherLiteral(";")
        };

        public TestString() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {

        }
    }
}
