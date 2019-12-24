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
        public bool Operand1NeedsParenthesis => Operand1 is Operation operation1 &&
                                                operation1.OperationType.Precedence > OperationType.Precedence;

        public IEvaluation Operand2 { get; }
        public bool Operand2NeedsParenthesis => Operand2 is Operation operation2 &&
                                                operation2.OperationType.Precedence >= OperationType.Precedence;

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

        public override string ToString()
        {
            string operand1String;
            string operand2String;

            if (Operand1 is Operation operation1
                && operation1.OperationType.Precedence > OperationType.Precedence)
            {
                operand1String = $"({Operand1})";
            }
            else
            {
                operand1String = Operand1.ToString();
            }

            if (Operand2 is Operation operation2
                && operation2.OperationType.Precedence > OperationType.Precedence)
            {
                operand2String = $"({Operand2})";
            }
            else
            {
                operand2String = Operand2.ToString();
            }

            return $"{operand1String} {OperationType} {operand2String}";
        }
    }
}
