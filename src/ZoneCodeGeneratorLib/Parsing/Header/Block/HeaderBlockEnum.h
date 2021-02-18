#pragma once

#include "IHeaderBlock.h"
#include "IHeaderBlockNameHolder.h"
#include "IHeaderBlockVariableDefining.h"
#include "Domain/Definition/BaseTypeDefinition.h"
#include "Domain/Definition/EnumDefinition.h"

class HeaderBlockEnum final : public IHeaderBlock, public IHeaderBlockNameHolder, public IHeaderBlockVariableDefining
{
    std::string m_namespace;
    std::string m_type_name;
    const BaseTypeDefinition* m_parent_type;
    bool m_is_typedef;
    std::vector<std::unique_ptr<EnumMember>> m_members;
    EnumDefinition* m_enum_definition;

    std::string m_variable_name;

public:
    HeaderBlockEnum(std::string typeName, const BaseTypeDefinition* parentType, bool isTypeDef);

    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen(HeaderParserState* state) override;
    void OnClose(HeaderParserState* state) override;
    void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) override;

    void AddEnumMember(std::unique_ptr<EnumMember> enumMember);

    void SetBlockName(const TokenPos& nameTokenPos, std::string name) override;
    bool IsDefiningVariable() override;
    DataDefinition* GetVariableType() override;
    std::string GetVariableName() override;
};
