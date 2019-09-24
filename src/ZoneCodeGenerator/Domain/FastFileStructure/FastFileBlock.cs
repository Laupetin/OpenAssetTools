namespace ZoneCodeGenerator.Domain.FastFileStructure
{
    class FastFileBlock
    {
        public enum Type
        {
            Temp,
            Runtime,
            Delay,
            Normal
        }

        public string Name { get; }
        public int Index { get; }
        public Type BlockType { get; }
        public bool IsDefault { get; }
        public bool IsTemp => BlockType == Type.Temp;
        public bool IsRuntime => BlockType == Type.Runtime;
        public bool IsDelay => BlockType == Type.Delay;
        public bool IsNormal => BlockType == Type.Normal;

        public FastFileBlock(string name, int index, Type blockType, bool isDefault)
        {
            Name = name;
            Index = index;
            BlockType = blockType;
            IsDefault = isDefault;
        }
    }
}
