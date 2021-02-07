#pragma once
#include <vector>

class IDataRepository
{
public:
    virtual ~IDataRepository() = default;

    /*
     * IEnumerable<DataTypeEnum> GetAllEnums();
        IEnumerable<DataTypeStruct> GetAllStructs();
        IEnumerable<DataTypeUnion> GetAllUnions();
        IEnumerable<DataTypeTypedef> GetAllTypedefs();
        IEnumerable<StructureInformation> GetAllStructureInformation();
        IEnumerable<FastFileBlock> GetAllFastFileBlocks();
     */
    const std::vector<>& 
};
