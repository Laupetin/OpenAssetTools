#pragma once

#include "IHeaderBlock.h"
#include "IHeaderBlockNameHolder.h"
#include "IHeaderBlockVariableDefining.h"
#include "IHeaderBlockVariableHolder.h"

#include <memory>
#include <string>
#include <vector>

class HeaderBlockUnion final : public IHeaderBlock, public IHeaderBlockNameHolder, public IHeaderBlockVariableDefining, public IHeaderBlockVariableHolder
{
public:
    HeaderBlockUnion(std::string name, bool isTypedef);

    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen(HeaderParserState* state) override;
    void OnClose(HeaderParserState* state) override;
    void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) override;

    void AddVariable(std::shared_ptr<Variable> variable) override;
    void SetCustomAlignment(int alignment);
    void Inherit(const StructDefinition* parentStruct);

    void SetBlockName(const TokenPos& nameTokenPos, std::string name) override;
    bool IsDefiningVariable() override;
    DataDefinition* GetVariableType() override;
    std::string GetVariableName() override;

private:
    std::string m_namespace;
    std::string m_type_name;
    std::vector<std::shared_ptr<Variable>> m_members;
    UnionDefinition* m_union_definition;

    int m_custom_alignment;

    bool m_is_typedef;
    bool m_has_custom_align;
    bool m_is_anonymous;

    std::string m_variable_name;
};
