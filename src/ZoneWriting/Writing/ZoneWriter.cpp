#include "ZoneWriter.h"

#include <iostream>

#include "WritingException.h"
#include "WritingFileStream.h"

ZoneWriter::ZoneWriter()
    : m_processor_chain_dirty(false)
{
}

IWritingStream* ZoneWriter::BuildWritingChain(IWritingStream* rootStream)
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

void ZoneWriter::AddXBlock(std::unique_ptr<XBlock> block)
{
    m_blocks.emplace_back(std::move(block));
}

void ZoneWriter::AddWritingStep(std::unique_ptr<IWritingStep> step)
{
    m_steps.emplace_back(std::move(step));
}

void ZoneWriter::AddStreamProcessor(std::unique_ptr<OutputStreamProcessor> streamProcessor)
{
    m_processors.push_back(std::move(streamProcessor));
    m_processor_chain_dirty = true;
}

void ZoneWriter::RemoveStreamProcessor(OutputStreamProcessor* streamProcessor)
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

bool ZoneWriter::WriteZone(std::ostream& stream)
{
    WritingFileStream fileStream(stream);
    auto* endStream = BuildWritingChain(&fileStream);

    try
    {
        for (const auto& step : m_steps)
        {
            step->PerformStep(this, endStream);

            if (m_processor_chain_dirty)
                endStream = BuildWritingChain(&fileStream);
        }
    }
    catch (WritingException& e)
    {
        std::cout << "Writing fastfile failed: " << e.Message() << std::endl;
        return false;
    }

    endStream->Flush();

    return true;
}
