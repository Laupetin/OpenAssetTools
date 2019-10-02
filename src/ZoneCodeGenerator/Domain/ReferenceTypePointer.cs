using System;
using ZoneCodeGenerator.Domain.Evaluation;

namespace ZoneCodeGenerator.Domain
{
    class ReferenceTypePointer : ReferenceType
    {
        public static IEvaluation DefaultCount = new OperandStatic(1);

        public IEvaluation Count { get; set; } = DefaultCount;

        public bool IsArray => !Count.IsStatic || Count.EvaluateNumeric() > 1;
    }
}
