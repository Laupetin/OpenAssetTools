using System.Collections.Generic;
using System.Linq;

namespace ZoneCodeGenerator.Domain
{
    abstract class DataTypeWithMembers : DataType
    {
        public int? AlignmentOverride { get; set; }
        private int alignment;
        public override int Alignment => AlignmentOverride ?? alignment;

        private int size;
        public override int Size => size;

        public int Pack { get; }

        public List<Variable> Members { get; }

        public bool IsAnonymous { get; set; }

        private bool finalized;
        public void FinalizeDataType()
        {
            if (finalized) return;
            
            finalized = true;
            CalculateProperties();
        }

        protected DataTypeWithMembers(string _namespace, string name, int pack, DataTypeType type) : base(_namespace, name, type)
        {
            Members = new List<Variable>();
            Pack = pack;
            finalized = false;
            IsAnonymous = false;
        }

        private void CalculateProperties()
        {
            foreach (var member in Members
                .Select(variable => variable.VariableType.Type)
                .OfType<DataTypeWithMembers>())
            {
                member.FinalizeDataType();
            }

            alignment = Members.Select(variable => variable.Alignment).Max();
            size = CalculateSize();
        }

        protected abstract int CalculateSize();
    }
}
