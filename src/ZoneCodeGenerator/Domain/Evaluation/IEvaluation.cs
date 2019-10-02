using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZoneCodeGenerator.Domain.Evaluation
{
    interface IEvaluation
    {
        bool IsStatic { get; }

        int EvaluateNumeric();
    }
}
