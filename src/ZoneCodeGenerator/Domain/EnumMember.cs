namespace ZoneCodeGenerator.Domain
{
    class EnumMember
    {
        public string Name { get; }
        public long Value { get; }

        public EnumMember(string name, long value)
        {
            Name = name;
            Value = value;
        }
    }
}
