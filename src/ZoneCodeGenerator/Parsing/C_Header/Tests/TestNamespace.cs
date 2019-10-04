using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestNamespace : AbstractTokenTest<IHeaderParserState>
    {
        private const string NamespaceNameToken = "namespace_name";

        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("namespace"),
            new MatcherName().WithName(NamespaceNameToken),
            new MatcherLiteral("{")
        };

        public TestNamespace() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            state.PushBlock(new BlockNamespace(state, NextMatch(NamespaceNameToken)));
        }
    }
}
