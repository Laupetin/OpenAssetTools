#include "ZoneLoader.h"
#include "Exception/LoadingException.h"
#include "LoadingFileStream.h"

#include <algorithm>

ZoneLoader::ZoneLoader(Zone* zone)
{
    m_zone = zone;
    m_processor_chain_dirty = false;
}

ZoneLoader::~ZoneLoader()
{
    for(auto step : m_steps)
    {
        delete step;
    }
    m_steps.clear();

    for(auto processor : m_processors)
    {
        delete processor;
    }
    m_processors.clear();
}

ILoadingStream* ZoneLoader::BuildLoadingChain(ILoadingStream* rootStream)
{
    ILoadingStream* currentStream = rootStream;

    for(auto processor : m_processors)
    {
        processor->SetBaseStream(currentStream);

        currentStream = processor;
    }
    
    m_processor_chain_dirty = false;
    return currentStream;
}

void ZoneLoader::AddXBlock(XBlock* block)
{
    m_blocks.push_back(block);

    std::sort(m_blocks.begin(), m_blocks.end(), [](XBlock* b1, XBlock* b2) -> bool
    {
        return b1->m_index < b2->m_index;
    });
}

void ZoneLoader::AddLoadingStep(ILoadingStep* step)
{
    m_steps.push_back(step);
}

void ZoneLoader::AddStreamProcessor(StreamProcessor* streamProcessor)
{
    m_processors.push_back(streamProcessor);
    m_processor_chain_dirty = true;
}

Zone* ZoneLoader::LoadZone(FileAPI::File* file)
{
    LoadingFileStream fileStream(file);
    ILoadingStream* endStream = BuildLoadingChain(&fileStream);

    try
    {
        for(auto step : m_steps)
        {
            step->PerformStep(this, endStream);

            if(m_processor_chain_dirty)
            {
                endStream = BuildLoadingChain(&fileStream);
            }
        }
    }
    catch (LoadingException& e)
    {
        const std::string detailedMessage = e.DetailedMessage();
        printf("Loading fastfile failed: %s\n", detailedMessage.c_str());

        delete m_zone;
    
        return nullptr;
    }

    m_zone->m_game->AddZone(m_zone);

    return m_zone;
}