using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorAssetName : IDataPostProcessor
    {
        private const string NameMemberName = "name";

        private static List<MemberInformation> FindNameMember(StructureInformation information)
        {
            var nameMemberInformation =
                information.OrderedMembers.FirstOrDefault(memberInformation =>
                    memberInformation.Member.Name.Equals(NameMemberName));

            if (nameMemberInformation != null)
            {
                var result = new List<MemberInformation> {nameMemberInformation};
                return result;
            }

            foreach (var embeddedMembers in information.OrderedMembers.Where(memberInformation =>
                memberInformation.Computations.IsEmbeddedReference && memberInformation.StructureType != null))
            {
                var embeddedMemberName = FindNameMember(embeddedMembers.StructureType);

                if (embeddedMemberName == null) continue;

                embeddedMemberName.Insert(0, embeddedMembers);
                return embeddedMemberName;
            }

            return null;
        }

        public bool PostProcess(IDataRepository repository)
        {
            var assetDataTypes =
                repository.GetAllStructs()
                    .AsEnumerable<DataTypeWithMembers>()
                    .Concat(repository.GetAllUnions())
                    .Select(repository.GetInformationFor)
                    .Where(information => information.IsAsset);

            foreach (var assetInformation in assetDataTypes)
            {
                assetInformation.NameChain = FindNameMember(assetInformation);
            }

            return true;
        }
    }
}