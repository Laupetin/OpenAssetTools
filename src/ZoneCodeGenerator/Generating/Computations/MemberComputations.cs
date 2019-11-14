using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Domain.Information;

namespace ZoneCodeGenerator.Generating.Computations
{
    class MemberComputations
    {
        private readonly MemberInformation information;

        public bool IsNonEmbeddedReference => information.Member.VariableType.References.OfType<ReferenceTypePointer>().Any();

        public bool IsSinglePointerReference => information.Member.VariableType.References.Any()
                                                && information.Member.VariableType.References.Last() is ReferenceTypePointer
                                                    pointerReference
                                                && !pointerReference.IsArray;

        public bool IsArrayPointerReference => information.Member.VariableType.References.Any()
                                               && information.Member.VariableType.References.Last() is ReferenceTypePointer
                                                   pointerReference
                                               && pointerReference.IsArray;

        public IEvaluation ArrayPointerCountEvaluation =>
            information.Member.VariableType.References.Last() is ReferenceTypePointer pointerReference
                ? pointerReference.Count
                : null;

        public bool IsArrayReference => information.Member.VariableType.References.Any()
                                        && information.Member.VariableType.References.Last() is ReferenceTypeArray;

        public IEnumerable<int> ArraySizes => information.Member.VariableType.References
            .OfType<ReferenceTypeArray>()
            .Select(array => array.ArraySize);

        public int PointerDepth => information.Member.VariableType.References
            .OfType<ReferenceTypePointer>()
            .Count();

        public bool IsSinglePointer => PointerDepth == 1;
        public bool IsDoublePointer => PointerDepth == 2;

        public MemberComputations(MemberInformation information)
        {
            this.information = information;
        }
    }
}