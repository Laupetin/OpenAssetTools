#pragma once

#include "ILoadingStep.h"
#include "Zone/Zone.h"
#include "Zone/XBlock.h"
#include "StreamProcessor.h"

#include <vector>
#include <istream>

class ILoadingStep;

class ZoneLoader
{
    std::vector<std::unique_ptr<ILoadingStep>> m_steps;
    std::vector<std::unique_ptr<StreamProcessor>> m_processors;

    bool m_processor_chain_dirty;

    Zone* m_zone;

    ILoadingStream* BuildLoadingChain(ILoadingStream* rootStream);

public:
    std::vector<XBlock*> m_blocks;

    explicit ZoneLoader(Zone* zone);

    void AddXBlock(std::unique_ptr<XBlock> block);
    void AddLoadingStep(std::unique_ptr<ILoadingStep> step);
    void AddStreamProcessor(std::unique_ptr<StreamProcessor> streamProcessor);

    void RemoveStreamProcessor(StreamProcessor* streamProcessor);

    Zone* LoadZone(std::istream& stream);
};
