#include "ProcessorXChunks.h"
#include "Zone/ZoneTypes.h"
#include "Loading/Exception/InvalidChunkSizeException.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <cstring>
#include <memory>

class DBLoadStream
{
    int m_index;

    std::unique_ptr<uint8_t[]> m_buffers[2];

    uint8_t* m_input_buffer;
    size_t m_input_size;

    uint8_t* m_output_buffer;
    size_t m_output_size;

    size_t m_chunk_size;

    bool m_is_loading;
    std::mutex m_load_mutex;
    std::condition_variable m_loading_finished;
    std::thread m_load_thread;

    std::vector<std::unique_ptr<IXChunkProcessor>>& m_processors;

    void Load()
    {
        std::lock_guard<std::mutex> lock(m_load_mutex);

        bool firstProcessor = true;

        for (const auto& processor : m_processors)
        {
            if (!firstProcessor)
            {
                uint8_t* previousInputBuffer = m_input_buffer;
                m_input_buffer = m_output_buffer;
                m_output_buffer = previousInputBuffer;

                m_input_size = m_output_size;
                m_output_size = 0;
            }

            m_output_size = processor->Process(m_index, m_input_buffer, m_input_size, m_output_buffer, m_chunk_size);

            firstProcessor = false;
        }

        m_is_loading = false;
        m_loading_finished.notify_all();
    }

public:
    DBLoadStream(const int streamIndex, const size_t chunkSize,
                 std::vector<std::unique_ptr<IXChunkProcessor>>& chunkProcessors) : m_processors(chunkProcessors)
    {
        m_index = streamIndex;
        m_chunk_size = chunkSize;

        for(auto& buffer : m_buffers)
            buffer = std::make_unique<uint8_t[]>(chunkSize);
        
        m_input_buffer = m_buffers[0].get();
        m_output_buffer = m_buffers[1].get();

        m_input_size = 0;
        m_output_size = 0;

        m_is_loading = false;
    }

    uint8_t* GetInputBuffer() const
    {
        return m_input_buffer;
    }

    void StartLoading(const size_t inputSize)
    {
        if (inputSize > 0)
        {
            std::unique_lock<std::mutex> lock(m_load_mutex);

            if (m_is_loading)
            {
                m_loading_finished.wait(lock);
            }

            m_input_size = inputSize;
            m_is_loading = true;
            m_load_thread = std::thread(&DBLoadStream::Load, this);
            m_load_thread.detach();
        }
        else
        {
            m_output_size = 0;
        }
    }

    void GetOutput(const uint8_t** pBuffer, size_t* pSize)
    {
        assert(pBuffer != nullptr);
        assert(pSize != nullptr);

        std::unique_lock<std::mutex> lock(m_load_mutex);
        if (m_is_loading)
        {
            m_loading_finished.wait(lock);
        }

        *pBuffer = m_output_buffer;
        *pSize = m_output_size;
    }
};

class ProcessorXChunks::ProcessorXChunksImpl
{
    ProcessorXChunks* m_base;

    std::vector<std::unique_ptr<DBLoadStream>> m_streams;
    size_t m_chunk_size;
    size_t m_vanilla_buffer_size;
    std::vector<std::unique_ptr<IXChunkProcessor>> m_chunk_processors;

    bool m_initialized_streams;
    unsigned int m_current_stream;
    const uint8_t* m_current_chunk;
    size_t m_current_chunk_size;
    size_t m_current_chunk_offset;
    size_t m_vanilla_buffer_offset;

    bool m_eof_reached;
    unsigned int m_eof_stream;

    void AdvanceStream(const unsigned int streamNum)
    {
        assert(streamNum >= 0 && streamNum < m_streams.size());

        if (m_eof_reached)
            return;

        xchunk_size_t chunkSize;
        if (m_vanilla_buffer_size > 0)
        {
            if (m_vanilla_buffer_offset + sizeof chunkSize > m_vanilla_buffer_size)
            {
                m_base->m_base_stream->Load(&chunkSize, m_vanilla_buffer_size - m_vanilla_buffer_offset);
                m_vanilla_buffer_offset = 0;
            }

            m_vanilla_buffer_offset = (m_vanilla_buffer_offset + sizeof chunkSize) % m_vanilla_buffer_size;
        }

        const size_t readSize = m_base->m_base_stream->Load(&chunkSize, sizeof chunkSize);

        if (readSize == 0)
        {
            m_eof_reached = true;
            m_eof_stream = streamNum;
            return;
        }

        if (chunkSize > m_chunk_size)
        {
            throw InvalidChunkSizeException(chunkSize, m_chunk_size);
        }

        const auto& stream = m_streams[streamNum];
        const size_t loadedChunkSize = m_base->m_base_stream->Load(stream->GetInputBuffer(), chunkSize);

        if (loadedChunkSize != chunkSize)
        {
            throw InvalidChunkSizeException(chunkSize);
        }

        if (m_vanilla_buffer_size > 0)
        {
            m_vanilla_buffer_offset = (m_vanilla_buffer_offset + loadedChunkSize) % m_vanilla_buffer_size;
        }

        stream->StartLoading(loadedChunkSize);
    }

    void NextStream()
    {
        AdvanceStream(m_current_stream);

        m_current_stream = (m_current_stream + 1) % m_streams.size();
        m_current_chunk_offset = 0;
        m_streams[m_current_stream]->GetOutput(&m_current_chunk, &m_current_chunk_size);
    }

    void InitStreams()
    {
        m_initialized_streams = true;
        m_vanilla_buffer_offset = static_cast<size_t>(m_base->m_base_stream->Pos());

        const unsigned int streamCount = m_streams.size();
        for (unsigned int streamNum = 0; streamNum < streamCount; streamNum++)
        {
            AdvanceStream(streamNum);
        }

        m_current_stream = 0;
        m_current_chunk_offset = 0;
        m_streams[0]->GetOutput(&m_current_chunk, &m_current_chunk_size);
    }

    bool EndOfStream() const
    {
        return m_eof_reached && m_eof_stream == m_current_stream;
    }

public:
    ProcessorXChunksImpl(ProcessorXChunks* base, const int numStreams, const size_t xChunkSize)
    {
        assert(base != nullptr);
        assert(numStreams > 0);
        assert(xChunkSize > 0);

        m_base = base;

        for (int streamIndex = 0; streamIndex < numStreams; streamIndex++)
        {
            m_streams.emplace_back(std::make_unique<DBLoadStream>(streamIndex, xChunkSize, m_chunk_processors));
        }

        m_chunk_size = xChunkSize;
        m_vanilla_buffer_size = 0;

        m_initialized_streams = false;
        m_current_stream = 0;
        m_current_chunk = nullptr;
        m_current_chunk_size = 0;
        m_current_chunk_offset = 0;
        m_vanilla_buffer_offset = 0;

        m_eof_reached = false;
        m_eof_stream = 0;
    }

    ProcessorXChunksImpl(ProcessorXChunks* base, const int numStreams, const size_t xChunkSize,
                         const size_t vanillaBufferSize) : ProcessorXChunksImpl(base, numStreams, xChunkSize)
    {
        m_vanilla_buffer_size = vanillaBufferSize;
    }

    void AddChunkProcessor(std::unique_ptr<IXChunkProcessor> streamProcessor)
    {
        assert(streamProcessor != nullptr);

        m_chunk_processors.emplace_back(std::move(streamProcessor));
    }

    size_t Load(void* buffer, const size_t length)
    {
        assert(buffer != nullptr);

        if (!m_initialized_streams)
        {
            InitStreams();
        }

        size_t loadedSize = 0;
        while (!EndOfStream() && loadedSize < length)
        {
            auto* bufferPos = static_cast<uint8_t*>(buffer) + loadedSize;
            const size_t sizeToRead = length - loadedSize;
            const size_t bytesLeftInCurrentChunk = m_current_chunk_size - m_current_chunk_offset;

            if (sizeToRead > bytesLeftInCurrentChunk)
            {
                assert(sizeToRead >= bytesLeftInCurrentChunk);
                memcpy(bufferPos, &m_current_chunk[m_current_chunk_offset], bytesLeftInCurrentChunk);
                loadedSize += bytesLeftInCurrentChunk;

                NextStream();
            }
            else
            {
                memcpy(bufferPos, &m_current_chunk[m_current_chunk_offset], sizeToRead);
                loadedSize += sizeToRead;
                m_current_chunk_offset += sizeToRead;

                if (m_current_chunk_offset == m_current_chunk_size)
                {
                    NextStream();
                }
            }
        }

        return loadedSize;
    }

    int64_t Pos() const
    {
        return m_base->m_base_stream->Pos();
    }
};

ProcessorXChunks::ProcessorXChunks(const int numStreams, const size_t xChunkSize)
{
    m_impl = new ProcessorXChunksImpl(this, numStreams, xChunkSize);
}

ProcessorXChunks::ProcessorXChunks(const int numStreams, const size_t xChunkSize, const size_t vanillaBufferSize)
{
    m_impl = new ProcessorXChunksImpl(this, numStreams, xChunkSize, vanillaBufferSize);
}

ProcessorXChunks::~ProcessorXChunks()
{
    delete m_impl;
    m_impl = nullptr;
}

void ProcessorXChunks::AddChunkProcessor(std::unique_ptr<IXChunkProcessor> chunkProcessor) const
{
    m_impl->AddChunkProcessor(std::move(chunkProcessor));
}

size_t ProcessorXChunks::Load(void* buffer, const size_t length)
{
    return m_impl->Load(buffer, length);
}

int64_t ProcessorXChunks::Pos()
{
    return m_impl->Pos();
}