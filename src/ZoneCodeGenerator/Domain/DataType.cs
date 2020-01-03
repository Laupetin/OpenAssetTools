namespace ZoneCodeGenerator.Domain
{
    abstract class DataType
    {
        public string Namespace { get; }
        public string Name { get; }
        public DataTypeType Type { get; }
        public string FullName => string.IsNullOrEmpty(Namespace) ? Name : $"{Namespace}::{Name}";

        public abstract int Alignment { get; }
        public abstract bool ForceAlignment { get; }
        public abstract int Size { get; }

        protected DataType(string _namespace, string name, DataTypeType type)
        {
            Namespace = _namespace;
            Name = name;
            Type = type;
        }
    }
}
