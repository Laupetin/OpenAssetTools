using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorDefaultBlock : IDataPostProcessor
    {
        public bool PostProcess(IDataRepository repository)
        {
            var memberDataTypes = 
                repository.GetAllStructs()
                .AsEnumerable<DataTypeWithMembers>()
                .Concat(repository.GetAllUnions());

            var defaultTemp = repository.GetAllFastFileBlocks().First(block => block.BlockType == FastFileBlock.Type.Temp && block.IsDefault) ??
                              repository.GetAllFastFileBlocks().First(block => block.BlockType == FastFileBlock.Type.Temp);

            var defaultNormal = repository.GetAllFastFileBlocks().First(block => block.BlockType == FastFileBlock.Type.Normal && block.IsDefault) ??
                              repository.GetAllFastFileBlocks().First(block => block.BlockType == FastFileBlock.Type.Normal);

            foreach (var memberType in memberDataTypes)
            {
                var info = repository.GetInformationFor(memberType);

                info.Block = info.IsAsset ? defaultTemp : defaultNormal;
            }

            return true;
        }
    }
}
