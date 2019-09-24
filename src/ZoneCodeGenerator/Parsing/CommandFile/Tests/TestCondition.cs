using System;
using System.Collections.Generic;
using System.Text;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestCondition : AbstractTokenTest<ICommandParserState>
    {
        private const string TypeNameToken = "typeName";
        private const string ConditionStatementTag = "conditionStatement";
        private const string ConditionChainTag = "conditionChain";
        private const string ConditionChainLinkTag = "conditionChainLink";
        private const string OperationTag = "operation";
        private const string OperandTag = "operand";

        private static readonly TokenMatcher operand = new MatcherGroupOr(
                new MatcherGroupAnd(
                    new MatcherTypename(),
                    new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray())
                    ),
                new MatcherNumber(),
                new MatcherLiteral("true"),
                new MatcherLiteral("false")
            ).WithTag(OperandTag);

        private static readonly TokenMatcher operation = new MatcherGroupOr(
                new MatcherLiteral("+"),
                new MatcherLiteral("-"),
                new MatcherLiteral("*"),
                new MatcherLiteral("/"),
                new MatcherGroupAnd(new MatcherLiteral("<"), new MatcherLiteral("<")),
                new MatcherGroupAnd(new MatcherLiteral(">"), new MatcherLiteral(">"))
            ).WithTag(OperationTag);

        private static readonly TokenMatcher conditionStatement = new MatcherGroupOr(
            new MatcherGroupAnd(
                new MatcherLiteral("("),
                new MatcherWithTag(ConditionStatementTag),
                new MatcherLiteral(")")
                ),
            new MatcherGroupAnd(
                new MatcherWithTag(OperandTag),
                new MatcherGroupOptional(new MatcherGroupAnd(
                    new MatcherWithTag(OperationTag),
                    new MatcherWithTag(OperandTag)
                    ))
                )
            ).WithTag(ConditionStatementTag);

        private static readonly TokenMatcher conditionChainLink = new MatcherGroupOr(
                new MatcherGroupAnd(new MatcherLiteral("="), new MatcherLiteral("=")),
                new MatcherGroupAnd(new MatcherLiteral("!"), new MatcherLiteral("=")),
                new MatcherGroupAnd(new MatcherLiteral("<"), new MatcherLiteral("=")),
                new MatcherGroupAnd(new MatcherLiteral(">"), new MatcherLiteral("=")),
                new MatcherLiteral("<"),
                new MatcherLiteral(">")
            ).WithTag(ConditionChainLinkTag);

        private static readonly TokenMatcher conditionChain = new MatcherGroupAnd(
            new MatcherWithTag(ConditionStatementTag),
                new MatcherWithTag(ConditionChainLinkTag),
                new MatcherWithTag(ConditionStatementTag)
            ).WithTag(ConditionChainTag);

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("set"),
            new MatcherLiteral("condition"),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherGroupOr(
                new MatcherLiteral("never"),
                new MatcherWithTag(ConditionChainTag)
                ),
            new MatcherLiteral(";")
        };

        public TestCondition() : base(matchers)
        {
            AddTaggedMatcher(conditionChain);
            AddTaggedMatcher(conditionChainLink);
            AddTaggedMatcher(conditionStatement);
            AddTaggedMatcher(operation);
            AddTaggedMatcher(operand);
        }

        protected override void ProcessMatch(ICommandParserState state)
        {

        }
    }
}
