using ZoneCodeGenerator.Domain.Evaluation;

namespace ZoneCodeGenerator.Domain.Information
{
    class MemberInformation
    {
        public StructureInformation StructureType { get; }
        public Variable Member { get; set; }
        public bool IsScriptString { get; set; }
        public IEvaluation Condition { get; set; }

        public MemberInformation(Variable member, StructureInformation structureType)
        {
            Member = member;
            StructureType = structureType;
            IsScriptString = false;
            Condition = null;
        }
    }
}
