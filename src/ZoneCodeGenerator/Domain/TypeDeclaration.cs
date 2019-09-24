using System.Collections.Generic;
using System.Linq;

namespace ZoneCodeGenerator.Domain
{
    class TypeDeclaration
    {
        private const int PointerSize = 4;

        private DataType type;
        public DataType Type
        {
            get
            {
                if (type is DataTypeTypedef typedef)
                {
                    return typedef.TypeDefinition.Type;
                }

                return type;
            }

            set => type = value;
        }

        public int? CustomBitSize { get; }

        public bool HasCustomBitSize => CustomBitSize != null;

        private readonly List<ReferenceType> references;
        public IReadOnlyList<ReferenceType> References => references.AsReadOnly();

        public int Alignment => references.OfType<ReferenceTypePointer>().Any() ? PointerSize : type.Alignment;

        public int Size
        {
            get
            {
                var currentSize = Type.Size;

                foreach (var reference in References.Reverse())
                {
                    switch (reference)
                    {
                        case ReferenceTypePointer _:
                            currentSize = PointerSize;
                            break;
                        case ReferenceTypeArray array:
                            currentSize *= array.ArraySize;
                            break;
                    }
                }

                return currentSize;
            }
        }

        public TypeDeclaration(DataType type, List<ReferenceType> references)
        {
            this.type = type;
            this.references = references ?? new List<ReferenceType>();
            CustomBitSize = null;
        }

        public TypeDeclaration(DataType type, int customBitSize, List<ReferenceType> references) : this(type, references)
        {
            CustomBitSize = customBitSize;
        }
    }
}
