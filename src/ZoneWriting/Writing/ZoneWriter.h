#pragma once

#include <vector>
#include <memory>

#include "IWritingStep.h"
#include "OutputStreamProcessor.h"
#include "Zone/Zone.h"

class IWritingStep;

class ZoneWriter
{
    std::vector<std::unique_ptr<IWritingStep>> m_steps;
    std::vector<std::unique_ptr<OutputStreamProcessor>> m_processors;

    bool m_processor_chain_dirty;

    IWritingStream* BuildWritingChain(IWritingStream* rootStream);

public:
    std::vector<std::unique_ptr<XBlock>> m_blocks;

    ZoneWriter();

    void AddXBlock(std::unique_ptr<XBlock> block);
    void AddWritingStep(std::unique_ptr<IWritingStep> step);
    void AddStreamProcessor(std::unique_ptr<OutputStreamProcessor> streamProcessor);

    void RemoveStreamProcessor(OutputStreamProcessor* streamProcessor);

    bool WriteZone(std::ostream& stream);
};
