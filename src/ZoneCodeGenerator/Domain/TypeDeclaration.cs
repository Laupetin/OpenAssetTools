using System.Collections.Generic;
using System.Linq;

namespace ZoneCodeGenerator.Domain
{
    class TypeDeclaration
    {
        private const int PointerSize = 4;

        public DataType Type { get; set; }


        public int? CustomBitSize { get; set; }

        public bool IsConst { get; set; }

        public bool HasCustomBitSize => CustomBitSize != null;

        private readonly List<ReferenceType> references;
        public IReadOnlyList<ReferenceType> References => references.AsReadOnly();

        public int Alignment => references.OfType<ReferenceTypePointer>().Any() ? PointerSize : Type.Alignment;
        public bool ForceAlignment => !references.OfType<ReferenceTypePointer>().Any() && Type.ForceAlignment;

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
            Type = type;
            this.references = references ?? new List<ReferenceType>();
            CustomBitSize = null;
        }
    }
}