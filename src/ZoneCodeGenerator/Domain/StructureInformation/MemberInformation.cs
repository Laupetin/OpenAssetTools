namespace ZoneCodeGenerator.Domain.StructureInformation
{
    class MemberInformation
    {
        public StructureInformation StructureType { get; }
        public Variable Member { get; set; }
        public bool IsScriptString { get; set; }

        public MemberInformation(Variable member, StructureInformation structureType)
        {
            Member = member;
            StructureType = structureType;
            IsScriptString = false;
        }
    }
}
