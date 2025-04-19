#pragma once
#include "DataDefinition.h"

class BaseTypeDefinition final : public DataDefinition
{
public:
    static const BaseTypeDefinition* const FLOAT;
    static const BaseTypeDefinition* const DOUBLE;
    static const BaseTypeDefinition* const BOOL;
    static const BaseTypeDefinition* const CHAR;
    static const BaseTypeDefinition* const UNSIGNED_CHAR;
    static const BaseTypeDefinition* const SHORT;
    static const BaseTypeDefinition* const UNSIGNED_SHORT;
    static const BaseTypeDefinition* const INT;
    static const BaseTypeDefinition* const UNSIGNED_INT;
    static const BaseTypeDefinition* const LONG;
    static const BaseTypeDefinition* const UNSIGNED_LONG;
    static const BaseTypeDefinition* const LONG_LONG;
    static const BaseTypeDefinition* const UNSIGNED_LONG_LONG;
    static const BaseTypeDefinition* const VOID;

    static const BaseTypeDefinition* const ALL_BASE_TYPES[];
    static const size_t ALL_BASE_TYPES_COUNT;

    [[nodiscard]] DataDefinitionType GetType() const override;
    [[nodiscard]] unsigned GetAlignment() const override;
    [[nodiscard]] bool GetForceAlignment() const override;
    [[nodiscard]] unsigned GetSize() const override;

    const unsigned m_size;

private:
    BaseTypeDefinition(std::string name, unsigned size);
};
