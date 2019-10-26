using System;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestCount : TestWithEvaluation
    {
        private const string TypeNameToken = "typeName";

        // set count <typename> <calculationStatement>;
        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("count"),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherWithTag(TagEvaluation),
            new MatcherLiteral(";")
        };

        public TestCount() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            if (!NextTag().Equals(TagEvaluation))
            {
                throw new Exception("Expected first count tag to be a calculation statement");
            }

            var evaluation = ProcessEvaluation(state);
        }
    }
}