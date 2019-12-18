using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain.Evaluation;

namespace ZoneCodeGenerator.Domain
{
    class ReferenceTypePointer : ReferenceType
    {
        public static IEvaluation DefaultCount = new OperandStatic(1);

        public IEvaluation Count { get; set; } = DefaultCount;
        public IEvaluation[] CountByArrayIndex { get; set; }
        public bool HasCountByArrayIndex => CountByArrayIndex != null;

        private static bool EvaluationIsArray(IEvaluation eval)
        {
            return !eval.IsStatic || eval.EvaluateNumeric() > 1;
        }

        public bool IsArray(int index)
        {
            return EvaluationIsArray(HasCountByArrayIndex ? CountByArrayIndex[index] : Count);
        }

        public bool AnyIsArray =>
            HasCountByArrayIndex ? CountByArrayIndex.Any(EvaluationIsArray) : EvaluationIsArray(Count);
    }
}