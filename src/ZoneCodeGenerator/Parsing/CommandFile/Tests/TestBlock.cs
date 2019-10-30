using System;
using System.Linq;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestBlock : AbstractTokenTest<ICommandParserState>
    {
        private const string BlockTypeToken = "type";
        private const string BlockEnumMemberToken = "enumEntry";
        private const string DefaultToken = "default";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("block"),
            new MatcherName().WithName(BlockTypeToken),
            new MatcherName().WithName(BlockEnumMemberToken),
            new MatcherGroupOptional(
                new MatcherLiteral("default").WithName(DefaultToken)
                ),
            new MatcherLiteral(";")
        };

        public TestBlock() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var blockEnumEntryName = NextMatch(BlockEnumMemberToken);
            var blockEnumEntry = state.Repository.GetAllEnums()
                .SelectMany(_enum => _enum.Members)
                .FirstOrDefault(member => member.Name.Equals(blockEnumEntryName));
            if (blockEnumEntry == null)
            {
                throw new TestFailedException($"Could not find enum entry '{blockEnumEntryName}' for block.");
            }

            var blockTypeInput = NextMatch(BlockTypeToken);
            if (!Enum.TryParse(blockTypeInput, true, out FastFileBlock.Type blockType))
            {
                var blockTypeValues = Enum.GetValues(typeof(FastFileBlock.Type)).OfType<FastFileBlock.Type>()
                    .Select(type => type.ToString());
                
                throw new TestFailedException($"Unknown fastfile block type '{blockTypeInput}'. Must be one of the following: {string.Join(", ", blockTypeValues)}");
            }

            var block = new FastFileBlock(blockEnumEntry.Name, blockEnumEntry.Value, blockType, HasMatcherTokens(DefaultToken));

            state.FastFileBlocks.Add(block);
        }
    }
}
