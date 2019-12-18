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
    class TestCount : TestWithEvaluation
    {
        private StructureInformation referencedType;
        private const string TokenTypeName = "typeName";
        private const string TokenPointerResolve = "pointerResolve";
        private const string TokenArrayIndex = "arrayIndex";

        // set count <typename> <calculationStatement>;
        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("count"),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple,
                new MatcherLiteral("*").WithName(TokenPointerResolve)),
            new MatcherTypename().WithName(TokenTypeName),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple,
                new MatcherArray().WithName(TokenArrayIndex)),
            new MatcherWithTag(TagEvaluation),
            new MatcherLiteral(";")
        };

        public TestCount() : base(matchers)
        {
        }

        private void SetCountByArrayIndex(MemberInformation referencedMember, ReferenceTypePointer pointer,
            IEvaluation evaluation)
        {
            if (pointer.CountByArrayIndex == null)
            {
                pointer.CountByArrayIndex = new IEvaluation[referencedMember.Member.VariableType.References
                    .OfType<ReferenceTypeArray>().Select(array => array.ArraySize).Aggregate((i1, i2) => i1 * i2)];
            }

            var arraySizes = referencedMember.Member.VariableType.References
                .OfType<ReferenceTypeArray>()
                .Select(array => array.ArraySize)
                .ToList();
            var index = 0;
            var indexDepth = 0;
            string s;
            while ((s = NextMatch(TokenArrayIndex)) != null)
            {
                var specifiedIndex = int.Parse(s);

                if (specifiedIndex >= arraySizes[indexDepth])
                {
                    throw new TestFailedException($"Specified index '{specifiedIndex}' is out of bounds at type '{referencedMember.Parent.Type.FullName}::{referencedMember.Member.Name}'");
                }

                var sizePerIndexInCurrentDepth = arraySizes.Count <= indexDepth + 1 ?
                    1 : arraySizes.Skip(indexDepth + 1).Aggregate((i1, i2) => i1 * i2);

                index += specifiedIndex * sizePerIndexInCurrentDepth;
                indexDepth++;
            }

            pointer.CountByArrayIndex[index] = evaluation;
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var typeName = NextMatch(TokenTypeName);
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
                throw new TestFailedException("Can only set count for members and not for types.");
            }

            if (!NextTag().Equals(TagEvaluation))
                throw new Exception("Expected first count tag to be a calculation statement");

            var evaluation = ProcessEvaluation(state);

            var referencedMember = typeMembers.Last();
            var reference = referencedMember.Member.VariableType.References
                .OfType<ReferenceTypePointer>()
                .Skip(GetMatcherTokenCount(TokenPointerResolve))
                .FirstOrDefault();

            if (reference != null)
            {
                if (HasMatcherTokens(TokenArrayIndex))
                {
                    SetCountByArrayIndex(referencedMember, reference, evaluation);
                }
                else
                {
                    reference.Count = evaluation;
                }
            }
            else
            {
                throw new TestFailedException(
                    $"Member '{referencedMember.Member.Name}' of type '{referencedMember.Member.VariableType.Type.FullName}' cannot have its count set due to it not having a pointer reference");
            }
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