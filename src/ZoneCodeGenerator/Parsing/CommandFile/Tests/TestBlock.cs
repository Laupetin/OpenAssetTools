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
        private const string BlockNumberToken = "num";
        private const string BlockTypeToken = "type";
        private const string BlockNameToken = "name";
        private const string DefaultToken = "default";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("block"), 
            new MatcherNumber().WithName(BlockNumberToken),
            new MatcherName().WithName(BlockTypeToken),
            new MatcherName().WithName(BlockNameToken),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple,new MatcherGroupOr(
                new MatcherLiteral("default").WithName(DefaultToken)
            )), 
            new MatcherLiteral(";")
        };

        public TestBlock() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var blockName = GetMatcherTokens(BlockNameToken)[0];
            var blockNumber = int.Parse(GetMatcherTokens(BlockNumberToken)[0]);

            var blockTypeInput = GetMatcherTokens(BlockTypeToken)[0];
            if (!Enum.TryParse(blockTypeInput, true, out FastFileBlock.Type blockType))
            {
                var blockTypeValues = Enum.GetValues(typeof(FastFileBlock.Type)).OfType<FastFileBlock.Type>()
                    .Select(type => type.ToString());
                
                throw new TestFailedException($"Unknown fastfile block type '{blockTypeInput}'. Must be one of the following: {string.Join(", ", blockTypeValues)}");
            }

            var block = new FastFileBlock(blockName, blockNumber, blockType, HasMatcherTokens(DefaultToken));

            state.FastFileBlocks.Add(block);
        }
    }
}
