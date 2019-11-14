using System.Collections.Generic;

namespace ZoneCodeGenerator.Domain
{
    class DataTypeEnum : DataType
    {
        public DataTypeBaseType ParentType { get; }
        public List<EnumMember> Members { get; }

        public override int Size => ParentType.Size;
        public override int Alignment => ParentType.Alignment;

        public DataTypeEnum(string _namespace, string name, DataTypeBaseType parentType) : base(_namespace, name, DataTypeType.Enum)
        {
            Members = new List<EnumMember>();
            ParentType = parentType;
        }

        public override string ToString()
        {
            return $"enum {FullName}";
        }
    }
}
