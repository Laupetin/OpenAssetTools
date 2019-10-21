using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestCount : AbstractTokenTest<ICommandParserState>
    {
        private const string CalculationStatementTag = "calculationStatement";
        private const string OperationTag = "operation";
        private const string OperandTag = "operand";

        private const string OperationOfCalculationStatementTag = "operationOfCalculationStatement";

        private const string TypeNameToken = "typeName";
        private const string OperationToken = "operationToken";

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
            new MatcherLiteral("+").WithName(OperationToken),
            new MatcherLiteral("-").WithName(OperationToken),
            new MatcherLiteral("*").WithName(OperationToken),
            new MatcherLiteral("/").WithName(OperationToken),
            new MatcherLiteral("<", "<").WithName(OperationToken),
            new MatcherLiteral(">", ">").WithName(OperationToken)
        ).WithTag(OperationTag);

        // calculationStatement ::= ( <calculationStatement> ) | <operand> [<operation> <calculationStatement>]
        private static readonly TokenMatcher calculationStatement = new MatcherGroupAnd(
            new MatcherGroupOr(
                new MatcherGroupAnd(
                    new MatcherLiteral("("),
                    new MatcherWithTag(CalculationStatementTag),
                    new MatcherLiteral(")")
                ),
                new MatcherWithTag(OperandTag)
            ),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherWithTag(OperationTag),
                new MatcherWithTag(CalculationStatementTag)
            )).WithTag(OperationOfCalculationStatementTag)

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
            switch (NextTag())
            {
                case CalculationStatementTag:
                    ProcessCalculationStatement(state);
                    break;

                default:
                    throw new Exception("Expected first count tag to be a calculation statement");
            }
        }

        private IEvaluation ProcessCalculationStatement(ICommandParserState state)
        {
            IEvaluation firstStatementPart;
            switch (NextTag())
            {
                case CalculationStatementTag:
                    firstStatementPart = ProcessCalculationStatement(state);
                    break;

                case OperandTag:
                    firstStatementPart = ProcessOperand(state);
                    break;

                default:
                    throw new Exception("Invalid followup tag @ CalculationStatement");
            }

            if (PeekTag() == OperationOfCalculationStatementTag)
            {
                NextTag();

                if(NextTag() != OperationTag)
                    throw new Exception("Expected operation tag @ CalculationStatement");

                OperationType operationType = ProcessOperation(state);

                if (NextTag() != CalculationStatementTag)
                    throw new Exception("Expected calculationStatement tag @ CalculationStatement");

                var secondStatementPart = ProcessCalculationStatement(state);

                return new Operation(firstStatementPart, secondStatementPart, operationType);
            }

            return firstStatementPart;
        }

        private IEvaluation ProcessOperand(ICommandParserState state)
        {
            return null;
        }

        private OperationType ProcessOperation(ICommandParserState state)
        {
            var operationToken = NextMatch(OperationToken);

            var result = OperationType.Types.FirstOrDefault(type => type.Syntax.Equals(operationToken));

            if (result == null)
            {
                throw new LoadingException($"Unknown operation '{operationToken}'");
            }

            return result;
        }
    }
}