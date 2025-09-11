#include "CalculateSizeAndAlignPostProcessor.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Utils/Alignment.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstdint>
#include <iostream>

namespace
{
    bool CalculateFieldsIfNecessary(IDataRepository* repository, const DataDefinition* definition);
    bool CalculateFields(IDataRepository* repository, TypeDeclaration* declaration);

    bool CalculateAlign(IDataRepository* repository, TypeDeclaration* declaration)
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
            declaration->m_alignment = GetPointerSizeForArchitecture(repository->GetArchitecture());
        }
        else
        {
            if (!CalculateFieldsIfNecessary(repository, declaration->m_type))
                return false;
            declaration->m_alignment = declaration->m_type->GetAlignment();
            if (declaration->m_type->GetForceAlignment())
                declaration->m_flags |= TypeDeclaration::FLAG_ALIGNMENT_FORCED;
        }

        return true;
    }

    bool CalculateAlign(IDataRepository* repository, DefinitionWithMembers* definition)
    {
        if (definition->m_has_alignment_override)
        {
            definition->m_flags |= DefinitionWithMembers::FLAG_ALIGNMENT_FORCED;
            definition->m_alignment = definition->m_alignment_override;
        }
        else
        {
            definition->m_alignment = 0;
            for (const auto& member : definition->m_members)
            {
                if (!CalculateFields(repository, member->m_type_declaration.get()))
                    return false;

                const auto memberAlignment = member->GetAlignment();
                if (memberAlignment > definition->m_alignment)
                    definition->m_alignment = memberAlignment;
            }
        }

        return true;
    }

    bool CalculateSize(IDataRepository* repository, TypeDeclaration* declaration)
    {
        if (declaration->m_declaration_modifiers.empty())
        {
            if (!CalculateFieldsIfNecessary(repository, declaration->m_type))
                return false;
            declaration->m_size = declaration->m_type->GetSize();
        }
        else
        {
            auto currentSize = 0u;

            // If the first modifier is a pointer we do not need the actual type size
            if (declaration->m_declaration_modifiers.back()->GetType() != DeclarationModifierType::POINTER)
            {
                if (!CalculateFieldsIfNecessary(repository, declaration->m_type))
                    return false;
                currentSize = declaration->m_type->GetSize();
            }

            for (auto i = declaration->m_declaration_modifiers.size(); i > 0; i--)
            {
                const auto& declarationModifier = declaration->m_declaration_modifiers[i - 1];

                switch (declarationModifier->GetType())
                {
                case DeclarationModifierType::POINTER:
                    currentSize = GetPointerSizeForArchitecture(repository->GetArchitecture());
                    break;

                case DeclarationModifierType::ARRAY:
                    currentSize *= dynamic_cast<ArrayDeclarationModifier*>(declarationModifier.get())->m_size;
                    break;
                }
            }

            declaration->m_size = currentSize;
        }

        return true;
    }

    bool CalculateSize(IDataRepository* repository, StructDefinition* definition)
    {
        definition->m_size = 0;
        auto currentBitOffset = 0u;

        for (const auto& member : definition->m_members)
        {
            if (!CalculateFields(repository, member->m_type_declaration.get()))
                return false;

            if (member->m_type_declaration->m_has_custom_bit_size)
            {
                member->m_offset = definition->m_size + currentBitOffset / 8;
                currentBitOffset += member->m_type_declaration->m_custom_bit_size;
            }
            else
            {
                if (currentBitOffset > 0)
                {
                    currentBitOffset = utils::Align(currentBitOffset, 8u);
                    definition->m_size += currentBitOffset / 8;
                    currentBitOffset = 0;
                }

                definition->m_size = utils::Align(definition->m_size,
                                                  member->GetForceAlignment() ? member->GetAlignment() : std::min(member->GetAlignment(), definition->m_pack));

                member->m_offset = definition->m_size;

                definition->m_size += member->m_type_declaration->GetSize();
            }
        }

        if (currentBitOffset > 0)
        {
            currentBitOffset = utils::Align(currentBitOffset, 8u);
            definition->m_size += currentBitOffset / 8;
        }

        definition->m_size = utils::Align(definition->m_size, definition->m_alignment);

        return true;
    }

    bool CalculateSize(IDataRepository* repository, UnionDefinition* definition)
    {
        definition->m_size = 0;

        for (const auto& member : definition->m_members)
        {
            if (!CalculateFields(repository, member->m_type_declaration.get()))
                return false;

            member->m_offset = 0;

            const auto memberSize = member->m_type_declaration->GetSize();
            if (memberSize > definition->m_size)
                definition->m_size = memberSize;
        }

        definition->m_size = utils::Align(definition->m_size, definition->m_alignment);

        return true;
    }

    bool CalculateFields(IDataRepository* repository, TypeDeclaration* declaration)
    {
        if (declaration->m_flags & TypeDeclaration::FLAG_FIELDS_CALCULATED)
            return true;

        if (!CalculateAlign(repository, declaration) || !CalculateSize(repository, declaration))
            return false;

        declaration->m_flags |= TypeDeclaration::FLAG_FIELDS_CALCULATED;
        return true;
    }

    bool CalculateFields(IDataRepository* repository, StructDefinition* structDefinition)
    {
        if (structDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATED)
            return true;
        if (structDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATING)
        {
            con::error("Detected circular dependency:");
            return false;
        }

        structDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATING;

        if (!CalculateAlign(repository, structDefinition) || !CalculateSize(repository, structDefinition))
        {
            return false;
        }

        structDefinition->m_flags &= ~DefinitionWithMembers::FLAG_FIELDS_CALCULATING;
        structDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATED;

        return true;
    }

    bool CalculateFields(IDataRepository* repository, UnionDefinition* unionDefinition)
    {
        if (unionDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATED)
            return true;
        if (unionDefinition->m_flags & DefinitionWithMembers::FLAG_FIELDS_CALCULATING)
        {
            con::error("Detected circular dependency:");
            return false;
        }

        unionDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATING;

        if (!CalculateAlign(repository, unionDefinition) || !CalculateSize(repository, unionDefinition))
        {
            return false;
        }

        unionDefinition->m_flags &= ~DefinitionWithMembers::FLAG_FIELDS_CALCULATING;
        unionDefinition->m_flags |= DefinitionWithMembers::FLAG_FIELDS_CALCULATED;

        return true;
    }

    bool CalculateFieldsIfNecessary(IDataRepository* repository, const DataDefinition* definition)
    {
        if (definition->GetType() == DataDefinitionType::STRUCT)
        {
            // We can do a const cast here because the only reason that field is const anyway is because it could be a base type
            return CalculateFields(repository, dynamic_cast<StructDefinition*>(const_cast<DataDefinition*>(definition)));
        }

        if (definition->GetType() == DataDefinitionType::UNION)
        {
            // We can do a const cast here because the only reason that field is const anyway is because it could be a base type
            return CalculateFields(repository, dynamic_cast<UnionDefinition*>(const_cast<DataDefinition*>(definition)));
        }

        if (definition->GetType() == DataDefinitionType::TYPEDEF)
        {
            // We can do a const cast here because the only reason that field is const anyway is because it could be a base type
            return CalculateFields(repository, dynamic_cast<TypedefDefinition*>(const_cast<DataDefinition*>(definition))->m_type_declaration.get());
        }

        return true;
    }
} // namespace

bool CalculateSizeAndAlignPostProcessor::PostProcess(IDataRepository* repository)
{
    if (repository->GetArchitecture() == Architecture::UNKNOWN)
    {
        con::error("You must set an architecture!");
        return false;
    }

    for (auto* structDefinition : repository->GetAllStructs())
    {
        if (!CalculateFields(repository, structDefinition))

            return false;
    }

    for (auto* unionDefinition : repository->GetAllUnions())
    {
        if (!CalculateFields(repository, unionDefinition))

            return false;
    }

    for (auto* typedefDeclaration : repository->GetAllTypedefs())
    {
        if (!CalculateFields(repository, typedefDeclaration->m_type_declaration.get()))
            return false;
    }

    return true;
}
