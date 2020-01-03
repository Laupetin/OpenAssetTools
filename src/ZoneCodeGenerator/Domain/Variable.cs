namespace ZoneCodeGenerator.Domain
{
    class Variable
    {
        public string Name { get; }

        public int? AlignmentOverride { get; set; }

        public int Alignment => AlignmentOverride ?? VariableType.Alignment;
        public bool ForceAlignment => AlignmentOverride != null || VariableType.ForceAlignment;

        public TypeDeclaration VariableType { get; }

        public Variable(string name, TypeDeclaration type)
        {
            Name = name;
            VariableType = type;
        }
    }
}
