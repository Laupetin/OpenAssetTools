namespace ZoneCodeGenerator.Domain
{
    class ReferenceTypeArray : ReferenceType
    {
        public int ArraySize { get; }

        public ReferenceTypeArray(int arraySize)
        {
            ArraySize = arraySize;
        }
    }
}
