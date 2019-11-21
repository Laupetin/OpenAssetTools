using System.Collections.Generic;
using System.IO;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Interface;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Generating
{
    class RenderingContext
    {
        public string Game { get; set; }
        public StructureInformation Asset { get; set; }

        public ISet<StructureInformation> Structures { get; }
        public ISet<DataType> MemberTypes { get; }

        public IEnumerable<StructureInformation> ReferencedAssets =>
            Structures.Where(inf => inf.IsAsset && inf != Asset);

        public IList<FastFileBlock> Blocks { get; private set; }

        public FastFileBlock DefaultNormalBlock => Blocks.FirstOrDefault(block => block.IsDefault && block.IsNormal) ??
                                                   Blocks.FirstOrDefault(block => block.IsNormal);

        private RenderingContext()
        {
            Structures = new HashSet<StructureInformation>();
            MemberTypes = new HashSet<DataType>();
        }

        private void AddToContext(StructureInformation structureInformation)
        {
            if (!Structures.Add(structureInformation))
                return;

            if (structureInformation.IsAsset && structureInformation != Asset)
                return;

            foreach (var member in structureInformation.OrderedMembers
                .Where(member => member.StructureType != null && !member.Computations.ShouldIgnore))
            {
                AddToContext(member.StructureType);
            }
        }

        public static RenderingContext BuildContext(CUISession session, StructureInformation asset)
        {
            var context = new RenderingContext
            {
                Asset = asset,
                Game = session.Game,
                Blocks = session.Repository.GetAllFastFileBlocks().ToList()
            };

            context.AddToContext(asset);
            context.MemberTypes.UnionWith(context.Structures
                .Where(structureInformation => !structureInformation.IsAsset || structureInformation == asset)
                .Where(structureInformation =>
                    structureInformation.Computations.IsUsed || structureInformation == asset)
                .SelectMany(information => information.OrderedMembers)
                .Where(information => !information.Computations.ShouldIgnore)
                .Select(information => information.Member.VariableType.Type)
                .Where(type => !(type is DataTypeBaseType) && type != asset.Type)
                .Distinct());

            return context;
        }
    }
}