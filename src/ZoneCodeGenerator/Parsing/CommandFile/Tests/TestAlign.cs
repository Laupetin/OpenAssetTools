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
    class TestAlign : AbstractTokenTest<ICommandParserState>
    {
        private const string MemberTypeNameToken = "name";
        private const string AlignValueToken = "align";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("set"),
            new MatcherLiteral("align"),
            new MatcherTypename().WithName(MemberTypeNameToken),
            new MatcherNumber().WithName(AlignValueToken), 
            new MatcherLiteral(";")
        };

        public TestAlign() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {

        }
    }
}
