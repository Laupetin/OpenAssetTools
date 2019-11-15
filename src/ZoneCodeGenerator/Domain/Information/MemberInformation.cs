using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Generating.Computations;

namespace ZoneCodeGenerator.Domain.Information
{
    class MemberInformation
    {
        public StructureInformation StructureType { get; }
        public Variable Member { get; set; }
        public bool IsString { get; set; }
        public bool IsScriptString { get; set; }
        public bool IsReusable { get; set; }
        public IEvaluation Condition { get; set; }

        public MemberComputations Computations => new MemberComputations(this);

        public MemberInformation(Variable member, StructureInformation structureType)
        {
            Member = member;
            StructureType = structureType;
            IsString = false;
            IsScriptString = false;
            IsReusable = false;
            Condition = null;
        }

        public override string ToString()
        {
            return $"info for {Member}";
        }
    }
}