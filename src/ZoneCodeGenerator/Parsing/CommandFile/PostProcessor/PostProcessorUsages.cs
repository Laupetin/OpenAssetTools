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

                    if (memberInformation.Computations.IsNonEmbeddedReference)
                        memberInformation.StructureType.NonEmbeddedReferenceExists = true;

                    if (memberInformation.Computations.IsSinglePointerReference)
                        memberInformation.StructureType.SinglePointerReferenceExists = true;

                    if (memberInformation.Computations.IsArrayPointerReference)
                        memberInformation.StructureType.ArrayPointerReferenceExists = true;

                    if (memberInformation.Computations.IsArrayReference)
                        memberInformation.StructureType.ArrayReferenceExists = true;
                }
            }

            return true;
        }
    }
}