#include "BaseRenderingContext.h"

BaseRenderingContext::BaseRenderingContext(std::string game, const GameVariant* variant, std::vector<const FastFileBlock*> fastFileBlocks)
    : m_game(std::move(game)),
      m_word_size_mismatch(variant->m_word_size != OWN_WORD_SIZE),
      m_word_size(variant->m_word_size),
      m_pointer_size(GetPointerSizeForWordSize(variant->m_word_size)),
      m_endianness_mismatch(std::endian::native != variant->m_endianness),
      m_endianness(variant->m_endianness),
      m_blocks(std::move(fastFileBlocks)),
      m_default_normal_block(nullptr),
      m_default_temp_block(nullptr),
      m_variant(variant)
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
