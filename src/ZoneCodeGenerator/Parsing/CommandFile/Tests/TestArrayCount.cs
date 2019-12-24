using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestArrayCount : TestWithEvaluation
    {
        private StructureInformation referencedType;
        private const string TypeNameToken = "typeName";

        // set count <typename> <calculationStatement>;
        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("arraycount"),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherWithTag(TagEvaluation),
            new MatcherLiteral(";")
        };

        public TestArrayCount() : base(matchers)
        {
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var typeName = NextMatch(TypeNameToken);
            var typeNameParts = typeName.Split(new[] { "::" }, StringSplitOptions.None);
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
                throw new TestFailedException("Can only set array size for members and not for types.");
            }

            if (!NextTag().Equals(TagEvaluation))
                throw new Exception("Expected first array size tag to be a calculation statement");

            var evaluation = ProcessEvaluation(state);

            var referencedMember = typeMembers.Last();
            var reference = referencedMember.Member.VariableType.References.OfType<ReferenceTypeArray>()
                .FirstOrDefault();

            if (reference != null)
                reference.DynamicCount = evaluation;
            else
                throw new TestFailedException(
                    $"Member '{referencedMember.Member.Name}' of type '{referencedMember.Member.VariableType.Type.FullName}' cannot have its array count set due to it not having an array reference");
        }

        protected override IEnumerable<StructureInformation> GetUsedTypes(ICommandParserState state)
        {
            if (state.DataTypeInUse != null
                && state.DataTypeInUse != referencedType)
            {
                return new[] { state.DataTypeInUse, referencedType };
            }

            return new[] { referencedType };
        }
    }
}