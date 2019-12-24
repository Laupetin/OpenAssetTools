using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Domain.Information;

namespace ZoneCodeGenerator.Generating.Computations
{
    class MemberReferenceComputations
    {
        private readonly MemberInformation information;
        private readonly List<int> referenceIndices;
        private readonly int combinedIndex;

        public ReferenceType Reference => referenceIndices.Count < information.Member.VariableType.References.Count
            ? information.Member.VariableType.References[referenceIndices.Count]
            : null;

        public ReferenceType NextReference =>
            referenceIndices.Count + 1 < information.Member.VariableType.References.Count
                ? information.Member.VariableType.References[referenceIndices.Count + 1]
                : null;

        public IEnumerable<ReferenceType> FollowingReferences =>
            information.Member.VariableType.References.Skip(referenceIndices.Count + 1);

        public IEnumerable<int> ArrayIndices => referenceIndices;

        public bool IsArray => Reference is ReferenceTypeArray;
        public int ArraySize => Reference is ReferenceTypeArray array ? array.ArraySize : 0;

        public bool HasDynamicArrayCount => Reference is ReferenceTypeArray array && array.DynamicCount != null;

        public IEvaluation DynamicArrayCountEvaluation =>
            Reference is ReferenceTypeArray array ? array.DynamicCount : null;

        public IEnumerable<MemberReferenceComputations> ArrayEntries => Enumerable.Range(0, ArraySize)
            .Select(i => new MemberReferenceComputations(information, referenceIndices.Concat(new[] {i})));

        public bool IsSinglePointer => Reference is ReferenceTypePointer referenceTypePointer
                                       && !referenceTypePointer.IsArray(combinedIndex)
                                       && !FollowingReferences.OfType<ReferenceTypePointer>().Any();

        public bool IsArrayPointer => Reference is ReferenceTypePointer referenceTypePointer
                                      && referenceTypePointer.IsArray(combinedIndex)
                                      && !FollowingReferences.OfType<ReferenceTypePointer>().Any();

        public IEvaluation ArrayPointerCountEvaluation
        {
            get
            {
                if (!(Reference is ReferenceTypePointer pointer))
                    return null;

                return pointer.HasCountByArrayIndex ? pointer.CountByArrayIndex[combinedIndex] : pointer.Count;
            }
        }

        public bool IsPointerArray =>
            (Reference is ReferenceTypePointer referenceTypePointer && referenceTypePointer.IsArray(combinedIndex) ||
             Reference is ReferenceTypeArray)
            && NextReference is ReferenceTypePointer nextReferencePointer &&
            !nextReferencePointer.IsArray(combinedIndex);

        public IEvaluation PointerArrayCountEvaluation => NextReference is ReferenceTypePointer referenceTypePointer
            ? referenceTypePointer.Count
            : null;

        public bool IsDynamicArray =>
            Reference is ReferenceTypeArray referenceTypeArray && referenceTypeArray.DynamicSize != null;

        public IEvaluation DynamicArraySizeEvaluation => Reference is ReferenceTypeArray referenceTypeArray
            ? referenceTypeArray.DynamicSize
            : null;

        public MemberReferenceComputations(MemberInformation information)
        {
            this.information = information;
            referenceIndices = new List<int>();
            combinedIndex = 0;
        }

        private MemberReferenceComputations(MemberInformation information, IEnumerable<int> referenceIndices)
        {
            this.information = information;
            this.referenceIndices = new List<int>(referenceIndices);

            var arraySizes = information.Member.VariableType.References
                .OfType<ReferenceTypeArray>()
                .Select(array => array.ArraySize)
                .ToList();
            var indexDepth = 0;
            combinedIndex = 0;
            foreach (var referenceIndex in this.referenceIndices)
            {
                var sizePerIndexInCurrentDepth = arraySizes.Count <= indexDepth + 1
                    ? 1
                    : arraySizes.Skip(indexDepth + 1).Aggregate((i1, i2) => i1 * i2);

                combinedIndex += referenceIndex * sizePerIndexInCurrentDepth;
                indexDepth++;
            }
        }
    }
}