namespace ZoneCodeGenerator.Domain
{
    class DataTypeTypedef : DataType
    {
        public TypeDeclaration TypeDefinition { get; }

        public DataTypeTypedef(string _namespace, string name, TypeDeclaration typeDefinitionDeclaration) : base(_namespace, name, DataTypeType.Typedef)
        {
            TypeDefinition = typeDefinitionDeclaration;
        }

        public int? AlignmentOverride { get; set; }
        public override int Alignment => AlignmentOverride ?? TypeDefinition.Alignment;
        public override bool ForceAlignment => AlignmentOverride != null || TypeDefinition.ForceAlignment;
        public override int Size => TypeDefinition.Size;

        public override string ToString()
        {
            return $"typedef {FullName}";
        }
    }
}