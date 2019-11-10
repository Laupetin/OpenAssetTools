using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorUsages : IDataPostProcessor
    {
        public bool PostProcess(IDataRepository repository)
        {
            foreach (var dataTypeWithMembers in repository.GetAllStructs()
                .AsEnumerable<DataTypeWithMembers>()
                .Concat(repository.GetAllUnions()))
            {
                var information = repository.GetInformationFor(dataTypeWithMembers);

                foreach (var memberInformation in information.OrderedMembers)
                {
                    if (memberInformation.StructureType == null) continue;

                    memberInformation.StructureType.Usages.Add(information);

                    if (memberInformation.IsNonEmbeddedReference)
                        memberInformation.StructureType.NonEmbeddedReferenceExists = true;

                    if (memberInformation.IsSinglePointerReference)
                        memberInformation.StructureType.SinglePointerReferenceExists = true;

                    if (memberInformation.IsArrayPointerReference)
                        memberInformation.StructureType.ArrayPointerReferenceExists = true;

                    if (memberInformation.IsArrayReference)
                        memberInformation.StructureType.ArrayReferenceExists = true;
                }
            }

            return true;
        }
    }
}