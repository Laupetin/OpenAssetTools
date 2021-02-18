#include "BaseTypeDefinition.h"

BaseTypeDefinition::BaseTypeDefinition(std::string name, const unsigned size)
    : DataDefinition("", std::move(name)),
      m_size(size)
{
}

DataDefinitionType BaseTypeDefinition::GetType() const
{
    return DataDefinitionType::BASE_TYPE;
}

unsigned BaseTypeDefinition::GetAlignment() const
{
    return m_size;
}

bool BaseTypeDefinition::GetForceAlignment() const
{
    return false;
}

unsigned BaseTypeDefinition::GetSize() const
{
    return m_size;
}

const BaseTypeDefinition* const BaseTypeDefinition::FLOAT = new BaseTypeDefinition("float", 4);
const BaseTypeDefinition* const BaseTypeDefinition::DOUBLE = new BaseTypeDefinition("double", 8);
const BaseTypeDefinition* const BaseTypeDefinition::BOOL = new BaseTypeDefinition("bool", 1);
const BaseTypeDefinition* const BaseTypeDefinition::CHAR = new BaseTypeDefinition("char", 1);
const BaseTypeDefinition* const BaseTypeDefinition::UNSIGNED_CHAR = new BaseTypeDefinition("unsigned char", 1);
const BaseTypeDefinition* const BaseTypeDefinition::SHORT = new BaseTypeDefinition("short", 2);
const BaseTypeDefinition* const BaseTypeDefinition::UNSIGNED_SHORT = new BaseTypeDefinition("unsigned short", 2);
const BaseTypeDefinition* const BaseTypeDefinition::INT = new BaseTypeDefinition("int", 4);
const BaseTypeDefinition* const BaseTypeDefinition::UNSIGNED_INT = new BaseTypeDefinition("unsigned int", 4);
const BaseTypeDefinition* const BaseTypeDefinition::LONG = new BaseTypeDefinition("long", 4);
const BaseTypeDefinition* const BaseTypeDefinition::UNSIGNED_LONG = new BaseTypeDefinition("unsigned long", 4);
const BaseTypeDefinition* const BaseTypeDefinition::LONG_LONG = new BaseTypeDefinition("long long", 8);
const BaseTypeDefinition* const BaseTypeDefinition::UNSIGNED_LONG_LONG = new BaseTypeDefinition("unsigned long long", 8);
const BaseTypeDefinition* const BaseTypeDefinition::VOID = new BaseTypeDefinition("void", 0);
const BaseTypeDefinition* const BaseTypeDefinition::ALL_BASE_TYPES[]
{
    FLOAT,
    DOUBLE,
    BOOL,
    CHAR,
    UNSIGNED_CHAR,
    SHORT,
    UNSIGNED_SHORT,
    INT,
    UNSIGNED_INT,
    LONG,
    UNSIGNED_LONG,
    LONG_LONG,
    UNSIGNED_LONG_LONG,
    VOID
};
const size_t BaseTypeDefinition::ALL_BASE_TYPES_COUNT = _countof(ALL_BASE_TYPES);