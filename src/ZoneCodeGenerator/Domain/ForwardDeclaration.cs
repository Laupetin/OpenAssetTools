using System;

namespace ZoneCodeGenerator.Domain
{
    class ForwardDeclaration : DataType
    {
        private DataType forwardedType;

        public DataType ForwardedType
        {
            get => forwardedType;

            set
            {
                if (forwardedType == null)
                {
                    if(value.Type != Type)
                        throw new DataException($"Type of forwarded type '{Name}' does not match previous declaration");

                    forwardedType = value;
                }
                else if(forwardedType != value)
                {
                    throw new DataException($"Tried to set forwarded type '{Name}' multiple times with different types");
                }
            }
        }

        public ForwardDeclaration(string _namespace, string name, DataTypeType dataTypeType) : base(_namespace, name, dataTypeType)
        {
            switch (dataTypeType)
            {
                case DataTypeType.Enum:
                case DataTypeType.Struct:
                case DataTypeType.Union:
                    break;

                default:
                    throw new ArgumentException($"Cannot create forward declaration for type '{dataTypeType}'");
            }
            forwardedType = null;
        }

        public override int Alignment => 0;
        public override int Size => 0;
    }
}
