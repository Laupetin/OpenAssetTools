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

        public bool ShouldIgnore => information.Condition != null
                                    && information.Condition.IsStatic
                                    && information.Condition.EvaluateNumeric() == 0;

        public bool ContainsNonEmbeddedReference =>
            information.Member.VariableType.References.OfType<ReferenceTypePointer>().Any();

        public bool ContainsSinglePointerReference => information.Member.VariableType.References.Any()
                                                      && information.Member.VariableType.References.Last() is
                                                          ReferenceTypePointer pointerReference
                                                      && !pointerReference.AnyIsArray;

        public bool ContainsArrayPointerReference => information.Member.VariableType.References.Any()
                                                     && information.Member.VariableType.References.Last() is
                                                         ReferenceTypePointer pointerReference
                                                     && pointerReference.AnyIsArray;

        public bool ContainsPointerArrayReference => ContainsSinglePointerReference
                                                     && (IsArray && PointerDepthIsOne || !IsArray && PointerDepthIsTwo);

        public bool ContainsArrayReference => information.Member.VariableType.References.Any()
                                              && information.Member.VariableType.References
                                                  .Last() is ReferenceTypeArray;


        public IEvaluation ArrayPointerCountEvaluation =>
            information.Member.VariableType.References.Last() is ReferenceTypePointer pointerReference
                ? pointerReference.Count
                : null;

        public bool IsArray => information.Member.VariableType.References
            .TakeWhile(type => type is ReferenceTypeArray)
            .Any();

        public IEnumerable<int> ArraySizes => information.Member.VariableType.References
            .TakeWhile(type => type is ReferenceTypeArray)
            .OfType<ReferenceTypeArray>()
            .Select(array => array.ArraySize);

        public int ArrayDimension => information.Member.VariableType.References
            .TakeWhile(type => type is ReferenceTypeArray)
            .Count();

        public bool IsPointerToArray => information.Member.VariableType.References.OfType<ReferenceTypePointer>().Any()
                                        && information.Member.VariableType.References.Last() is ReferenceTypeArray;

        public IEnumerable<int> PointerToArraySizes => information.Member.VariableType.References
            .Reverse()
            .TakeWhile(type => type is ReferenceTypeArray)
            .OfType<ReferenceTypeArray>()
            .Reverse()
            .Select(array => array.ArraySize);

        public int PointerDepth => information.Member.VariableType.References
            .OfType<ReferenceTypePointer>()
            .Count();

        public bool PointerDepthIsOne => PointerDepth == 1;
        public bool PointerDepthIsTwo => PointerDepth == 2;

        public bool IsNotDefaultNormalBlock =>
            information.Block != null && !(information.Block.IsNormal && information.Block.IsDefault);

        public bool IsTempBlock => information.Block != null && information.Block.IsTemp;
        public bool IsRuntimeBlock => information.Block != null && information.Block.IsRuntime;

        public bool IsFirstMember =>
            information.Parent.OrderedMembers.FirstOrDefault(member =>
                !member.IsLeaf && !member.Computations.ShouldIgnore) == information;

        public bool IsLastMember =>
            information.Parent.OrderedMembers.LastOrDefault(member =>
                !member.IsLeaf && !member.Computations.ShouldIgnore) == information;

        public bool HasDynamicArraySize => information.Member.VariableType.References
            .OfType<ReferenceTypeArray>()
            .Any(array => array.DynamicSize != null);

        public bool IsBarrier => HasDynamicArraySize ||
                                 information.StructureType != null &&
                                  information.StructureType != information.Parent &&
                                  information.StructureType.Computations.Barrier != null;

        public MemberReferenceComputations References => new MemberReferenceComputations(information);

        public MemberComputations(MemberInformation information)
        {
            this.information = information;
        }
    }
}