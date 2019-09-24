#pragma once

#include "ILoadingStep.h"
#include "Zone/Zone.h"
#include "Zone/XBlock.h"
#include "Utils/FileAPI.h"
#include "StreamProcessor.h"

#include <vector>

class ILoadingStep;

class ZoneLoader
{
    std::vector<ILoadingStep*> m_steps;
    std::vector<StreamProcessor*> m_processors;

    bool m_processor_chain_dirty;

    Zone* m_zone;

    ILoadingStream* BuildLoadingChain(ILoadingStream* rootStream);

public:
    std::vector<XBlock*> m_blocks;

    explicit ZoneLoader(Zone* zone);
    ~ZoneLoader();

    void AddXBlock(XBlock* block);
    void AddLoadingStep(ILoadingStep* step);
    void AddStreamProcessor(StreamProcessor* streamProcessor);

    Zone* LoadZone(FileAPI::File* file);
};
