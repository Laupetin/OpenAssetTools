using System.Linq;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Domain
{
    class DataTypeUnion : DataTypeWithMembers
    {
        public DataTypeUnion(string _namespace, string name, int pack) : base(_namespace, name, pack, DataTypeType.Union)
        {
        }

        protected override int CalculateSize()
        {
            return Members
                .Select(variable => variable.VariableType.Size)
                .Max()
                .Align(Alignment);
        }
    }
}
