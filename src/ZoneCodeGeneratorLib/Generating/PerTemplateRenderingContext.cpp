#include "PerTemplateRenderingContext.h"

#include "Domain/Computations/StructureComputations.h"

PerTemplateRenderingContext::PerTemplateRenderingContext(const IDataRepository* repository)
    : m_game(repository->GetGameName()),
      m_blocks(repository->GetAllFastFileBlocks()),
      m_default_normal_block(nullptr),
      m_default_temp_block(nullptr)
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

    for (auto* info : repository->GetAllStructureInformation())
    {
        if (!StructureComputations(info).IsAsset())
            continue;

        m_all_assets.emplace_back(info);
    }

    for (auto* variant : repository->GetGameVariants())
    {
        m_all_variants.emplace_back(variant);
    }
}
