#pragma once

#include <memory>
#include <stack>
#include <unordered_map>

#include "Utils/ClassUtils.h"
#include "Domain/Definition/BaseTypeDefinition.h"
#include "Domain/Definition/DataDefinition.h"
#include "Domain/Definition/EnumMember.h"
#include "Domain/Definition/ForwardDeclaration.h"
#include "Parsing/IPackValueSupplier.h"
#include "Utils/NamespaceBuilder.h"
#include "Parsing/Header/Block/IHeaderBlock.h"
#include "Persistence/IDataRepository.h"

class IHeaderBlock;

class HeaderParserState
{
    std::vector<std::unique_ptr<DataDefinition>> m_header_definitions;

    std::stack<std::unique_ptr<IHeaderBlock>> m_blocks;
    std::unordered_map<std::string, const DataDefinition*> m_definitions;
    std::unordered_map<std::string, EnumMember*> m_enum_members;
    std::unordered_map<std::string, std::unique_ptr<ForwardDeclaration>> m_forward_declarations;

    void AddBaseDataType(const BaseTypeDefinition* baseType);

public:
    const IPackValueSupplier* const m_pack_value_supplier;
    NamespaceBuilder m_namespace;

    explicit HeaderParserState(const IPackValueSupplier* packValueSupplier);

    _NODISCARD IHeaderBlock* GetBlock() const;
    void PushBlock(std::unique_ptr<IHeaderBlock> block);
    void PopBlock();

    void AddDataType(std::unique_ptr<DataDefinition> definition);
    void AddForwardDeclaration(std::unique_ptr<ForwardDeclaration> forwardDeclaration);

    const DataDefinition* FindType(const std::string& typeName);
    EnumMember* FindEnumMember(const std::string& enumMemberName);

    void SaveToRepository(IDataRepository* repository);
};
