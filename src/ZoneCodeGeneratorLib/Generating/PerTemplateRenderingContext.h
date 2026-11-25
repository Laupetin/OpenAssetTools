#pragma once

#include "Persistence/IDataRepository.h"

#include <string>
#include <vector>

class PerTemplateRenderingContext
{
public:
    explicit PerTemplateRenderingContext(const IDataRepository* repository);

    std::string m_game;
    std::vector<const FastFileBlock*> m_blocks;

    const FastFileBlock* m_default_normal_block;
    const FastFileBlock* m_default_temp_block;

    std::vector<StructureInformation*> m_all_assets;
    std::vector<GameVariant*> m_all_variants;
};
