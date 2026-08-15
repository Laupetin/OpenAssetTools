#include "CalculateSizeAndAlignPostProcessor.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Utils/Alignment.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>

namespace
{
    constexpr auto CLEAR_FLAGS_MASK = ~(DefinitionWithMembers::FLAG_FIELDS_CALCULATING | DefinitionWithMembers::FLAG_FIELDS_CALCULATED);

    bool CalculateFieldsIfNecessary(IDataRepository* repository, const DataDefinition* definition, WordSize wordSize);
    bool CalculateFields(IDataRepository* repository, TypeDeclaration* declaration, WordSize wordSize);

    bool CalculateAlign(IDataRepository* repository, TypeDeclaration* declaration, const WordSize wordSize)
    {
        auto hasPointerModifier = false;
        for (const auto& declarationModifier : declaration->m_declaration_modifiers)
        {
            if (declarationModifier->GetType() == DeclarationModifierType::POINTER)
            {
                hasPointerModifier = true;
                break;
            }
        }

        if (hasPointerModifier)
        {
            declaration->m_alignment[std::to_underlying(wordSize)] = GetPointerSizeForWordSize(wordSize);
        }
        else
        {
            if (!CalculateFieldsIfNecessary(repository, declaration->m_type, wordSize))
                return false;

            declaration->m_alignment[std::to_underlying(wordSize)] = declaration->m_type->GetAlignment(wordSize);
            if (declaration->m_type->GetForceAlignment())
                declaration->m_flags |= TypeDeclaration::FLAG_ALIGNMENT_FORCED;
        }

        return true;
    }

    bool CalculateAlign(IDataRepository* repository, DefinitionWithMembers* definition, const WordSize wordSize)
    {
        if (!definition->GetForceAlignment())
        {
            auto& definitionAlignment = definition->m_alignment[std::to_underlying(wordSize)];
            definitionAlignment = 0;
            for (const auto& member : definition->m_members)
            {
                if (!CalculateFields(repository, member->m_type_declaration.get(), wordSize))
                    return false;

                const auto memberAlignment = member->GetAlignment(wordSize);
                definitionAlignment = std::max(memberAlignment, definitionAlignment);
            }
        }

        return true;
    }

    bool CalculateSize(IDataRepository* repository, TypeDeclaration* declaration, const WordSize wordSize)
    {
        if (declaration->m_declaration_modifiers.empty())
        {
            if (!CalculateFieldsIfNecessary(repository, declaration->m_type, wordSize))
                return false;
            declaration->m_size[std::to_underlying(wordSize)] = declaration->m_type->GetSize(wordSize);
        }
        else
        {
            auto currentSize = 0u;

            // If the first modifier is a pointer we do not need the actual type size
            if (declaration->m_declaration_modifiers.back()->GetType() != DeclarationModifierType::POINTER)
            {
                if (!CalculateFieldsIfNecessary(repository, declaration->m_type, wordSize))
                    return false;
                currentSize = declaration->m_type->GetSize(wordSize);
            }

            for (auto i = declaration->m_declaration_modifiers.size(); i > 0; i--)
            {
                const auto& declarationModifier = declaration->m_declaration_modifiers[i - 1];

                switch (declarationModifier->GetType())
                {
                case DeclarationModifierType::POINTER:
                    currentSize = GetPointerSizeForWordSize(wordSize);
                    break;

                case DeclarationModifierType::ARRAY:
                    currentSize *= dynamic_cast<ArrayDeclarationModifier*>(declarationModifier.get())->m_size;
                    break;
                }
            }

            declaration->m_size[std::to_underlying(wordSize)] = currentSize;
        }

        return true;
    }

    bool CalculateSize(IDataRepository* repository, StructDefinition* definition, const WordSize wordSize)
    {
        auto& definitionSize = definition->m_size[std::to_underlying(wordSize)];
        definitionSize = 0;
        auto currentBitOffset = 0u;

        for (const auto& member : definition->m_members)
        {
            if (!CalculateFields(repository, member->m_type_declaration.get(), wordSize))
                return false;

            if (member->m_type_declaration->m_has_custom_bit_size)
            {
                member->m_offset = definitionSize + currentBitOffset / 8;
                currentBitOffset += member->m_type_declaration->m_custom_bit_size;
            }
            else
            {
                if (currentBitOffset > 0)
                {
                    currentBitOffset = utils::Align(currentBitOffset, 8u);
                    definitionSize += currentBitOffset / 8;
                    currentBitOffset = 0;
                }

                definitionSize =
                    utils::Align(definitionSize,
                                 member->GetForceAlignment() ? member->GetAlignment(wordSize) : std::min(member->GetAlignment(wordSize), definition->m_pack));

                member->m_offset = definitionSize;

                definitionSize += member->m_type_declaration->GetSize(wordSize);
            }
        }

        if (currentBitOffset > 0)
        {
            currentBitOffset = utils::Align(currentBitOffset, 8u);
            definitionSize += currentBitOffset / 8;
        }

        definitionSize = utils::Align(definitionSize, definition->m_alignment[std::to_underlying(wordSize)]);

        return true;
    }

    bool CalculateSize(IDataRepository* repository, UnionDefinition* definition, const WordSize wordSize)
    {
        auto& definitionSize = definition->m_size[std::to_underlying(wordSize)];
        definitionSize = 0;

        for (const auto& member : definition->m_members)
        {
            if (!CalculateFields(repository, member->m_type_declaration.get(), wordSize))
                return false;

            member->m_offset = 0;

            const auto memberSize = member->m_type_declaration->GetSize(wordSize);
            if (memberSize > definitionSize)
                definitionSize = memberSize;
        }

        definitionSize = utils::Align(definitionSize, definition->m_alignment[std::to_underlying(wordSize)]);

        return true;
    }

    bool CalculateFields(IDataRepository* repository, TypeDeclaration* declaration, const WordSize wordSize)
    {
        if (declaration->m_flags & TypeDeclaration::FLAG_FIELDS_CALCULATED)
            return true;

        if (!CalculateAlign(repository, declaration, wordSize) || !CalculateSize(repository, declaration, wordSize))
            return false;

        declaration->m_flags |= TypeDeclaration::FLAG_FIELDS_CALCULATED;
        return true;
    }

    bool CalculateFields(IDataRepository* repository, StructDefinition* structDefinition, const WordSize wordSize)
    {
        if (structDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATED)
            return true;
        if (structDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATING)
        {
            con::error("Detected circular dependency:");
            return false;
        }

        structDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATING;

        if (!CalculateAlign(repository, structDefinition, wordSize) || !CalculateSize(repository, structDefinition, wordSize))
        {
            return false;
        }

        structDefinition->m_flags &= ~DefinitionWithMembers::FLAG_FIELDS_CALCULATING;
        structDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATED;

        return true;
    }

    bool CalculateFields(IDataRepository* repository, UnionDefinition* unionDefinition, const WordSize wordSize)
    {
        if (unionDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATED)
            return true;
        if (unionDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATING)
        {
            con::error("Detected circular dependency:");
            return false;
        }

        unionDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATING;

        if (!CalculateAlign(repository, unionDefinition, wordSize) || !CalculateSize(repository, unionDefinition, wordSize))
        {
            return false;
        }

        unionDefinition->m_flags &= ~DefinitionWithMembers::FLAG_FIELDS_CALCULATING;
        unionDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATED;

        return true;
    }

    bool CalculateFieldsIfNecessary(IDataRepository* repository, const DataDefinition* definition, const WordSize wordSize)
    {
        if (definition->GetType() == DataDefinitionType::STRUCT)
        {
            // We can do a const cast here because the only reason that field is const anyway is because it could be a base type
            return CalculateFields(repository, dynamic_cast<StructDefinition*>(const_cast<DataDefinition*>(definition)), wordSize);
        }

        if (definition->GetType() == DataDefinitionType::UNION)
        {
            // We can do a const cast here because the only reason that field is const anyway is because it could be a base type
            return CalculateFields(repository, dynamic_cast<UnionDefinition*>(const_cast<DataDefinition*>(definition)), wordSize);
        }

        if (definition->GetType() == DataDefinitionType::TYPEDEF)
        {
            // We can do a const cast here because the only reason that field is const anyway is because it could be a base type
            return CalculateFields(repository, dynamic_cast<TypedefDefinition*>(const_cast<DataDefinition*>(definition))->m_type_declaration.get(), wordSize);
        }

        return true;
    }

    void ClearDefinitionFlags(DefinitionWithMembers& definition)
    {
        definition.m_flags &= CLEAR_FLAGS_MASK;

        for (const auto& member : definition.m_members)
            member->m_type_declaration->m_flags &= CLEAR_FLAGS_MASK;
    }

    void ClearDefinitionFlags(IDataRepository& repository)
    {
        for (auto* structDefinition : repository.GetAllStructs())
            ClearDefinitionFlags(*structDefinition);

        for (auto* unionDefinition : repository.GetAllUnions())
            ClearDefinitionFlags(*unionDefinition);

        for (auto* typedefDeclaration : repository.GetAllTypedefs())
            typedefDeclaration->m_type_declaration->m_flags &= CLEAR_FLAGS_MASK;
    }
} // namespace

bool CalculateSizeAndAlignPostProcessor::PostProcess(IDataRepository* repository)
{
    for (auto wordSizeNum = 0; wordSizeNum < WORD_SIZE_COUNT; wordSizeNum++)
    {
        const auto wordSize = static_cast<WordSize>(wordSizeNum);
        for (auto* structDefinition : repository->GetAllStructs())
        {
            if (!CalculateFields(repository, structDefinition, wordSize))
                return false;
        }

        for (auto* unionDefinition : repository->GetAllUnions())
        {
            if (!CalculateFields(repository, unionDefinition, wordSize))
                return false;
        }

        for (auto* typedefDeclaration : repository->GetAllTypedefs())
        {
            if (!CalculateFields(repository, typedefDeclaration->m_type_declaration.get(), wordSize))
                return false;
        }

        if (wordSizeNum + 1 < WORD_SIZE_COUNT)
            ClearDefinitionFlags(*repository);
    }

    return true;
}
