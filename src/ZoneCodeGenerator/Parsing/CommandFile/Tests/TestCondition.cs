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
        private const string ConditionTag = "condition";
        private const string ConditionRelationTag = "conditionRelation";
        private const string OperationTag = "operation";
        private const string OperandTag = "operand";
        private const string ConditionChainLinkTag = "conditionChainLink";
        private const string ConditionChainTag = "conditionChain";

        // operand ::= <typename> <array>+ | <number> | true | false
        private static readonly TokenMatcher operand = new MatcherGroupOr(
            new MatcherGroupAnd(
                new MatcherTypename(),
                new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray())
            ),
            new MatcherNumber(),
            new MatcherLiteral("true"),
            new MatcherLiteral("false")
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

        // conditionStatement ::= ( <conditionStatement> ) | <operand> [<operation> <operand>]
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
                    new MatcherWithTag(ConditionStatementTag)
                ))
            )
        ).WithTag(ConditionStatementTag);

        // conditionChainLink ::= == | != | <= | >= | < | >
        private static readonly TokenMatcher conditionRelation = new MatcherGroupOr(
            new MatcherGroupAnd(new MatcherLiteral("="), new MatcherLiteral("=")),
            new MatcherGroupAnd(new MatcherLiteral("!"), new MatcherLiteral("=")),
            new MatcherGroupAnd(new MatcherLiteral("<"), new MatcherLiteral("=")),
            new MatcherGroupAnd(new MatcherLiteral(">"), new MatcherLiteral("=")),
            new MatcherLiteral("<"),
            new MatcherLiteral(">")
        ).WithTag(ConditionRelationTag);

        // condition ::= <conditionStatement> <conditionRelation> <conditionStatement>
        private static readonly TokenMatcher condition = new MatcherGroupAnd(
            new MatcherWithTag(ConditionStatementTag),
            new MatcherWithTag(ConditionRelationTag),
            new MatcherWithTag(ConditionStatementTag)
        ).WithTag(ConditionTag);

        // conditionChainLink ::= && | ||
        private static readonly TokenMatcher conditionChainLink = new MatcherGroupOr(
            new MatcherGroupAnd(new MatcherLiteral("&"), new MatcherLiteral("&")),
            new MatcherGroupAnd(new MatcherLiteral("|"), new MatcherLiteral("|"))
        ).WithTag(ConditionChainLinkTag);

        // conditionChain ::= <condition> (<conditionChainLink> <condition>)*
        private static readonly TokenMatcher conditionChain = new MatcherGroupAnd(
            new MatcherWithTag(ConditionTag),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherGroupAnd(
                new MatcherWithTag(ConditionChainLinkTag),
                new MatcherWithTag(ConditionTag)
            ))
        ).WithTag(ConditionChainTag);

        // set condition <typename> (always | never | <conditionChain>);
        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("condition"),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherGroupOr(
                new MatcherLiteral("always"),
                new MatcherLiteral("never"),
                new MatcherWithTag(ConditionChainTag)
            ),
            new MatcherLiteral(";")
        };

        public TestCondition() : base(matchers)
        {
            AddTaggedMatcher(operand);
            AddTaggedMatcher(operation);
            AddTaggedMatcher(conditionStatement);
            AddTaggedMatcher(conditionRelation);
            AddTaggedMatcher(condition);
            AddTaggedMatcher(conditionChainLink);
            AddTaggedMatcher(conditionChain);
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
        }
    }
}