#pragma once
#include <string>

#include "Utils/ClassUtils.h"

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

    std::string m_namespace;
    std::string m_name;
    
    _NO_DISCARD virtual DataDefinitionType GetType() const = 0;
    _NO_DISCARD virtual unsigned GetAlignment() = 0;
    _NO_DISCARD virtual bool GetForceAlignment() = 0;
    _NO_DISCARD virtual unsigned GetSize() = 0;

    _NO_DISCARD std::string GetFullName() const;
};
