using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestCondition : TestWithEvaluation
    {
        private StructureInformation referencedType;

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
            var typeName = NextMatch(TypeNameToken);
            var typeNameParts = typeName.Split(new[] {"::"}, StringSplitOptions.None);

            if (state.DataTypeInUse != null
                && state.GetMembersFromParts(typeNameParts, state.DataTypeInUse, out var typeMembers))
            {
                referencedType = state.DataTypeInUse;
            }
            else if (state.GetTypenameAndMembersFromParts(typeNameParts, out referencedType, out typeMembers))
            {
                // Do nothing
            }
            else
            {
                throw new TestFailedException($"Could not find type/members '{typeName}'");
            }

            if (typeMembers == null
                || !typeMembers.Any())
            {
                throw new TestFailedException("Can only set conditions for members and not for types.");
            }

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

            var referencedMember = typeMembers.Last();
            referencedMember.Condition = evaluation;
        }

        protected override IEnumerable<StructureInformation> GetUsedTypes(ICommandParserState state)
        {
            if (state.DataTypeInUse != null
                && state.DataTypeInUse != referencedType)
            {
                return new[] {state.DataTypeInUse, referencedType};
            }

            return new[] {referencedType};
        }
    }
}