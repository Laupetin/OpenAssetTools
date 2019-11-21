using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZoneCodeGenerator.Domain.Evaluation
{
    class OperandStatic : IEvaluation
    {
        public int Value { get; }
        public EnumMember EnumMemberValue { get; }

        public bool IsStatic => true;
        public int EvaluateNumeric()
        {
            return Value;
        }

        public OperandStatic(int value)
        {
            Value = value;
            EnumMemberValue = null;
        }

        public OperandStatic(EnumMember enumMember)
        {
            Value = (int) enumMember.Value;
            EnumMemberValue = enumMember;
        }

        public override string ToString()
        {
            return Value.ToString();
        }
    }
}
