#include "PrettyPrinter.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Utils/NamespaceBuilder.h"

#include <algorithm>
#include <format>
#include <iostream>

PrettyPrinter::PrettyPrinter(std::ostream& stream, const IDataRepository* repository)
    : m_stream(stream),
      m_repository(repository)
{
}

void PrettyPrinter::PrintSeparator() const
{
    m_stream << "\n==========================================================================================================\n\n";
}

void PrettyPrinter::PrintVariablePointerToArray(Variable* variable) const
{
    const auto& declarationModifiers = variable->m_type_declaration->m_declaration_modifiers;

    std::vector<int> arraySize;
    auto pointerDepth = 0;
    auto modifierOffset = 0u;

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::POINTER)
            break;

        pointerDepth++;
    }

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::ARRAY)
            break;

        arraySize.push_back(dynamic_cast<ArrayDeclarationModifier*>(modifier.get())->m_size);
    }

    m_stream << std::format("  {}(", variable->m_type_declaration->m_type->GetFullName());
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << std::format("{})", variable->m_name);
    for (auto size : arraySize)
        m_stream << std::format("[{}]", size);
    m_stream << "\n";
}

void PrettyPrinter::PrintVariableArrayOfPointers(Variable* variable) const
{
    const auto& declarationModifiers = variable->m_type_declaration->m_declaration_modifiers;

    std::vector<int> arraySize;
    auto pointerDepth = 0;
    auto modifierOffset = 0u;

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::ARRAY)
            break;

        arraySize.push_back(dynamic_cast<ArrayDeclarationModifier*>(modifier.get())->m_size);
    }

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::POINTER)
            break;

        pointerDepth++;
    }

    m_stream << std::format("  {}", variable->m_type_declaration->m_type->GetFullName());
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << std::format(" {}", variable->m_name);
    for (auto size : arraySize)
        m_stream << std::format("[{}]", size);
    m_stream << "\n";
}

void PrettyPrinter::PrintVariable(Variable* variable) const
{
    const auto& declarationModifiers = variable->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
    {
        m_stream << std::format("  {} {}\n", variable->m_type_declaration->m_type->GetFullName(), variable->m_name);
    }
    else
    {
        if (declarationModifiers[0]->GetType() == DeclarationModifierType::POINTER
            && std::ranges::any_of(declarationModifiers,
                                   [](const std::unique_ptr<DeclarationModifier>& modifier)
                                   {
                                       return modifier->GetType() == DeclarationModifierType::ARRAY;
                                   }))
        {
            PrintVariablePointerToArray(variable);
        }
        else
        {
            PrintVariableArrayOfPointers(variable);
        }
    }
}

void PrettyPrinter::PrintTypedefPointerToArray(TypedefDefinition* typedefDefinition) const
{
    const auto& declarationModifiers = typedefDefinition->m_type_declaration->m_declaration_modifiers;

    std::vector<int> arraySize;
    auto pointerDepth = 0;
    auto modifierOffset = 0u;

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::POINTER)
            break;

        pointerDepth++;
    }

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::ARRAY)
            break;

        arraySize.push_back(dynamic_cast<ArrayDeclarationModifier*>(modifier.get())->m_size);
    }

    m_stream << std::format("  {}(", typedefDefinition->m_type_declaration->m_type->GetFullName());
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << std::format("{})", typedefDefinition->m_name);
    for (auto size : arraySize)
        m_stream << std::format("[{}]", size);
    m_stream << "\n";
}

void PrettyPrinter::PrintTypedefArrayOfPointers(TypedefDefinition* typedefDefinition) const
{
    const auto& declarationModifiers = typedefDefinition->m_type_declaration->m_declaration_modifiers;

    std::vector<int> arraySize;
    auto pointerDepth = 0;
    auto modifierOffset = 0u;

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::ARRAY)
            break;

        arraySize.push_back(dynamic_cast<ArrayDeclarationModifier*>(modifier.get())->m_size);
    }

    for (; modifierOffset < declarationModifiers.size(); modifierOffset++)
    {
        const auto& modifier = declarationModifiers[modifierOffset];
        if (modifier->GetType() != DeclarationModifierType::POINTER)
            break;

        pointerDepth++;
    }

    m_stream << std::format("  {}", typedefDefinition->m_type_declaration->m_type->GetFullName());
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << std::format(" {}", typedefDefinition->m_name);
    for (auto size : arraySize)
        m_stream << std::format("[{}]", size);
    m_stream << "\n";
}

void PrettyPrinter::PrintEnums() const
{
    const auto& allEnums = m_repository->GetAllEnums();
    m_stream << std::format("{} enums:\n", allEnums.size());

    for (auto* enumDefinition : allEnums)
    {
        m_stream << std::format(" Name: {}\n", enumDefinition->GetFullName());
        m_stream << std::format(" Alignment: {}\n", enumDefinition->GetAlignment());
        m_stream << std::format(" Size: {}\n", enumDefinition->GetSize());

        for (const auto& enumMember : enumDefinition->m_members)
        {
            m_stream << std::format("  {} = {}\n", enumMember->m_name, enumMember->m_value);
        }

        m_stream << "\n";
    }
}

void PrettyPrinter::PrintStructs() const
{
    const auto& allStructs = m_repository->GetAllStructs();
    m_stream << std::format("{} structs:\n", allStructs.size());

    for (auto* structDefinition : allStructs)
    {
        m_stream << std::format(" Name: {}\n", structDefinition->GetFullName());
        m_stream << std::format(" Alignment: {}\n", structDefinition->GetAlignment());
        m_stream << std::format(" Size: {}\n", structDefinition->GetSize());

        for (const auto& variable : structDefinition->m_members)
        {
            PrintVariable(variable.get());
        }

        m_stream << "\n";
    }
}

void PrettyPrinter::PrintUnions() const
{
    const auto& allUnions = m_repository->GetAllUnions();
    m_stream << std::format("{} unions:\n", allUnions.size());

    for (auto* unionDefinition : allUnions)
    {
        m_stream << std::format(" Name: {}\n", unionDefinition->GetFullName());
        m_stream << std::format(" Alignment: {}\n", unionDefinition->GetAlignment());
        m_stream << std::format(" Size: {}\n", unionDefinition->GetSize());

        for (const auto& variable : unionDefinition->m_members)
        {
            PrintVariable(variable.get());
        }

        m_stream << "\n";
    }
}

void PrettyPrinter::PrintTypedefs() const
{
    const auto& allTypedefs = m_repository->GetAllTypedefs();
    m_stream << std::format("{} typedefs:\n", allTypedefs.size());

    for (auto* typedefDefinition : allTypedefs)
    {
        m_stream << std::format(" Name: {}\n", typedefDefinition->GetFullName());
        m_stream << std::format(" Alignment: {}\n", typedefDefinition->GetAlignment());
        m_stream << std::format(" Size: {}\n", typedefDefinition->GetSize());

        const auto& declarationModifiers = typedefDefinition->m_type_declaration->m_declaration_modifiers;
        if (declarationModifiers.empty())
        {
            m_stream << std::format("  {}\n", typedefDefinition->m_type_declaration->m_type->GetFullName());
        }
        else
        {
            if (declarationModifiers[0]->GetType() == DeclarationModifierType::POINTER
                && std::ranges::any_of(declarationModifiers,
                                       [](const std::unique_ptr<DeclarationModifier>& modifier)
                                       {
                                           return modifier->GetType() == DeclarationModifierType::ARRAY;
                                       }))
            {
                PrintTypedefPointerToArray(typedefDefinition);
            }
            else
            {
                PrintTypedefArrayOfPointers(typedefDefinition);
            }
        }

        m_stream << "\n";
    }
}

void PrettyPrinter::PrintAll() const
{
    PrintEnums();

    PrintSeparator();

    PrintStructs();

    PrintSeparator();

    PrintUnions();

    PrintSeparator();

    PrintTypedefs();
}
