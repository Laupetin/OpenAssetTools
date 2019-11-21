using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorUsages : IDataPostProcessor
    {
        private static bool ProcessAsset(StructureInformation assetStructure)
        {
            var processedAssets = new HashSet<StructureInformation>();
            var processingQueue = new Queue<StructureInformation>();
            processingQueue.Enqueue(assetStructure);

            while (processingQueue.Count != 0)
            {
                var currentStructure = processingQueue.Dequeue();

                if (!processedAssets.Add(currentStructure))
                {
                    continue;
                }

                foreach (var member in currentStructure.OrderedMembers
                    .Where(member => member.StructureType != null)
                    .Where(member => !member.Computations.ShouldIgnore))
                {
                    if (member.Computations.IsNonEmbeddedReference)
                        member.StructureType.NonEmbeddedReferenceExists = true;

                    if (member.Computations.IsSinglePointerReference)
                        member.StructureType.SinglePointerReferenceExists = true;

                    if (member.Computations.IsArrayPointerReference)
                        member.StructureType.ArrayPointerReferenceExists = true;

                    if (member.Computations.IsArrayReference)
                        member.StructureType.ArrayReferenceExists = true;

                    member.StructureType.Usages.Add(currentStructure);
                    processingQueue.Enqueue(member.StructureType);
                }
            }

            return true;
        }

        public bool PostProcess(IDataRepository repository)
        {
            return repository.GetAllStructs()
                .AsEnumerable<DataTypeWithMembers>()
                .Concat(repository.GetAllUnions())
                .Select(repository.GetInformationFor)
                .Where(information => information.IsAsset)
                .All(ProcessAsset);
        }
    }
}