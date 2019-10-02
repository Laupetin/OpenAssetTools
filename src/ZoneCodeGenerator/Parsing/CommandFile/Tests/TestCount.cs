using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestCount : AbstractTokenTest<ICommandParserState>
    {
        private const string TypeNameToken = "typeName";
        private const string CalculationStatementTag = "calculationStatement";
        private const string OperationTag = "operation";
        private const string OperandTag = "operand";

        // operand ::= <typename> <array>* | <number>
        private static readonly TokenMatcher operand = new MatcherGroupOr(
            new MatcherGroupAnd(
                new MatcherTypename(),
                new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray())
            ),
            new MatcherNumber()
        ).WithTag(OperandTag);

        // operation ::= + | - | * | / | << | >>
        private static readonly TokenMatcher operation = new MatcherGroupOr(
            new MatcherLiteral("+"),
            new MatcherLiteral("-"),
            new MatcherLiteral("*"),
            new MatcherLiteral("/"),
            new MatcherGroupAnd(new MatcherLiteral("<"), new MatcherLiteral("<")),
            new MatcherGroupAnd(new MatcherLiteral(">"), new MatcherLiteral(">"))
        ).WithTag(OperationTag);

        // calculationStatement ::= ( <calculationStatement> ) | <operand> [<operation> <calculationStatement>]
        private static readonly TokenMatcher calculationStatement = new MatcherGroupOr(
            new MatcherGroupAnd(
                new MatcherLiteral("("),
                new MatcherWithTag(CalculationStatementTag),
                new MatcherLiteral(")")
            ),
            new MatcherGroupAnd(
                new MatcherWithTag(OperandTag),
                new MatcherGroupOptional(new MatcherGroupAnd(
                    new MatcherWithTag(OperationTag),
                    new MatcherWithTag(CalculationStatementTag)
                ))
            )
        ).WithTag(CalculationStatementTag);

        // set count <typename> <calculationStatement>;
        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("count"),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherWithTag(CalculationStatementTag),
            new MatcherLiteral(";")
        };

        public TestCount() : base(matchers)
        {
            AddTaggedMatcher(operand);
            AddTaggedMatcher(operation);
            AddTaggedMatcher(calculationStatement);
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
        }
    }
}