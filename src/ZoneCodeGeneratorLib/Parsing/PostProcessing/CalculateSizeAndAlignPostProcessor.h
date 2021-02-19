#pragma once

#include "IPostProcessor.h"

class CalculateSizeAndAlignPostProcessor final : public IPostProcessor
{
    static unsigned GetPointerSizeForArchitecture(Architecture architecture);
    static bool CalculateAlign(IDataRepository* repository, TypeDeclaration* declaration);
    static bool CalculateAlign(IDataRepository* repository, DefinitionWithMembers* definition);
    static bool CalculateSize(IDataRepository* repository, TypeDeclaration* declaration);
    static bool CalculateSize(IDataRepository* repository, StructDefinition* definition);
    static bool CalculateSize(IDataRepository* repository, UnionDefinition* definition);
    static bool CalculateFields(IDataRepository* repository, TypeDeclaration* declaration);
    static bool CalculateFields(IDataRepository* repository, StructDefinition* structDefinition);
    static bool CalculateFields(IDataRepository* repository, UnionDefinition* unionDefinition);
    static bool CalculateFieldsIfNecessary(IDataRepository* repository, const DataDefinition* definition);

public:
    bool PostProcess(IDataRepository* repository) override;
};
