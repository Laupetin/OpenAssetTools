using System;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestCondition : TestWithEvaluation
    {
        private const string TagAlways = "always";
        private const string TagNever = "never";
        private const string TypeNameToken = "typeName";

        // set condition <typename> (always | never | <CalculationStatementTag>);
        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("condition"),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherGroupOr(
                new MatcherLiteral("always").WithTag(TagAlways),
                new MatcherLiteral("never").WithTag(TagNever),
                new MatcherWithTag(TagEvaluation)
            ),
            new MatcherLiteral(";")
        };

        public TestCondition() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            IEvaluation evaluation;
            switch (NextTag())
            {
                case TagAlways:
                    evaluation = new OperandStatic(1);
                    break;

                case TagNever:
                    evaluation = new OperandStatic(0);
                    break;

                case TagEvaluation:
                    evaluation = ProcessEvaluation(state);
                    break;

                default:
                    throw new Exception("Unexpected Tag in TestCondition");
            }


        }
    }
}