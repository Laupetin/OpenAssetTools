#include "OutputProcessorXChunks.h"

#include <cassert>

#include "Writing/WritingException.h"
#include "Zone/ZoneTypes.h"
#include "Zone/XChunk/XChunkException.h"

void OutputProcessorXChunks::Init()
{
    if (m_vanilla_buffer_size > 0)
        m_vanilla_buffer_offset = static_cast<size_t>(m_base_stream->Pos()) % m_vanilla_buffer_size;

    m_initialized = true;
}

void OutputProcessorXChunks::WriteChunk()
{
    if (m_vanilla_buffer_size > 0)
    {
        if (m_vanilla_buffer_offset + sizeof(xchunk_size_t) > m_vanilla_buffer_size)
        {
            xchunk_size_t zeroMem = 0;
            m_base_stream->Write(&zeroMem, m_vanilla_buffer_size - m_vanilla_buffer_offset);
            m_vanilla_buffer_offset = 0;
        }
    }

    try
    {
        for (const auto& processor : m_chunk_processors)
        {
            m_input_size = processor->Process(m_current_stream, m_input_buffer, m_input_size, m_output_buffer, m_chunk_size);
            auto* swap = m_input_buffer;
            m_input_buffer = m_output_buffer;
            m_output_buffer = swap;
        }
    }
    catch (XChunkException& e)
    {
        throw WritingException(e.Message());
    }

    auto chunkSize = static_cast<xchunk_size_t>(m_input_size);
    m_base_stream->Write(&chunkSize, sizeof(chunkSize));
    m_base_stream->Write(m_input_buffer, m_input_size);

    if (m_vanilla_buffer_size > 0)
    {
        m_vanilla_buffer_offset += sizeof(chunkSize) + m_input_size;
        m_vanilla_buffer_offset %= m_vanilla_buffer_size;
    }

    m_current_stream = (m_current_stream + 1) % m_stream_count;
}

OutputProcessorXChunks::OutputProcessorXChunks(const int numStreams, const size_t xChunkSize)
    : m_stream_count(numStreams),
      m_chunk_size(xChunkSize),
      m_vanilla_buffer_size(0),
      m_initialized(false),
      m_current_stream(0),
      m_vanilla_buffer_offset(0),
      m_input_buffer(nullptr),
      m_output_buffer(nullptr),
      m_input_size(0)
{
    assert(numStreams > 0);
    assert(xChunkSize > 0);

    for (auto i = 0u; i < 2u; i++)
        m_buffers.emplace_back(std::make_unique<uint8_t[]>(xChunkSize));

    m_input_buffer = m_buffers[0].get();
    m_output_buffer = m_buffers[1].get();
}

OutputProcessorXChunks::OutputProcessorXChunks(const int numStreams, const size_t xChunkSize, const size_t vanillaBufferSize)
    : OutputProcessorXChunks(numStreams, xChunkSize)
{
    m_vanilla_buffer_size = vanillaBufferSize;
}

void OutputProcessorXChunks::AddChunkProcessor(std::unique_ptr<IXChunkProcessor> chunkProcessor)
{
    assert(chunkProcessor != nullptr);

    m_chunk_processors.emplace_back(std::move(chunkProcessor));
}

void OutputProcessorXChunks::Write(const void* buffer, const size_t length)
{
    assert(buffer != nullptr);

    if (!m_initialized)
        Init();

    auto sizeRemaining = length;
    while (sizeRemaining > 0)
    {
        const auto toWrite = std::min(m_chunk_size - m_input_size, sizeRemaining);

        memcpy(&m_input_buffer[m_input_size], &static_cast<const char*>(buffer)[length - sizeRemaining], toWrite);
        m_input_size += toWrite;
        if (m_input_size >= m_chunk_size)
            WriteChunk();

        sizeRemaining -= toWrite;
    }
}

void OutputProcessorXChunks::Flush()
{
    if (m_input_size)
        WriteChunk();

    m_base_stream->Flush();
}

int64_t OutputProcessorXChunks::Pos()
{
    return m_base_stream->Pos();
}
