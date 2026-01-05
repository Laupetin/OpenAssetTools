#pragma once

#include "Persistence/IDataRepository.h"

#include <string>
#include <vector>

class OncePerTemplateRenderingContext
{
public:
    static std::unique_ptr<OncePerTemplateRenderingContext> BuildContext(const IDataRepository* repository);

    std::string m_game;
    bool m_architecture_mismatch;
    unsigned m_pointer_size;
    std::vector<const FastFileBlock*> m_blocks;
    std::vector<StructureInformation*> m_assets;

    const FastFileBlock* m_default_normal_block;
    const FastFileBlock* m_default_temp_block;

private:
    OncePerTemplateRenderingContext(std::string game,
                                    Architecture gameArchitecture,
                                    std::vector<const FastFileBlock*> fastFileBlocks,
                                    std::vector<StructureInformation*> assets);
};
