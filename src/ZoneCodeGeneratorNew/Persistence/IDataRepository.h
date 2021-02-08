#pragma once

#include <vector>

#include "Utils/ClassUtils.h"
#include "Domain/Definition/EnumDefinition.h"

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
    _NODISCARD const std::vector<EnumDefinition*>& GetAllEnums() const;
    _NODISCARD const std::vector<EnumDefinition*>& GetAllStructs() const;
    _NODISCARD const std::vector<EnumDefinition*>& GetAllUnions() const;
    _NODISCARD const std::vector<EnumDefinition*>& GetAllTypedefs() const;
    _NODISCARD const std::vector<EnumDefinition*>& GetAllStructureInformation() const;
    _NODISCARD const std::vector<EnumDefinition*>& GetAllFastFileBlocks() const;
};
