#pragma once

#include "BaseRenderingContext.h"
#include "Persistence/IDataRepository.h"

#include <string>
#include <vector>

class OncePerTemplateRenderingContext : public BaseRenderingContext
{
public:
    static std::unique_ptr<OncePerTemplateRenderingContext> BuildContext(const IDataRepository* repository);

    std::vector<StructureInformation*> m_assets;

private:
    OncePerTemplateRenderingContext(std::string game,
                                    Architecture gameArchitecture,
                                    std::vector<const FastFileBlock*> fastFileBlocks,
                                    std::vector<StructureInformation*> assets);
};
