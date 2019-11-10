using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestScriptString : AbstractTokenTest<ICommandParserState>
    {
        private const string MemberTypeNameToken = "name";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("set"),
            new MatcherLiteral("scriptstring"),
            new MatcherTypename().WithName(MemberTypeNameToken),
            new MatcherLiteral(";")
        };

        public TestScriptString() : base(matchers)
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
                throw new TestFailedException("Need to specify a member when trying to set to a script string.");
            }

            var lastMember = memberList.Last();
            if (!(lastMember.Member.VariableType.Type is DataTypeBaseType))
            {
                throw new TestFailedException($"Specified member '{lastMember.Member.Name}' is not a base type and therefore cannot be a script string.");
            }

            lastMember.IsScriptString = true;
        }
    }
}
