using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Domain.Information;

namespace ZoneCodeGenerator.Persistence
{
    class InMemoryDataRepository : IDataRepository
    {
        private readonly List<DataTypeTypedef> typedefs;
        private readonly List<DataTypeEnum> enums;
        private readonly List<DataTypeStruct> structs;
        private readonly List<DataTypeUnion> unions;
        private readonly List<StructureInformation> structureInformationList;
        private readonly List<FastFileBlock> blocks;

        public InMemoryDataRepository()
        {
            typedefs = new List<DataTypeTypedef>();
            enums = new List<DataTypeEnum>();
            structs = new List<DataTypeStruct>();
            unions = new List<DataTypeUnion>();
            structureInformationList = new List<StructureInformation>();
            blocks = new List<FastFileBlock>();
        }

        public void Add(DataTypeEnum data)
        {
            enums.Add(data);
        }

        public void Add(DataTypeTypedef data)
        {
            typedefs.Add(data);
        }

        public void Add(FastFileBlock block)
        {
            blocks.Add(block);
        }

        public void Add(DataTypeStruct data)
        {
            structs.Add(data);
        }

        public void Add(DataTypeUnion data)
        {
            unions.Add(data);
        }

        public IEnumerable<DataTypeEnum> GetAllEnums()
        {
            return enums.AsEnumerable();
        }

        public IEnumerable<DataTypeStruct> GetAllStructs()
        {
            return structs.AsEnumerable();
        }

        public IEnumerable<DataTypeTypedef> GetAllTypedefs()
        {
            return typedefs.AsEnumerable();
        }

        public IEnumerable<StructureInformation> GetAllStructureInformation()
        {
            return structureInformationList.AsEnumerable();
        }

        public IEnumerable<FastFileBlock> GetAllFastFileBlocks()
        {
            return blocks.AsEnumerable();
        }

        public IEnumerable<DataTypeUnion> GetAllUnions()
        {
            return unions.AsEnumerable();
        }

        public DataType GetDataTypeByName(string name)
        {
            DataType data = enums.Find(_enum => _enum.FullName.Equals(name));
            if (data != null)
                return data;

            data = structs.Find(_struct => _struct.FullName.Equals(name));
            if (data != null)
                return data;

            data = unions.Find(_struct => _struct.FullName.Equals(name));
            if (data != null)
                return data;

            data = typedefs.Find(typeDef => typeDef.FullName.Equals(name));

            return data;
        }

        public StructureInformation GetInformationFor(DataTypeWithMembers dataTypeWithMembers)
        {
            var information = structureInformationList.Find(listEntry => listEntry.Type == dataTypeWithMembers);

            if (information == null)
            {
                information = new StructureInformation(dataTypeWithMembers);
                structureInformationList.Add(information);

                foreach (var member in dataTypeWithMembers.Members)
                {
                    StructureInformation memberStructureInformation = null;

                    var memberDataType = member.VariableType.Type;

                    while (memberDataType is DataTypeTypedef typedef)
                    {
                        memberDataType = typedef.TypeDefinition.Type;
                    }

                    if (memberDataType is DataTypeWithMembers memberType)
                    {
                        memberStructureInformation = GetInformationFor(memberType);
                    }

                    information.OrderedMembers.Add(new MemberInformation(member, memberStructureInformation));
                }
            }

            return information;
        }
    }
}
