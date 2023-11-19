#include "PrettyPrinter.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Utils/NamespaceBuilder.h"

#include <algorithm>
#include <iostream>

PrettyPrinter::PrettyPrinter(std::ostream& stream, const IDataRepository* repository)
    : m_stream(stream),
      m_repository(repository)
{
}

void PrettyPrinter::PrintSeparator() const
{
    m_stream << std::endl
             << "==========================================================================================================" << std::endl
             << std::endl;
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

    m_stream << "  " << variable->m_type_declaration->m_type->GetFullName() << '(';
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << variable->m_name << ")";
    for (auto size : arraySize)
        m_stream << '[' << size << ']';
    m_stream << std::endl;
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

    m_stream << "  " << variable->m_type_declaration->m_type->GetFullName();
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << " " << variable->m_name;
    for (auto size : arraySize)
        m_stream << '[' << size << ']';
    m_stream << std::endl;
}

void PrettyPrinter::PrintVariable(Variable* variable) const
{
    const auto& declarationModifiers = variable->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
    {
        std::cout << "  " << variable->m_type_declaration->m_type->GetFullName() << " " << variable->m_name << std::endl;
    }
    else
    {
        if (declarationModifiers[0]->GetType() == DeclarationModifierType::POINTER
            && std::any_of(declarationModifiers.begin(),
                           declarationModifiers.end(),
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

    m_stream << "  " << typedefDefinition->m_type_declaration->m_type->GetFullName() << '(';
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << typedefDefinition->m_name << ")";
    for (auto size : arraySize)
        m_stream << '[' << size << ']';
    m_stream << std::endl;
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

    m_stream << "  " << typedefDefinition->m_type_declaration->m_type->GetFullName();
    for (auto p = 0; p < pointerDepth; p++)
        m_stream << '*';
    m_stream << " " << typedefDefinition->m_name;
    for (auto size : arraySize)
        m_stream << '[' << size << ']';
    m_stream << std::endl;
}

void PrettyPrinter::PrintEnums() const
{
    const auto& allEnums = m_repository->GetAllEnums();
    m_stream << allEnums.size() << " enums:" << std::endl;

    for (auto* enumDefinition : allEnums)
    {
        m_stream << " Name: " << enumDefinition->GetFullName() << std::endl;
        m_stream << " Alignment: " << enumDefinition->GetAlignment() << std::endl;
        m_stream << " Size: " << enumDefinition->GetSize() << std::endl;

        for (const auto& enumMember : enumDefinition->m_members)
        {
            m_stream << "  " << enumMember->m_name << " = " << enumMember->m_value << std::endl;
        }

        m_stream << std::endl;
    }
}

void PrettyPrinter::PrintStructs() const
{
    const auto& allStructs = m_repository->GetAllStructs();
    m_stream << allStructs.size() << " structs:" << std::endl;

    for (auto* structDefinition : allStructs)
    {
        m_stream << " Name: " << structDefinition->GetFullName() << std::endl;
        m_stream << " Alignment: " << structDefinition->GetAlignment() << std::endl;
        m_stream << " Size: " << structDefinition->GetSize() << std::endl;

        for (const auto& variable : structDefinition->m_members)
        {
            PrintVariable(variable.get());
        }

        m_stream << std::endl;
    }
}

void PrettyPrinter::PrintUnions() const
{
    const auto& allUnions = m_repository->GetAllUnions();
    m_stream << allUnions.size() << " unions:" << std::endl;

    for (auto* unionDefinition : allUnions)
    {
        m_stream << " Name: " << unionDefinition->GetFullName() << std::endl;
        m_stream << " Alignment: " << unionDefinition->GetAlignment() << std::endl;
        m_stream << " Size: " << unionDefinition->GetSize() << std::endl;

        for (const auto& variable : unionDefinition->m_members)
        {
            PrintVariable(variable.get());
        }

        m_stream << std::endl;
    }
}

void PrettyPrinter::PrintTypedefs() const
{
    const auto& allTypedefs = m_repository->GetAllTypedefs();
    m_stream << allTypedefs.size() << " typedefs:" << std::endl;

    for (auto* typedefDefinition : allTypedefs)
    {
        m_stream << " Name: " << typedefDefinition->GetFullName() << std::endl;
        m_stream << " Alignment: " << typedefDefinition->GetAlignment() << std::endl;
        m_stream << " Size: " << typedefDefinition->GetSize() << std::endl;

        const auto& declarationModifiers = typedefDefinition->m_type_declaration->m_declaration_modifiers;
        if (declarationModifiers.empty())
        {
            std::cout << "  " << typedefDefinition->m_type_declaration->m_type->GetFullName() << std::endl;
        }
        else
        {
            if (declarationModifiers[0]->GetType() == DeclarationModifierType::POINTER
                && std::any_of(declarationModifiers.begin(),
                               declarationModifiers.end(),
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

        m_stream << std::endl;
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
