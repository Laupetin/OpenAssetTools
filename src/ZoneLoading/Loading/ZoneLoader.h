#pragma once

#include "ILoadingStep.h"
#include "StreamProcessor.h"
#include "Zone/Zone.h"

#include <istream>
#include <memory>
#include <vector>

class ILoadingStep;

class ZoneReader
{
public:
    ZoneReader();

    void AddLoadingStep(std::unique_ptr<ILoadingStep> step);
    void AddStreamProcessor(std::unique_ptr<StreamProcessor> streamProcessor);

    void RemoveStreamProcessor(const StreamProcessor* streamProcessor);

    bool Run(std::istream& stream);

protected:
    ILoadingStream* BuildLoadingChain(ILoadingStream* rootStream);

    std::vector<std::unique_ptr<ILoadingStep>> m_steps;
    std::vector<std::unique_ptr<StreamProcessor>> m_processors;

    bool m_processor_chain_dirty;
};

class ZoneLoader : public ZoneReader
{
public:
    explicit ZoneLoader(std::unique_ptr<Zone> zone);

    std::unique_ptr<Zone> LoadZone(std::istream& stream);

private:
    std::unique_ptr<Zone> m_zone;
};
