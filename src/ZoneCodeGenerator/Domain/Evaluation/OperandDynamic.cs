using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZoneCodeGenerator.Domain.Evaluation
{
    class OperandDynamic : IEvaluation
    {
        public bool IsStatic => false;
        public int EvaluateNumeric()
        {
            throw new Exception("A dynamic operand cannot be evaluated.");
        }
    }
}
