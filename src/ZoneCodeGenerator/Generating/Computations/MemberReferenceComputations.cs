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

        public IEnumerable<MemberReferenceComputations> ArrayEntries => Enumerable.Range(0, ArraySize)
            .Select(i => new MemberReferenceComputations(information, referenceIndices.Concat(new[] {i})));

        public bool IsSinglePointer => Reference is ReferenceTypePointer referenceTypePointer
                                       && !referenceTypePointer.IsArray
                                       && !FollowingReferences.OfType<ReferenceTypePointer>().Any();

        public bool IsArrayPointer => Reference is ReferenceTypePointer referenceTypePointer
                                      && referenceTypePointer.IsArray
                                      && !FollowingReferences.OfType<ReferenceTypePointer>().Any();

        public IEvaluation ArrayPointerCountEvaluation => Reference is ReferenceTypePointer referenceTypePointer
            ? referenceTypePointer.Count
            : null;

        public bool IsPointerArray =>
            (Reference is ReferenceTypePointer referenceTypePointer && referenceTypePointer.IsArray ||
             Reference is ReferenceTypeArray)
            && NextReference is ReferenceTypePointer nextReferencePointer && !nextReferencePointer.IsArray;

        public IEvaluation PointerArrayCountEvaluation => NextReference is ReferenceTypePointer referenceTypePointer
            ? referenceTypePointer.Count
            : null;

        public MemberReferenceComputations(MemberInformation information)
        {
            this.information = information;
            referenceIndices = new List<int>();
        }

        private MemberReferenceComputations(MemberInformation information, IEnumerable<int> referenceIndices)
        {
            this.information = information;
            this.referenceIndices = new List<int>(referenceIndices);
        }
    }
}