using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestCloseBlock : AbstractTokenTest<IHeaderParserState>
    {
        private const string NameToken = "close_with_name";
        private const string SemicolonToken = "close_with_semi_colon";

        private readonly bool semicolonRequired;

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("}"),
            new MatcherGroupOptional( new MatcherGroupAnd(
                new MatcherGroupOptional(new MatcherName().WithName(NameToken)),
                new MatcherLiteral(";").WithName(SemicolonToken)
            ))
        };

        public TestCloseBlock(bool semicolonRequired = false) : base(matchers)
        {
            this.semicolonRequired = semicolonRequired;
        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            if (HasMatcherTokens(NameToken))
            {
                if (state.CurrentBlock is INameAssignable nameAssignableBlock)
                {
                    nameAssignableBlock.AssignName(GetMatcherTokens(NameToken)[0]);
                }
                else
                {
                    throw new TestFailedException($"Cannot give block '{state.CurrentBlock.Type}' a name when closing");
                }
            }

            if (!HasMatcherTokens(SemicolonToken) && semicolonRequired)
                throw new TestFailedException($"Block '{state.CurrentBlock.Type}' needs to be closed with a '{'}'};'");

            state.PopBlock();
        }
    }
}
