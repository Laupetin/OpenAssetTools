using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.C_Header.Impl
{
    class HeaderParserState : IHeaderParserState
    {
        private const int DefaultPack = 8;
        
        private readonly Block defaultBlock;
        private readonly Stack<int> packStack;
        private readonly Stack<Block> blockStack;

        private readonly Dictionary<string, DataType> dataTypes;
        private readonly Dictionary<string, ForwardDeclaration> forwardDeclarations;

        public Namespace CurrentNamespace { get; }

        public HeaderParserState()
        {
            defaultBlock = new BlockNone(this);
            packStack = new Stack<int>();
            blockStack = new Stack<Block>();

            dataTypes = new Dictionary<string, DataType>();
            forwardDeclarations = new Dictionary<string, ForwardDeclaration>();
            CurrentNamespace = new Namespace();
        }

        public int CurrentPack => packStack.Count == 0 ? DefaultPack : packStack.Peek();

        public Block CurrentBlock => blockStack.Count == 0 ? defaultBlock : blockStack.Peek();

        public Block PreviousBlock => blockStack.Count < 2 ? defaultBlock : blockStack.ElementAt(1);

        public void PushPack(int pack)
        {
            packStack.Push(pack);
        }

        public int PopPack()
        {
            return packStack.Pop();
        }

        public void PushBlock(Block block)
        {
            block.OnOpen();
            blockStack.Push(block);
        }

        public Block PopBlock()
        {
            var block = blockStack.Pop();

            block.OnClose();
            CurrentBlock.OnChildBlockClose(block);

            return block;
        }

        public void AddDataType(DataType dataType)
        {
            if (dataTypes.ContainsKey(dataType.FullName))
                throw new ParserException($"Type '{dataType.FullName}' is already defined.");

            dataTypes.Add(dataType.FullName, dataType);
        }

        public void AddForwardDeclaration(ForwardDeclaration forwardDeclaration)
        {
            if (forwardDeclarations.ContainsKey(forwardDeclaration.FullName))
            {
                var existingForwardDeclaration = forwardDeclarations[forwardDeclaration.FullName];

                if(existingForwardDeclaration.Type != forwardDeclaration.Type)
                    throw new ParserException($"Forward declaration of type '{forwardDeclaration.FullName}' already done with different type.");
            }
            else
            {
                forwardDeclarations.Add(forwardDeclaration.FullName, forwardDeclaration);
            }
        }

        public DataType FindType(string typename)
        {
            var currentNamespaceTypename = Namespace.Combine(CurrentNamespace, typename);

            var baseType = DataTypeBaseType.BASE_TYPES.FirstOrDefault(databaseBaseType => databaseBaseType.Name.Equals(typename));

            if (baseType != null)
                return baseType;

            if (dataTypes.ContainsKey(typename))
            {
                return dataTypes[typename];
            }
            
            if (dataTypes.ContainsKey(currentNamespaceTypename))
            {
                return dataTypes[currentNamespaceTypename];
            }

            if (forwardDeclarations.ContainsKey(typename))
            {
                return forwardDeclarations[typename];
            }

            if (forwardDeclarations.ContainsKey(currentNamespaceTypename))
            {
                return forwardDeclarations[currentNamespaceTypename];
            }

            return null;
        }

        public EnumMember FindEnumMember(string enumMemberName)
        {
            foreach(var block in blockStack)
            {
                if (!(block is BlockEnum blockEnum)) continue;

                var foundMember = blockEnum.FindMember(enumMemberName);

                if (foundMember != null)
                    return foundMember;
            }

            foreach (var dataType in dataTypes)
            {
                if (!(dataType.Value is DataTypeEnum _enum))
                    continue;

                var foundMember = _enum.Members.Find(member => member.Name.Equals(enumMemberName));

                if (foundMember != null)
                    return foundMember;
            }

            return null;
        }

        private void ResolveForwardDeclarations()
        {
            foreach(var forwardDeclaration in forwardDeclarations.Values)
            {
                var foundType = FindType(forwardDeclaration.FullName);

                if (foundType == null || foundType is ForwardDeclaration)
                    throw new ParserException($"Type {forwardDeclaration.FullName} was declared but not defined.");

                forwardDeclaration.ForwardedType = foundType;
            }

            foreach (var dataType in dataTypes.Values)
            {
                if (!(dataType is DataTypeWithMembers dataTypeWithMembers)) continue;

                foreach (var variable in dataTypeWithMembers.Members)
                {
                    if (variable.VariableType.Type is ForwardDeclaration forwardDeclaration)
                        variable.VariableType.Type = forwardDeclaration.ForwardedType;
                }
            }
        }

        public void FinishAndSaveTo(IDataRepository dataRepository)
        {
            if(blockStack.Count > 0)
                throw new ParserException($"Parsing finished but {blockStack.Count} blocks were not closed.");

            ResolveForwardDeclarations();

            foreach (var dataType in dataTypes.Values)
            {
                switch (dataType)
                {
                    case DataTypeEnum _enum:
                        dataRepository.Add(_enum);
                        break;
                    case DataTypeStruct _struct:
                        _struct.FinalizeDataType();
                        dataRepository.Add(_struct);
                        break;
                    case DataTypeTypedef typedef:
                        dataRepository.Add(typedef);
                        break;
                    case DataTypeUnion union:
                        union.FinalizeDataType();
                        dataRepository.Add(union);
                        break;
                    default:
                        throw new ArgumentOutOfRangeException(nameof(dataType));
                }
            }
        }

        public IEnumerable<ITokenTest<IHeaderParserState>> GetTests()
        {
            return CurrentBlock.GetAvailableTests();
        }
    }
}
