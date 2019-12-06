using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Interface;

namespace ZoneCodeGenerator.Generating
{
    class RenderingContext
    {
        public class UsedType
        {
            public DataType Type { get; }
            public StructureInformation Information { get; }
            public bool IsContextAsset { get; set; }

            public bool NonEmbeddedReferenceExists { get; set; }
            public bool ArrayReferenceExists { get; set; }
            public bool PointerArrayReferenceExists { get; set; }
            public bool PointerArrayReferenceIsReusable { get; set; }

            public UsedType(DataType type, StructureInformation information)
            {
                Type = type;
                Information = information;
                IsContextAsset = false;
                NonEmbeddedReferenceExists = false;
                ArrayReferenceExists = false;
                PointerArrayReferenceExists = false;
                PointerArrayReferenceIsReusable = false;
            }
        }

        public string Game { get; set; }
        public StructureInformation Asset { get; set; }

        private readonly IDictionary<DataType, UsedType> usedTypes;
        public IEnumerable<UsedType> UsedTypes => usedTypes.Values;

        public IEnumerable<StructureInformation> UsedStructures => UsedTypes
            .Select(type => type.Information)
            .Where(information => information != null)
            .Distinct();

        public IEnumerable<UsedType> ReferencedAssets => UsedTypes.Where(type => type.Information != null && type.Information.IsAsset && type.Information != Asset);

        public IList<FastFileBlock> Blocks { get; private set; }

        public FastFileBlock DefaultNormalBlock => Blocks.FirstOrDefault(block => block.IsDefault && block.IsNormal) ??
                                                   Blocks.FirstOrDefault(block => block.IsNormal);

        public FastFileBlock DefaultTempBlock => Blocks.FirstOrDefault(block => block.IsDefault && block.IsTemp) ??
                                                   Blocks.FirstOrDefault(block => block.IsTemp);

        private RenderingContext()
        {
            usedTypes = new Dictionary<DataType, UsedType>();
        }

        private void AddToContext(StructureInformation structureInformation)
        {
            if (usedTypes.ContainsKey(structureInformation.Type))
                return;

            var newUsage = new UsedType(structureInformation.Type, structureInformation);
            usedTypes.Add(newUsage.Type, newUsage);

            if (structureInformation.IsAsset && structureInformation != Asset)
                return;

            foreach (var member in structureInformation.OrderedMembers
                .Where(member => !member.Computations.ShouldIgnore))
            {
                if(member.StructureType != null)
                    AddToContext(member.StructureType);

                UsedType usedType;
                if (!usedTypes.ContainsKey(member.Member.VariableType.Type))
                {
                    usedType = new UsedType(member.Member.VariableType.Type, member.StructureType);
                    usedTypes.Add(usedType.Type, usedType);
                }
                else
                {
                    usedType = usedTypes[member.Member.VariableType.Type];
                }

                if (member.Computations.ContainsNonEmbeddedReference)
                    usedType.NonEmbeddedReferenceExists = true;

                if (member.Computations.ContainsArrayPointerReference || member.Computations.ContainsArrayReference)
                    usedType.ArrayReferenceExists = true;

                if (member.Computations.ContainsPointerArrayReference && !member.IsString)
                {
                    usedType.PointerArrayReferenceExists = true;

                    if (member.IsReusable)
                        usedType.PointerArrayReferenceIsReusable = true;
                }
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
            context.usedTypes[asset.Type].IsContextAsset = true;

            return context;
        }
    }
}