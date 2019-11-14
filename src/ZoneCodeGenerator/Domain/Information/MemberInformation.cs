using System.Linq;
using ZoneCodeGenerator.Domain.Evaluation;

namespace ZoneCodeGenerator.Domain.Information
{
    class MemberInformation
    {
        public StructureInformation StructureType { get; }
        public Variable Member { get; set; }
        public bool IsString { get; set; }
        public bool IsScriptString { get; set; }
        public IEvaluation Condition { get; set; }

        public bool IsNonEmbeddedReference => Member.VariableType.References.Any();

        public bool IsSinglePointerReference => Member.VariableType.References.Any()
                                                && Member.VariableType.References.Last() is ReferenceTypePointer
                                                    pointerReference
                                                && !pointerReference.IsArray;

        public bool IsArrayPointerReference => Member.VariableType.References.Any()
                                               && Member.VariableType.References.Last() is ReferenceTypePointer
                                                   pointerReference
                                               && pointerReference.IsArray;

        public bool IsArrayReference => Member.VariableType.References.Any()
                                        && Member.VariableType.References.Last() is ReferenceTypeArray;

        public MemberInformation(Variable member, StructureInformation structureType)
        {
            Member = member;
            StructureType = structureType;
            IsString = false;
            IsScriptString = false;
            Condition = null;
        }

        public override string ToString()
        {
            return $"info for {Member}";
        }
    }
}