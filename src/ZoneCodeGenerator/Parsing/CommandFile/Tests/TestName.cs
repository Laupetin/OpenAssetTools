using System;
using System.Linq;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestName : AbstractTokenTest<ICommandParserState>
    {
        private const string MemberTypeNameToken = "name";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("set"),
            new MatcherLiteral("name"),
            new MatcherTypename().WithName(MemberTypeNameToken),
            new MatcherLiteral(";")
        };

        public TestName() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var typeName = NextMatch(MemberTypeNameToken);
            var typeNameParts = typeName.Split(new[] { "::" }, StringSplitOptions.None);
            StructureInformation structure;

            if (state.DataTypeInUse != null &&
                state.GetMembersFromParts(typeNameParts, state.DataTypeInUse, out var memberList))
            {
                structure = state.DataTypeInUse;
            }
            else if (state.GetTypenameAndMembersFromParts(typeNameParts, out structure, out memberList))
            {
                // Do nothing
            }
            else
            {
                throw new TestFailedException($"Could not find type '{typeName}'.");
            }

            if (memberList == null || !memberList.Any())
            {
                throw new TestFailedException("Need to specify a member when trying to set to a structure name.");
            }

            for (var i = 0; i < memberList.Count - 1; i++)
            {
                if (memberList[i].Computations.ContainsNonEmbeddedReference)
                {
                    throw new TestFailedException("Can only add embedded members to name chain.");
                }
            }

            if (!memberList[memberList.Count - 1].IsString)
            {
                throw new TestFailedException("Final name member must be a string.");
            }

            structure.NameChain = memberList;
        }
    }
}
