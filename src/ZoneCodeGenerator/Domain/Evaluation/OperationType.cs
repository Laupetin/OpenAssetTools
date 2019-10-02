using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace ZoneCodeGenerator.Domain.Evaluation
{
    sealed class OperationType
    {
        // https://en.cppreference.com/w/cpp/language/operator_precedence
        public enum OperationPrecedence
        {
            MultiplicationDivisionRemainder = 1,
            AdditionSubtraction = 2,
            BitwiseShift = 3,
            RelationalGreaterLessThan = 4,
            RelationalEquals = 5,
            LogicalAnd = 6,
            LogicalOr = 7
        }

        public delegate int EvaluationFunction(int operand1, int operand2);

        public string Syntax { get; }
        public OperationPrecedence Precedence { get; }
        public EvaluationFunction Function { get; }

        public override string ToString()
        {
            return Syntax;
        }

        private OperationType(string syntax, OperationPrecedence precedence, EvaluationFunction function)
        {
            Syntax = syntax;
            Precedence = precedence;
            Function = function;
        }

        public static OperationType OperationAdd = new OperationType(
            "+",
            OperationPrecedence.AdditionSubtraction,
            (operand1, operand2) => operand1 + operand2
            );

        public static OperationType OperationSubtract = new OperationType(
            "-",
            OperationPrecedence.AdditionSubtraction,
            (operand1, operand2) => operand1 - operand2
            );

        public static OperationType OperationMultiply = new OperationType(
            "*",
            OperationPrecedence.MultiplicationDivisionRemainder,
            (operand1, operand2) => operand1 * operand2
            );

        public static OperationType OperationDivide = new OperationType(
            "/",
            OperationPrecedence.MultiplicationDivisionRemainder,
            (operand1, operand2) => operand1 / operand2
            );

        public static OperationType OperationRemainder = new OperationType(
            "%",
            OperationPrecedence.MultiplicationDivisionRemainder,
            (operand1, operand2) => operand1 % operand2
            );

        public static OperationType OperationShiftLeft = new OperationType(
            "<<",
            OperationPrecedence.BitwiseShift,
            (operand1, operand2) => operand1 << operand2
            );

        public static OperationType OperationShiftRight = new OperationType(
            ">>",
            OperationPrecedence.BitwiseShift,
            (operand1, operand2) => operand1 >> operand2
            );

        public static OperationType OperationGreaterThan = new OperationType(
            ">",
            OperationPrecedence.RelationalGreaterLessThan,
            (operand1, operand2) => operand1 > operand2 ? 1 : 0
            );

        public static OperationType OperationGreaterEqualsThan = new OperationType(
            ">=",
            OperationPrecedence.RelationalGreaterLessThan,
            (operand1, operand2) => operand1 >= operand2 ? 1 : 0
            );

        public static OperationType OperationLessThan = new OperationType(
            "<",
            OperationPrecedence.RelationalGreaterLessThan,
            (operand1, operand2) => operand1 < operand2 ? 1 : 0
            );

        public static OperationType OperationLessEqualsThan = new OperationType(
            "<=",
            OperationPrecedence.RelationalGreaterLessThan,
            (operand1, operand2) => operand1 <= operand2 ? 1 : 0
            );

        public static OperationType OperationEquals = new OperationType(
            "==",
            OperationPrecedence.RelationalEquals,
            (operand1, operand2) => operand1 == operand2 ? 1 : 0
            );

        public static OperationType OperationNotEquals = new OperationType(
            "!=",
            OperationPrecedence.RelationalEquals,
            (operand1, operand2) => operand1 != operand2 ? 1 : 0
            );

        public static OperationType OperationAnd = new OperationType(
            "&&",
            OperationPrecedence.LogicalAnd,
            (operand1, operand2) => operand1 > 0 && operand2 > 0 ? 1 : 0
            );

        public static OperationType OperationOr = new OperationType(
            "||",
            OperationPrecedence.LogicalOr,
            (operand1, operand2) => operand1 > 0 || operand2 > 0 ? 1 : 0
            );

        public static List<OperationType> Types => typeof(OperationType)
            .GetFields(BindingFlags.Static | BindingFlags.Public)
            .Select(info => info.GetValue(null))
            .OfType<OperationType>()
            .ToList();
    }
}