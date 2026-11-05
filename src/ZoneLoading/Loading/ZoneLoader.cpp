#include "ZoneLoader.h"

#include "Exception/LoadingException.h"
#include "LoadingFileStream.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <format>

ZoneReader::ZoneReader()
    : m_processor_chain_dirty(false)
{
}

void ZoneReader::AddLoadingStep(std::unique_ptr<ILoadingStep> step)
{
    m_steps.emplace_back(std::move(step));
}

void ZoneReader::AddStreamProcessor(std::unique_ptr<StreamProcessor> streamProcessor)
{
    m_processors.push_back(std::move(streamProcessor));
    m_processor_chain_dirty = true;
}

void ZoneReader::RemoveStreamProcessor(const StreamProcessor* streamProcessor)
{
    for (auto i = m_processors.begin(); i < m_processors.end(); ++i)
    {
        if (i->get() == streamProcessor)
        {
            m_processors.erase(i);
            m_processor_chain_dirty = true;
            break;
        }
    }
}

bool ZoneReader::Run(std::istream& stream)
{
    LoadingFileStream fileStream(stream);
    auto* endStream = BuildLoadingChain(&fileStream);
    assert(endStream);

    try
    {
        for (const auto& step : m_steps)
        {
            step->PerformStep(*this, *endStream);

            if (m_processor_chain_dirty)
            {
                endStream = BuildLoadingChain(&fileStream);
                assert(endStream);
            }
        }
    }
    catch (LoadingException& e)
    {
        con::error("Loading fastfile failed: {}", e.DetailedMessage());

        return false;
    }

    return true;
}

ILoadingStream* ZoneReader::BuildLoadingChain(ILoadingStream* rootStream)
{
    auto* currentStream = rootStream;

    for (const auto& processor : m_processors)
    {
        processor->SetBaseStream(currentStream);

        currentStream = processor.get();
    }

    m_processor_chain_dirty = false;
    return currentStream;
}

ZoneLoader::ZoneLoader(std::unique_ptr<Zone> zone)
    : m_zone(std::move(zone))
{
}

std::unique_ptr<Zone> ZoneLoader::LoadZone(std::istream& stream)
{
    if (!Run(stream))
        return nullptr;

    m_zone->Register();

    return std::move(m_zone);
}
