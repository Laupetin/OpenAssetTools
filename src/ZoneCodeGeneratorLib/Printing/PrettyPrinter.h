#pragma once

#include "Persistence/IDataRepository.h"

#include <ostream>

class PrettyPrinter
{
    std::ostream& m_stream;
    const IDataRepository* const m_repository;

    void PrintSeparator() const;
    void PrintVariablePointerToArray(Variable* variable) const;
    void PrintVariableArrayOfPointers(Variable* variable) const;
    void PrintVariable(Variable* variable) const;
    void PrintTypedefPointerToArray(TypedefDefinition* typedefDefinition) const;
    void PrintTypedefArrayOfPointers(TypedefDefinition* typedefDefinition) const;

    void PrintEnums() const;
    void PrintStructs() const;
    void PrintUnions() const;
    void PrintTypedefs() const;

public:
    PrettyPrinter(std::ostream& stream, const IDataRepository* repository);

    void PrintAll() const;
};
