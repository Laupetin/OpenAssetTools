#include "BaseRenderingContext.h"

BaseRenderingContext::BaseRenderingContext(std::string game, const WordSize gameWordSize, std::vector<const FastFileBlock*> fastFileBlocks)
    : m_game(std::move(game)),
      m_word_size_mismatch(gameWordSize != OWN_WORD_SIZE),
      m_pointer_size(GetPointerSizeForWordSize(gameWordSize)),
      m_blocks(std::move(fastFileBlocks)),
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
