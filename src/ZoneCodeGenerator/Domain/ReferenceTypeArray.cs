using ZoneCodeGenerator.Domain.Evaluation;

namespace ZoneCodeGenerator.Domain
{
    class ReferenceTypeArray : ReferenceType
    {
        public int ArraySize { get; }

        public IEvaluation DynamicSize { get; set; }

        public ReferenceTypeArray(int arraySize)
        {
            ArraySize = arraySize;
            DynamicSize = null;
        }
    }
}
