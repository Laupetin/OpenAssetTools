#pragma once

#include "ILoadingStep.h"
#include "StreamProcessor.h"
#include "Zone/XBlock.h"
#include "Zone/Zone.h"

#include <istream>
#include <memory>
#include <vector>

class ILoadingStep;

class ZoneLoader
{
public:
    explicit ZoneLoader(std::unique_ptr<Zone> zone);

    void AddXBlock(std::unique_ptr<XBlock> block);
    void AddLoadingStep(std::unique_ptr<ILoadingStep> step);
    void AddStreamProcessor(std::unique_ptr<StreamProcessor> streamProcessor);

    void RemoveStreamProcessor(const StreamProcessor* streamProcessor);

    std::unique_ptr<Zone> LoadZone(std::istream& stream);

    std::vector<XBlock*> m_blocks;

private:
    ILoadingStream* BuildLoadingChain(ILoadingStream* rootStream);

    std::vector<std::unique_ptr<ILoadingStep>> m_steps;
    std::vector<std::unique_ptr<StreamProcessor>> m_processors;

    bool m_processor_chain_dirty;

    std::unique_ptr<Zone> m_zone;
};
