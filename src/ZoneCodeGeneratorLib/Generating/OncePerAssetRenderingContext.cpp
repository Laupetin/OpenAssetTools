#include "OncePerAssetRenderingContext.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"

#include <algorithm>

RenderingUsedType::RenderingUsedType(const DataDefinition* type, StructureInformation* info)
    : m_members_loaded(false),
      m_type(type),
      m_info(info),
      m_is_context_asset(false),
      m_non_runtime_reference_exists(false),
      m_non_embedded_reference_exists(false),
      m_array_reference_exists(false),
      m_pointer_array_reference_exists(false),
      m_pointer_array_reference_is_reusable(false)
{
}

OncePerAssetRenderingContext::OncePerAssetRenderingContext(std::string game,
                                                           const Architecture gameArchitecture,
                                                           std::vector<const FastFileBlock*> fastFileBlocks)
    : BaseRenderingContext(std::move(game), gameArchitecture, std::move(fastFileBlocks)),
      m_asset(nullptr),
      m_has_actions(false)
{
    for (const auto* block : m_blocks)
    {
        if (block->m_is_default)
        {
            if (block->m_type == FastFileBlockType::NORMAL && m_default_normal_block == nullptr)
                m_default_normal_block = block;
            else if (block->m_type == FastFileBlockType::TEMP && m_default_temp_block == nullptr)
                m_default_temp_block = block;
        }
    }
}

RenderingUsedType* OncePerAssetRenderingContext::AddUsedType(std::unique_ptr<RenderingUsedType> usedType)
{
    auto* result = usedType.get();
    m_used_types.push_back(usedType.get());
    m_used_types_lookup.emplace(std::make_pair(usedType->m_type, std::move(usedType)));

    return result;
}

RenderingUsedType* OncePerAssetRenderingContext::GetBaseType(const IDataRepository* repository, MemberComputations* computations, RenderingUsedType* usedType)
{
    if (usedType->m_type->GetType() == DataDefinitionType::TYPEDEF)
    {
        const auto* typedefDefinition = dynamic_cast<const TypedefDefinition*>(usedType->m_type);

        while (typedefDefinition->m_type_declaration->m_type->GetType() == DataDefinitionType::TYPEDEF)
            typedefDefinition = dynamic_cast<const TypedefDefinition*>(typedefDefinition->m_type_declaration->m_type);

        const auto foundUsedType = m_used_types_lookup.find(typedefDefinition->m_type_declaration->m_type);
        if (foundUsedType == m_used_types_lookup.end())
        {
            const auto* memberDef = dynamic_cast<const DefinitionWithMembers*>(typedefDefinition->m_type_declaration->m_type);
            StructureInformation* info = nullptr;
            if (memberDef)
                info = repository->GetInformationFor(memberDef);

            auto* addedUsedType = AddUsedType(std::make_unique<RenderingUsedType>(typedefDefinition->m_type_declaration->m_type, info));
            ScanUsedTypeIfNeeded(repository, computations, usedType);
            return addedUsedType;
        }

        return foundUsedType->second.get();
    }

    return nullptr;
}

void OncePerAssetRenderingContext::AddMembersToContext(const IDataRepository* repository, StructureInformation* info)
{
    for (const auto& member : info->m_ordered_members)
    {
        MemberComputations computations(member.get());

        if (computations.ShouldIgnore())
            continue;

        RenderingUsedType* usedType;
        const auto existingUsedType = m_used_types_lookup.find(member->m_member->m_type_declaration->m_type);
        if (existingUsedType == m_used_types_lookup.end())
            usedType = AddUsedType(std::make_unique<RenderingUsedType>(member->m_member->m_type_declaration->m_type, member->m_type));
        else
            usedType = existingUsedType->second.get();

        auto* baseUsedType = GetBaseType(repository, &computations, usedType);

        if (!computations.IsInRuntimeBlock())
        {
            usedType->m_non_runtime_reference_exists = true;

            if (baseUsedType != nullptr)
                baseUsedType->m_non_runtime_reference_exists = true;
        }

        if (computations.ContainsNonEmbeddedReference())
            usedType->m_non_embedded_reference_exists = true;

        if (computations.ContainsArrayPointerReference() || computations.ContainsArrayReference())
            usedType->m_array_reference_exists = true;

        if (computations.ContainsPointerArrayReference() && !member->m_is_string)
        {
            usedType->m_pointer_array_reference_exists = true;

            if (member->m_is_reusable)
                usedType->m_pointer_array_reference_is_reusable = true;
        }

        ScanUsedTypeIfNeeded(repository, &computations, usedType);
    }
}

void OncePerAssetRenderingContext::ScanUsedTypeIfNeeded(const IDataRepository* repository, MemberComputations* computations, RenderingUsedType* usedType)
{
    if (usedType->m_info != nullptr && !StructureComputations(usedType->m_info).IsAsset() && !computations->IsInRuntimeBlock() && !usedType->m_members_loaded)
    {
        usedType->m_members_loaded = true;
        AddMembersToContext(repository, usedType->m_info);
    }
}

void OncePerAssetRenderingContext::MakeAsset(const IDataRepository* repository, StructureInformation* asset)
{
    m_asset = asset;
    AddUsedType(std::make_unique<RenderingUsedType>(asset->m_definition, asset));

    AddMembersToContext(repository, asset);
}

void OncePerAssetRenderingContext::CreateUsedTypeCollections()
{
    for (auto* usedType : m_used_types)
    {
        if (usedType->m_info != nullptr)
        {
            const StructureComputations computations(usedType->m_info);

            if (usedType->m_info->m_definition == usedType->m_type)
                m_used_structures.push_back(usedType);

            if (computations.IsAsset())
            {
                if (usedType->m_info != m_asset)
                    m_referenced_assets.push_back(usedType);
                else
                    usedType->m_is_context_asset = true;
            }

            if (!m_has_actions && UsedTypeHasActions(usedType))
            {
                m_has_actions = true;
            }
        }
    }
}

bool OncePerAssetRenderingContext::UsedTypeHasActions(const RenderingUsedType* usedType) const
{
    const StructureComputations computations(usedType->m_info);

    if (computations.IsAsset() && !usedType->m_is_context_asset)
        return false;

    if (!usedType->m_non_runtime_reference_exists && !usedType->m_is_context_asset)
        return false;

    if (usedType->m_info->m_post_load_action)
        return true;

    if (std::ranges::any_of(usedType->m_info->m_ordered_members,
                            [](const auto& member) -> bool
                            {
                                return member->m_post_load_action && !MemberComputations(member.get()).ShouldIgnore();
                            }))
    {
        return true;
    }

    return false;
}

std::unique_ptr<OncePerAssetRenderingContext> OncePerAssetRenderingContext::BuildContext(const IDataRepository* repository, StructureInformation* asset)
{
    auto context = std::make_unique<OncePerAssetRenderingContext>(
        OncePerAssetRenderingContext(repository->GetGameName(), repository->GetArchitecture(), repository->GetAllFastFileBlocks()));

    context->MakeAsset(repository, asset);
    context->CreateUsedTypeCollections();

    return std::move(context);
}
