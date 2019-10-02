using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZoneCodeGenerator.Domain.Evaluation
{
    class Operation : IEvaluation
    {
        public IEvaluation Operand1 { get; }
        public IEvaluation Operand2 { get; }
        public OperationType OperationType { get; }

        public bool IsStatic => Operand1.IsStatic && Operand2.IsStatic;

        public int EvaluateNumeric()
        {
            return OperationType.Function(Operand1.EvaluateNumeric(), Operand2.EvaluateNumeric());
        }

        public Operation(IEvaluation operand1, IEvaluation operand2, OperationType type)
        {
            Operand1 = operand1;
            Operand2 = operand2;
            OperationType = type;
        }
    }
}
