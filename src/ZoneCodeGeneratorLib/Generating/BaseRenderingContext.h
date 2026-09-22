#pragma once

#include "Persistence/IDataRepository.h"

#include <string>
#include <vector>

class BaseRenderingContext
{
public:
    std::string m_game;
    bool m_word_size_mismatch;
    WordSize m_word_size;
    unsigned m_pointer_size;
    bool m_endianness_mismatch;
    std::endian m_endianness;
    std::vector<const FastFileBlock*> m_blocks;

    const FastFileBlock* m_default_normal_block;
    const FastFileBlock* m_default_temp_block;

    const GameVariant* m_variant;

protected:
    BaseRenderingContext(std::string game, const GameVariant* variant, std::vector<const FastFileBlock*> fastFileBlocks);
};
