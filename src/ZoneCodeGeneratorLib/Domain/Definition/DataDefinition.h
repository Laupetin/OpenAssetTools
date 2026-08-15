#pragma once

#include "Domain/WordSize.h"

#include <string>

enum class DataDefinitionType
{
    STRUCT,
    UNION,
    ENUM,
    TYPEDEF,
    BASE_TYPE,
    FORWARD_DECLARATION
};

class DataDefinition
{
public:
    DataDefinition() = default;
    DataDefinition(std::string _namespace, std::string name);

    virtual ~DataDefinition() = default;
    DataDefinition(const DataDefinition& other) = default;
    DataDefinition(DataDefinition&& other) noexcept = default;
    DataDefinition& operator=(const DataDefinition& other) = default;
    DataDefinition& operator=(DataDefinition&& other) noexcept = default;

    [[nodiscard]] virtual DataDefinitionType GetType() const = 0;
    [[nodiscard]] virtual unsigned GetAlignment(WordSize wordSize) const = 0;
    [[nodiscard]] virtual bool GetForceAlignment() const = 0;
    [[nodiscard]] virtual unsigned GetSize(WordSize wordSize) const = 0;

    [[nodiscard]] std::string GetFullName() const;

    std::string m_namespace;
    std::string m_name;
};
