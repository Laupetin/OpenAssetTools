#include "OncePerTemplateRenderingContext.h"

#include "Domain/Computations/StructureComputations.h"

#include <algorithm>

OncePerTemplateRenderingContext::OncePerTemplateRenderingContext(std::string game,
                                                                 const Architecture gameArchitecture,
                                                                 std::vector<const FastFileBlock*> fastFileBlocks,
                                                                 std::vector<StructureInformation*> assets)
    : m_game(std::move(game)),
      m_architecture_mismatch(gameArchitecture != OWN_ARCHITECTURE),
      m_pointer_size(GetPointerSizeForArchitecture(gameArchitecture)),
      m_blocks(std::move(fastFileBlocks)),
      m_assets(std::move(assets)),
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
}

std::unique_ptr<OncePerTemplateRenderingContext> OncePerTemplateRenderingContext::BuildContext(const IDataRepository* repository)
{
    std::vector<StructureInformation*> assetInformation;
    for (auto* info : repository->GetAllStructureInformation())
    {
        if (!StructureComputations(info).IsAsset())
            continue;

        assetInformation.emplace_back(info);
    }

    return std::make_unique<OncePerTemplateRenderingContext>(
        OncePerTemplateRenderingContext(repository->GetGameName(), repository->GetArchitecture(), repository->GetAllFastFileBlocks(), assetInformation));
}
