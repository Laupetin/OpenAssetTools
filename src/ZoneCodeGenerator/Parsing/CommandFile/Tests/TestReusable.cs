using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestReusable : AbstractTokenTest<ICommandParserState>
    {
        private const string MemberTypeNameToken = "name";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("set"),
            new MatcherLiteral("reusable"),
            new MatcherTypename().WithName(MemberTypeNameToken),
            new MatcherLiteral(";")
        };

        public TestReusable() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var typeName = NextMatch(MemberTypeNameToken);
            var typeNameParts = typeName.Split(new[] { "::" }, StringSplitOptions.None);

            if (state.DataTypeInUse != null &&
                state.GetMembersFromParts(typeNameParts, state.DataTypeInUse, out var memberList))
            {
                // Do nothing
            }
            else if (state.GetTypenameAndMembersFromParts(typeNameParts, out _, out memberList))
            {
                // Do nothing
            }
            else
            {
                throw new TestFailedException($"Could not find type '{typeName}'.");
            }

            if (memberList == null || !memberList.Any())
            {
                throw new TestFailedException("Need to specify a member when trying to set to a string.");
            }

            var lastMember = memberList.Last();

            if (!lastMember.Computations.ContainsNonEmbeddedReference)
            {
                throw new TestFailedException($"Specified member '{lastMember.Member.Name}' is not a pointer reference and therefore cannot be reusable.");
            }

            lastMember.IsReusable = true;
        }
    }
}
