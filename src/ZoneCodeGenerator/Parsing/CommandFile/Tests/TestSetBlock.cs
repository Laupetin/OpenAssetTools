using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
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
            new MatcherTypename().WithName(TokenTypeName),
            new MatcherName().WithName(TokenBlockEnumEntry),
            new MatcherLiteral(";")
        };

        public TestSetBlock() : base(matchers)
        {
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var typeName = NextMatch(TokenTypeName);
            var typeNameParts = typeName.Split(new[] {"::"}, StringSplitOptions.None);

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

            if (!memberList.Any())
            {
                throw new TestFailedException("Must specify a member and not a type when setting a block.");
            }

            var member = memberList.Last();

            var blockName = NextMatch(TokenBlockEnumEntry);
            var block = state.FastFileBlocks
                .FirstOrDefault(fastFileBlock => fastFileBlock.Name.Equals(blockName));

            member.Block =
                block ?? throw new TestFailedException($"Could not find fastfile block with name '{blockName}'");
        }
    }
}