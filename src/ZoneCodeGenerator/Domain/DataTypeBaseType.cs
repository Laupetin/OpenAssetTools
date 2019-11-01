using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace ZoneCodeGenerator.Domain
{
    class DataTypeBaseType : DataType
    {
        public static readonly DataTypeBaseType FLOAT = new DataTypeBaseType("float", 4);
        public static readonly DataTypeBaseType DOUBLE = new DataTypeBaseType("double", 8);
        public static readonly DataTypeBaseType BOOL = new DataTypeBaseType("bool", 1);
        public static readonly DataTypeBaseType CHAR = new DataTypeBaseType("char", 1);
        public static readonly DataTypeBaseType UNSIGNED_CHAR = new DataTypeBaseType("unsigned char", 1);
        public static readonly DataTypeBaseType SHORT = new DataTypeBaseType("short", 2);
        public static readonly DataTypeBaseType UNSIGNED_SHORT = new DataTypeBaseType("unsigned short", 2);
        public static readonly DataTypeBaseType INT = new DataTypeBaseType("int", 4);
        public static readonly DataTypeBaseType UNSIGNED_INT = new DataTypeBaseType("unsigned int", 4);
        public static readonly DataTypeBaseType LONG = new DataTypeBaseType("long", 4);
        public static readonly DataTypeBaseType UNSIGNED_LONG = new DataTypeBaseType("unsigned long", 4);
        public static readonly DataTypeBaseType LONG_LONG = new DataTypeBaseType("long long", 8);
        public static readonly DataTypeBaseType UNSIGNED_LONG_LONG = new DataTypeBaseType("unsigned long long", 8);
        public static readonly DataTypeBaseType VOID = new DataTypeBaseType("void", 0);

        public static readonly IEnumerable<DataTypeBaseType> BASE_TYPES = typeof(DataTypeBaseType)
            .GetFields(BindingFlags.Public | BindingFlags.Static)
            .Where(info => info.FieldType == typeof(DataTypeBaseType))
            .Select(info => info.GetValue(null))
            .Cast<DataTypeBaseType>()
            .ToList();

        public override int Size { get; }
        public override int Alignment => Size;

        private DataTypeBaseType(string name, int size) : base("", name, DataTypeType.BaseType)
        {
            Size = size;
        }
    }
}
