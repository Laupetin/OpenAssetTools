using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestSetBlock : AbstractTokenTest<ICommandParserState>
    {
        private const string TokenTypeName = "name";
        private const string TokenBlockEnumEntry = "block";

        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("block"),
            new MatcherGroupOr(new MatcherGroupAnd(
                    new MatcherTypename().WithName(TokenTypeName),
                    new MatcherName().WithName(TokenBlockEnumEntry)
                ),
                new MatcherName().WithName(TokenBlockEnumEntry)),
            new MatcherLiteral(";")
        };

        public TestSetBlock() : base(matchers)
        {
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var typeName = NextMatch(TokenTypeName);

            StructureInformation typeInfo;
            List<MemberInformation> memberList;

            if (typeName != null)
            {
                var typeNameParts = typeName.Split(new[] { "::" }, StringSplitOptions.None);
                if (state.DataTypeInUse != null &&
                    state.GetMembersFromParts(typeNameParts, state.DataTypeInUse, out memberList))
                {
                    typeInfo = state.DataTypeInUse;
                }
                else if (state.GetTypenameAndMembersFromParts(typeNameParts, out typeInfo, out memberList))
                {
                    // Do nothing
                }
                else
                {
                    throw new TestFailedException($"Could not find type '{typeName}'.");
                }
            }
            else
            {
                typeInfo = state.DataTypeInUse;
                memberList = new List<MemberInformation>();

                if(typeInfo == null)
                    throw new TestFailedException("Must specify a type or member.");
            }

            var blockName = NextMatch(TokenBlockEnumEntry);
            var block = state.FastFileBlocks
                .FirstOrDefault(fastFileBlock => fastFileBlock.Name.Equals(blockName));

            if (block == null)
                throw new TestFailedException($"Could not find fastfile block with name '{blockName}'");

            if (memberList.Any())
            {
                var member = memberList.Last();
                member.Block = block;
            }
            else
            {
                typeInfo.Block = block;
            }
        }
    }
}