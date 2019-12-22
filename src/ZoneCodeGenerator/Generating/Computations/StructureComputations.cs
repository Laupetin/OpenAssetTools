using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Domain.Information;

namespace ZoneCodeGenerator.Generating.Computations
{
    class StructureComputations
    {
        private readonly StructureInformation information;

        public bool IsUsed => information.Usages.Any();

        public MemberInformation Barrier =>
            information.OrderedMembers.FirstOrDefault(memberInformation => memberInformation.Computations.IsBarrier);

        public StructureComputations(StructureInformation information)
        {
            this.information = information;
        }
    }
}