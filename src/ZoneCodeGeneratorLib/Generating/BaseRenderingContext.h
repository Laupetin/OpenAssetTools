#pragma once

#include "Persistence/IDataRepository.h"

#include <string>
#include <vector>

class BaseRenderingContext
{
public:
    std::string m_game;
    bool m_architecture_mismatch;
    unsigned m_pointer_size;
    std::vector<const FastFileBlock*> m_blocks;

    const FastFileBlock* m_default_normal_block;
    const FastFileBlock* m_default_temp_block;

protected:
    BaseRenderingContext(std::string game, Architecture gameArchitecture, std::vector<const FastFileBlock*> fastFileBlocks);
};
