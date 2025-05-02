#include "ProcessorXChunks.h"

#include "Loading/Exception/InvalidChunkSizeException.h"
#include "Zone/ZoneTypes.h"

#include <cassert>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace
{
    class DbLoadStream
    {
    public:
        DbLoadStream(const int streamIndex, const size_t chunkSize, std::vector<std::unique_ptr<IXChunkProcessor>>& chunkProcessors)
            : m_index(streamIndex),
              m_input_size(0),
              m_output_size(0),
              m_chunk_size(chunkSize),
              m_is_loading(false),
              m_processors(chunkProcessors)
        {
            for (auto& buffer : m_buffers)
                buffer = std::make_unique<uint8_t[]>(chunkSize);

            m_input_buffer = m_buffers[0].get();
            m_output_buffer = m_buffers[1].get();
        }

        [[nodiscard]] uint8_t* GetInputBuffer() const
        {
            return m_input_buffer;
        }

        void StartLoading(const size_t inputSize)
        {
            if (inputSize > 0)
            {
                std::unique_lock lock(m_load_mutex);

                if (m_is_loading)
                {
                    m_loading_finished.wait(lock);
                }

                m_input_size = inputSize;
                m_is_loading = true;
                m_load_thread = std::thread(&DbLoadStream::Load, this);
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

            std::unique_lock lock(m_load_mutex);
            if (m_is_loading)
            {
                m_loading_finished.wait(lock);
            }

            *pBuffer = m_output_buffer;
            *pSize = m_output_size;
        }

    private:
        void Load()
        {
            std::lock_guard lock(m_load_mutex);

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
    };

    class ProcessorXChunks final : public processor::IProcessorXChunks
    {
    public:
        ProcessorXChunks(const int numStreams, const size_t xChunkSize, const std::optional<size_t> vanillaBufferSize)
            : m_chunk_size(xChunkSize),
              m_vanilla_buffer_size(vanillaBufferSize),
              m_initialized_streams(false),
              m_current_stream(0),
              m_current_chunk(nullptr),
              m_current_chunk_size(0),
              m_current_chunk_offset(0),
              m_vanilla_buffer_offset(0),
              m_eof_reached(false),
              m_eof_stream(0)
        {
            assert(numStreams > 0);
            assert(xChunkSize > 0);

            for (int streamIndex = 0; streamIndex < numStreams; streamIndex++)
            {
                m_streams.emplace_back(std::make_unique<DbLoadStream>(streamIndex, xChunkSize, m_chunk_processors));
            }
        }

        size_t Load(void* buffer, const size_t length) override
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
                    std::memcpy(bufferPos, &m_current_chunk[m_current_chunk_offset], bytesLeftInCurrentChunk);
                    loadedSize += bytesLeftInCurrentChunk;

                    NextStream();
                }
                else
                {
                    std::memcpy(bufferPos, &m_current_chunk[m_current_chunk_offset], sizeToRead);
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

        int64_t Pos() override
        {
            return m_base_stream->Pos();
        }

        void AddChunkProcessor(std::unique_ptr<IXChunkProcessor> chunkProcessor) override
        {
            assert(chunkProcessor);

            m_chunk_processors.emplace_back(std::move(chunkProcessor));
        }

    private:
        void AdvanceStream(const unsigned streamNum)
        {
            assert(streamNum < m_streams.size());

            if (m_eof_reached)
                return;

            xchunk_size_t chunkSize;
            if (m_vanilla_buffer_size.has_value())
            {
                if (m_vanilla_buffer_offset + sizeof(chunkSize) > *m_vanilla_buffer_size)
                {
                    m_base_stream->Load(&chunkSize, *m_vanilla_buffer_size - m_vanilla_buffer_offset);
                    m_vanilla_buffer_offset = 0;
                }

                m_vanilla_buffer_offset = (m_vanilla_buffer_offset + sizeof(chunkSize)) % *m_vanilla_buffer_size;
            }

            const size_t readSize = m_base_stream->Load(&chunkSize, sizeof(chunkSize));

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
            const size_t loadedChunkSize = m_base_stream->Load(stream->GetInputBuffer(), chunkSize);

            if (loadedChunkSize != chunkSize)
            {
                throw InvalidChunkSizeException(chunkSize);
            }

            if (m_vanilla_buffer_size.has_value())
            {
                m_vanilla_buffer_offset = (m_vanilla_buffer_offset + loadedChunkSize) % *m_vanilla_buffer_size;
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
            m_vanilla_buffer_offset = static_cast<size_t>(m_base_stream->Pos());

            const auto streamCount = static_cast<unsigned>(m_streams.size());
            for (auto streamNum = 0u; streamNum < streamCount; streamNum++)
            {
                AdvanceStream(streamNum);
            }

            m_current_stream = 0;
            m_current_chunk_offset = 0;
            m_streams[0]->GetOutput(&m_current_chunk, &m_current_chunk_size);
        }

        [[nodiscard]] bool EndOfStream() const
        {
            return m_eof_reached && m_eof_stream == m_current_stream;
        }

        std::vector<std::unique_ptr<DbLoadStream>> m_streams;
        size_t m_chunk_size;
        std::optional<size_t> m_vanilla_buffer_size;
        std::vector<std::unique_ptr<IXChunkProcessor>> m_chunk_processors;

        bool m_initialized_streams;
        unsigned int m_current_stream;
        const uint8_t* m_current_chunk;
        size_t m_current_chunk_size;
        size_t m_current_chunk_offset;
        size_t m_vanilla_buffer_offset;

        bool m_eof_reached;
        unsigned int m_eof_stream;
    };
} // namespace

namespace processor
{
    std::unique_ptr<IProcessorXChunks> CreateProcessorXChunks(int numStreams, const size_t xChunkSize)
    {
        return std::make_unique<ProcessorXChunks>(numStreams, xChunkSize, std::nullopt);
    }

    std::unique_ptr<IProcessorXChunks> CreateProcessorXChunks(int numStreams, const size_t xChunkSize, const size_t vanillaBufferSize)
    {
        return std::make_unique<ProcessorXChunks>(numStreams, xChunkSize, vanillaBufferSize);
    }
} // namespace processor
