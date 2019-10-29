using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    abstract class TestWithEvaluation : AbstractTokenTest<ICommandParserState>
    {
        private const string TagOperand = "operand";
        private const string TagOperationType = "operationType";
        private const string TokenOperationType = "operationTypeToken";

        // Evaluation Sub-Tags
        private const string TagEvaluationParenthesis = "evaluationParenthesis";
        private const string TagEvaluationNot = "evaluationNot";
        private const string TagEvaluationOperation = "evaluationOperation";

        // Operand Sub-Tags
        private const string TagOperandNumber = "operandNumber";
        private const string TagOperandTypename = "operandTypename";
        private const string TagOperandArray = "operandArray";
        private const string TokenOperandNumber = "operandNumberToken";
        private const string TokenOperandTypename = "operandTypenameToken";
        private const string TokenOperandArray = "operandArrayToken";

        // Visible to children
        protected const string TagEvaluation = "evaluation";

        // operand ::= <typename> <array>* | <number>
        private static readonly TokenMatcher operand = new MatcherGroupOr(
            new MatcherGroupAnd(
                new MatcherTypename().WithName(TokenOperandTypename),
                new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray().WithName(TokenOperandArray).WithTag(TagOperandArray))
            ).WithTag(TagOperandTypename),
            new MatcherNumber().WithName(TokenOperandNumber).WithTag(TagOperandNumber)
        ).WithTag(TagOperand);

        // operationType ::= + | - | * | / | << | >> | ...
        private static readonly TokenMatcher operationType = new MatcherGroupOr(
            OperationType.Types
                .Select(type => new MatcherLiteral(type.Syntax.ToCharArray().Select(c => c.ToString()).ToArray()).WithName(TokenOperationType))
                .Cast<TokenMatcher>().ToArray()
        ).WithTag(TagOperationType);

        // evaluation ::= ( <evaluation> ) | <operand> [<operationType> <evaluation>]
        private static readonly TokenMatcher evaluation = new MatcherGroupAnd(
            new MatcherGroupOr(
                new MatcherGroupAnd(
                    new MatcherGroupOptional(new MatcherLiteral("!").WithTag(TagEvaluationNot)),
                    new MatcherLiteral("("),
                    new MatcherWithTag(TagEvaluation),
                    new MatcherLiteral(")")
                ).WithTag(TagEvaluationParenthesis),
                new MatcherWithTag(TagOperand)
            ),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherWithTag(TagOperationType),
                new MatcherWithTag(TagEvaluation)
            ).WithTag(TagEvaluationOperation))

        ).WithTag(TagEvaluation);

        protected TestWithEvaluation(TokenMatcher[] matchers) : base(matchers)
        {
            AddTaggedMatcher(operand);
            AddTaggedMatcher(operationType);
            AddTaggedMatcher(evaluation);
        }

        private IEvaluation ProcessOperand(ICommandParserState state)
        {
            var operandTypeTag = NextTag();

            if (operandTypeTag.Equals(TagOperandNumber))
            {
                var numberString = NextMatch(TokenOperandNumber);
                return new OperandStatic(int.Parse(numberString));
            }
            else if(operandTypeTag.Equals(TagOperandTypename))
            {
                var typenameString = NextMatch(TokenOperandTypename);
                var arrayIndexStrings = new List<string>();

                while (PeekTag().Equals(TagOperandArray))
                {
                    NextTag();
                    arrayIndexStrings.Add(NextMatch(TokenOperandArray));
                }

                return new OperandDynamic();
            }
            else
            {
                throw new Exception("Unknown Operand Type");
            }
        }

        private OperationType ProcessOperationType(ICommandParserState state)
        {
            var operationMatch = NextMatch(TokenOperationType);

            return OperationType.Types.First(type => type.Syntax.Equals(operationMatch));
        }

        private IEvaluation ProcessEvaluationInParenthesis(ICommandParserState state)
        {
            if (NextTag().Equals(TagEvaluationNot))
            {
                NextTag();
                return new Operation(ProcessEvaluation(state), new OperandStatic(0), OperationType.OperationEquals);
            }

            return ProcessEvaluation(state);
        }

        protected IEvaluation ProcessEvaluation(ICommandParserState state)
        {
            IEvaluation firstStatementPart;
            switch (NextTag())
            {
                case TagEvaluationParenthesis:
                    firstStatementPart = ProcessEvaluationInParenthesis(state);
                    break;

                case TagOperand:
                    firstStatementPart = ProcessOperand(state);
                    break;

                default:
                    throw new Exception("Invalid followup tag @ Evaluation");
            }

            if (PeekTag() == TagEvaluationOperation)
            {
                NextTag();

                if (NextTag() != TagOperationType)
                    throw new Exception("Expected operationType tag @ Evaluation");

                var type = ProcessOperationType(state);

                if (NextTag() != TagEvaluation)
                    throw new Exception("Expected EvaluationTag @ Evaluation");

                var secondStatementPart = ProcessEvaluation(state);

                return new Operation(firstStatementPart, secondStatementPart, type);
            }

            return firstStatementPart;
        }
    }
}