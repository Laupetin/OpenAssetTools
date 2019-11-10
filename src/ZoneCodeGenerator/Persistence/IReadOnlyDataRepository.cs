using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Domain.Information;

namespace ZoneCodeGenerator.Persistence
{
    interface IReadOnlyDataRepository
    {
        IEnumerable<DataTypeEnum> GetAllEnums();
        IEnumerable<DataTypeStruct> GetAllStructs();
        IEnumerable<DataTypeUnion> GetAllUnions();
        IEnumerable<DataTypeTypedef> GetAllTypedefs();
        IEnumerable<StructureInformation> GetAllStructureInformation();
        IEnumerable<FastFileBlock> GetAllFastFileBlocks();

        DataType GetDataTypeByName(string name);
        StructureInformation GetInformationFor(DataTypeWithMembers dataTypeWithMembers);
    }
}
