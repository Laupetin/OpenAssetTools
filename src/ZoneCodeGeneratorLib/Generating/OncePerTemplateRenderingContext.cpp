#include "OncePerTemplateRenderingContext.h"

#include "Domain/Computations/StructureComputations.h"

#include <algorithm>

OncePerTemplateRenderingContext::OncePerTemplateRenderingContext(std::string game,
                                                                 const WordSize gameWordSize,
                                                                 const std::endian endianness,
                                                                 std::vector<const FastFileBlock*> fastFileBlocks,
                                                                 std::vector<StructureInformation*> assets)
    : BaseRenderingContext(std::move(game), gameWordSize, endianness, std::move(fastFileBlocks)),
      m_assets(std::move(assets))
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

std::unique_ptr<OncePerTemplateRenderingContext> OncePerTemplateRenderingContext::BuildContext(const IDataRepository* repository, const GameVariant* variant)
{
    std::vector<StructureInformation*> assetInformation;
    for (auto* info : repository->GetAllStructureInformation())
    {
        if (!StructureComputations(info).IsAsset())
            continue;

        assetInformation.emplace_back(info);
    }

    return std::make_unique<OncePerTemplateRenderingContext>(OncePerTemplateRenderingContext(
        repository->GetGameName(), variant->m_word_size, variant->m_endianness, repository->GetAllFastFileBlocks(), assetInformation));
}
