using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Generating.Computations;

namespace ZoneCodeGenerator.Domain.Information
{
    class MemberInformation
    {
        public StructureInformation Parent { get; }
        public StructureInformation StructureType { get; }
        public Variable Member { get; set; }
        public bool IsString { get; set; }
        public bool IsScriptString { get; set; }
        public bool IsReusable { get; set; }
        public bool IsLeaf { get; set; }
        public IEvaluation Condition { get; set; }
        public FastFileBlock Block { get; set; }

        public MemberComputations Computations => new MemberComputations(this);

        public MemberInformation(StructureInformation parent, Variable member, StructureInformation structureType)
        {
            Parent = parent;
            Member = member;
            StructureType = structureType;
            IsString = false;
            IsScriptString = false;
            IsReusable = false;
            IsLeaf = false;
            Condition = null;
            Block = null;
        }

        public override string ToString()
        {
            return $"info for {Member}";
        }
    }
}