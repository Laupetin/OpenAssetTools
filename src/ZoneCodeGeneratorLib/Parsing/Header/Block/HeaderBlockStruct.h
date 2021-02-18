#pragma once

#include <string>

#include "IHeaderBlock.h"
#include "IHeaderBlockNameHolder.h"
#include "IHeaderBlockVariableDefining.h"
#include "Domain/Definition/StructDefinition.h"
#include "Domain/Definition/Variable.h"

class HeaderBlockStruct final : public IHeaderBlock, public IHeaderBlockNameHolder, public IHeaderBlockVariableDefining
{
    std::string m_namespace;
    std::string m_type_name;
    std::vector<std::shared_ptr<Variable>> m_members;
    StructDefinition* m_struct_definition;

    int m_custom_alignment;

    bool m_is_typedef;
    bool m_has_custom_align;

    std::string m_variable_name;

public:
    HeaderBlockStruct(std::string name, bool isTypedef);

    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen(HeaderParserState* state) override;
    void OnClose(HeaderParserState* state) override;
    void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) override;

    void AddVariable(std::shared_ptr<Variable> variable);
    void SetCustomAlignment(int alignment);
    void Inherit(const StructDefinition* parentStruct);

    void SetBlockName(const TokenPos& nameTokenPos, std::string name) override;
    bool IsDefiningVariable() override;
    DataDefinition* GetVariableType() override;
    std::string GetVariableName() override;
};
