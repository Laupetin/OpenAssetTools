namespace ZoneCodeGenerator.Domain
{
    class DataTypeTypedef : DataType
    {
        public TypeDeclaration TypeDefinition { get; }

        public DataTypeTypedef(string _namespace, string name, TypeDeclaration typeDefinitionDeclaration) : base(_namespace, name, DataTypeType.Typedef)
        {
            TypeDefinition = typeDefinitionDeclaration;
        }

        public override int Alignment => TypeDefinition.Alignment;
        public override int Size => TypeDefinition.Size;
    }
}