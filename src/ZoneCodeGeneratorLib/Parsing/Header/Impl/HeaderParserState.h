#pragma once

#include "Domain/Definition/BaseTypeDefinition.h"
#include "Domain/Definition/DataDefinition.h"
#include "Domain/Definition/EnumMember.h"
#include "Domain/Definition/ForwardDeclaration.h"
#include "Parsing/Header/Block/IHeaderBlock.h"
#include "Parsing/IPackValueSupplier.h"
#include "Persistence/IDataRepository.h"
#include "Utils/ClassUtils.h"
#include "Utils/NamespaceBuilder.h"

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

class IHeaderBlock;

class HeaderParserState
{
public:
    explicit HeaderParserState(const IPackValueSupplier* packValueSupplier);

    _NODISCARD IHeaderBlock* GetBlock() const;
    void PushBlock(std::unique_ptr<IHeaderBlock> block);
    void PopBlock();

    void AddDataType(std::unique_ptr<DataDefinition> definition);
    void AddForwardDeclaration(std::unique_ptr<ForwardDeclaration> forwardDeclaration);

    const DataDefinition* FindType(const std::string& typeName);
    EnumMember* FindEnumMember(const std::string& enumMemberName);

    bool SaveToRepository(IDataRepository* repository);

    const IPackValueSupplier* const m_pack_value_supplier;
    NamespaceBuilder m_namespace;

private:
    void AddBaseDataType(const BaseTypeDefinition* baseType);
    bool ResolveForwardDeclarations();
    static bool ReplaceForwardDeclarationsInStruct(StructDefinition* structDefinition);
    static bool ReplaceForwardDeclarationsInTypedef(TypedefDefinition* typedefDefinition);
    static bool ReplaceForwardDeclarationsInUnion(UnionDefinition* unionDefinition);
    bool ReplaceForwardDeclarationsInDefinitions();
    bool MoveDefinitionsToRepository(IDataRepository* repository);

    std::vector<std::unique_ptr<DataDefinition>> m_header_definitions;

    std::stack<std::unique_ptr<IHeaderBlock>> m_blocks;
    std::unordered_map<std::string, const DataDefinition*> m_definitions;
    std::unordered_map<std::string, EnumMember*> m_enum_members;
    std::unordered_map<std::string, std::unique_ptr<ForwardDeclaration>> m_forward_declarations;
};
