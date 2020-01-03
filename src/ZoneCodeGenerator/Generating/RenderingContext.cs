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
            public bool MembersLoaded { get; set; }
            public DataType Type { get; }
            public StructureInformation Information { get; }
            public bool IsContextAsset { get; set; }

            public bool NonRuntimeReferenceExists { get; set; }
            public bool NonEmbeddedReferenceExists { get; set; }
            public bool ArrayReferenceExists { get; set; }
            public bool PointerArrayReferenceExists { get; set; }
            public bool PointerArrayReferenceIsReusable { get; set; }

            public UsedType(DataType type, StructureInformation information)
            {
                MembersLoaded = false;
                Type = type;
                Information = information;
                IsContextAsset = false;
                NonRuntimeReferenceExists = false;
                NonEmbeddedReferenceExists = false;
                ArrayReferenceExists = false;
                PointerArrayReferenceExists = false;
                PointerArrayReferenceIsReusable = false;
            }
        }

        public string Game { get; set; }
        public StructureInformation Asset { get; private set; }

        private readonly IDictionary<DataType, UsedType> usedTypes;
        public IEnumerable<UsedType> UsedTypes => usedTypes.Values;

        public IEnumerable<UsedType> UsedStructures => UsedTypes
            .Where(usedType => usedType.Information != null && usedType.Information.Type == usedType.Type);

        public IEnumerable<UsedType> ReferencedAssets => UsedTypes.Where(type =>
            type.Information != null && type.Information.IsAsset && type.Information != Asset);

        public IList<FastFileBlock> Blocks { get; private set; }

        public FastFileBlock DefaultNormalBlock => Blocks.FirstOrDefault(block => block.IsDefault && block.IsNormal) ??
                                                   Blocks.FirstOrDefault(block => block.IsNormal);

        public FastFileBlock DefaultTempBlock => Blocks.FirstOrDefault(block => block.IsDefault && block.IsTemp) ??
                                                 Blocks.FirstOrDefault(block => block.IsTemp);

        public bool HasActions => UsedTypes.Any(type =>
            (type.Information == null || !type.Information.IsAsset || type.IsContextAsset) && type.NonRuntimeReferenceExists &&
            type.Information?.PostLoadAction != null);

        private RenderingContext()
        {
            usedTypes = new Dictionary<DataType, UsedType>();
        }

        private UsedType GetBaseType(UsedType usedType)
        {
            if (usedType.Type is DataTypeTypedef typeDef)
            {
                while (typeDef.TypeDefinition.Type is DataTypeTypedef anotherTypeDef)
                {
                    typeDef = anotherTypeDef;
                }

                if (!usedTypes.ContainsKey(typeDef.TypeDefinition.Type))
                {
                    var result = new UsedType(typeDef.TypeDefinition.Type, usedType.Information);
                    usedTypes.Add(typeDef.TypeDefinition.Type, result);

                    return result;
                }

                return usedTypes[typeDef.TypeDefinition.Type];
            }

            return null;
        }

        private void AddMembersToContext(StructureInformation structureInformation)
        {
            foreach (var member in structureInformation.OrderedMembers
                .Where(member => !member.Computations.ShouldIgnore))
            {
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

                var baseUsedType = GetBaseType(usedType);

                if (!member.Computations.IsRuntimeBlock)
                {
                    usedType.NonRuntimeReferenceExists = true;

                    if (baseUsedType != null)
                        baseUsedType.NonRuntimeReferenceExists = true;
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

                if (usedType.Information != null && !usedType.Information.IsAsset && !member.Computations.IsRuntimeBlock &&
                    !usedType.MembersLoaded)
                {
                    usedType.MembersLoaded = true;
                    AddMembersToContext(usedType.Information);
                }
            }
        }

        private void MakeAsset(StructureInformation asset)
        {
            Asset = asset;
            var usage = new UsedType(asset.Type, asset) {IsContextAsset = true};
            usedTypes.Add(usage.Type, usage);

            AddMembersToContext(asset);
        }

        public static RenderingContext BuildContext(CUISession session, StructureInformation asset)
        {
            var context = new RenderingContext
            {
                Game = session.Game,
                Blocks = session.Repository.GetAllFastFileBlocks().ToList()
            };

            context.MakeAsset(asset);

            return context;
        }
    }
}