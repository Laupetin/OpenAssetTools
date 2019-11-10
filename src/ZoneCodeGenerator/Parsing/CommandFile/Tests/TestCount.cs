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
    class TestCount : TestWithEvaluation
    {
        private StructureInformation referencedType;
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
                throw new TestFailedException("Can only set count for members and not for types.");
            }

            if (!NextTag().Equals(TagEvaluation))
                throw new Exception("Expected first count tag to be a calculation statement");

            var evaluation = ProcessEvaluation(state);

            var referencedMember = typeMembers.Last();
            var reference = referencedMember.Member.VariableType.References.OfType<ReferenceTypePointer>()
                .LastOrDefault();

            if (reference != null)
                reference.Count = evaluation;
            else
                throw new TestFailedException(
                    $"Member '{referencedMember.Member.Name}' of type '{referencedMember.Member.VariableType.Type.FullName}' cannot have its count set due to it not having a pointer reference");
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