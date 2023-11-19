#pragma once

#include "Domain/Definition/EnumDefinition.h"
#include "Domain/Definition/StructDefinition.h"
#include "Domain/Definition/TypedefDefinition.h"
#include "Domain/Definition/UnionDefinition.h"
#include "Domain/Environment/Architecture.h"
#include "Domain/FastFile/FastFileBlock.h"
#include "Domain/Information/StructureInformation.h"
#include "Utils/ClassUtils.h"

#include <vector>

class IDataRepository
{
public:
    IDataRepository() = default;
    virtual ~IDataRepository() = default;
    IDataRepository(const IDataRepository& other) = default;
    IDataRepository(IDataRepository&& other) noexcept = default;
    IDataRepository& operator=(const IDataRepository& other) = default;
    IDataRepository& operator=(IDataRepository&& other) noexcept = default;

    virtual void Add(std::unique_ptr<EnumDefinition> enumsDefinition) = 0;
    virtual void Add(std::unique_ptr<StructDefinition> structDefinition) = 0;
    virtual void Add(std::unique_ptr<UnionDefinition> unionDefinition) = 0;
    virtual void Add(std::unique_ptr<TypedefDefinition> typedefDefinition) = 0;
    virtual void Add(std::unique_ptr<StructureInformation> structureInformation) = 0;
    virtual void Add(std::unique_ptr<FastFileBlock> fastFileBlock) = 0;

    _NODISCARD virtual const std::string& GetGameName() const = 0;
    virtual void SetGame(std::string gameName) = 0;
    _NODISCARD virtual Architecture GetArchitecture() const = 0;
    virtual void SetArchitecture(Architecture architecture) = 0;

    _NODISCARD virtual const std::vector<EnumDefinition*>& GetAllEnums() const = 0;
    _NODISCARD virtual const std::vector<StructDefinition*>& GetAllStructs() const = 0;
    _NODISCARD virtual const std::vector<UnionDefinition*>& GetAllUnions() const = 0;
    _NODISCARD virtual const std::vector<TypedefDefinition*>& GetAllTypedefs() const = 0;
    _NODISCARD virtual const std::vector<StructureInformation*>& GetAllStructureInformation() const = 0;
    _NODISCARD virtual const std::vector<const FastFileBlock*>& GetAllFastFileBlocks() const = 0;

    _NODISCARD virtual DataDefinition* GetDataDefinitionByName(const std::string& name) const = 0;
    _NODISCARD virtual StructureInformation* GetInformationFor(const DefinitionWithMembers* definitionWithMembers) const = 0;
    _NODISCARD virtual EnumMember* GetEnumMemberByName(const std::string& name) const = 0;
    _NODISCARD virtual const FastFileBlock* GetFastFileBlockByName(const std::string& name) const = 0;
};
