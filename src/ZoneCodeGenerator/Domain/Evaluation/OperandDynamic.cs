using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain.Information;

namespace ZoneCodeGenerator.Domain.Evaluation
{
    class OperandDynamic : IEvaluation
    {
        public StructureInformation Structure { get; }
        public IList<MemberInformation> ReferencedMemberChain { get; }
        public IList<int> ArrayIndices { get; }

        public bool IsStatic => false;

        public OperandDynamic(StructureInformation structure, IEnumerable<MemberInformation> memberChain)
        {
            Structure = structure;
            ReferencedMemberChain = new List<MemberInformation>(memberChain);
            ArrayIndices = new List<int>();
        }

        public int EvaluateNumeric()
        {
            throw new Exception("A dynamic operand cannot be evaluated.");
        }

        public override string ToString()
        {
            return $"{Structure.Type.FullName}::{string.Join("::", ReferencedMemberChain.Select(information => information.Member.Name))}{string.Concat(ArrayIndices.Select(i => $"[{i}]"))}";
        }
    }
}
