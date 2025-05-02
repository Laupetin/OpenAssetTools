#include "ProcessorCaptureData.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace
{
    class ProcessorCaptureData final : public processor::IProcessorCaptureData
    {
    public:
        explicit ProcessorCaptureData(const size_t captureSize)
            : m_data(std::make_unique<uint8_t[]>(captureSize)),
              m_capture_size(captureSize),
              m_captured_data_size(0)
        {
        }

        size_t Load(void* buffer, const size_t length) override
        {
            if (m_captured_data_size >= m_capture_size)
                return m_base_stream->Load(buffer, length);

            auto dataToCapture = m_capture_size - m_captured_data_size;
            dataToCapture = std::min(length, dataToCapture);

            auto loadedSize = m_base_stream->Load(&m_data[m_captured_data_size], dataToCapture);
            assert(length >= loadedSize);
            std::memcpy(buffer, &m_data[m_captured_data_size], loadedSize);

            m_captured_data_size += loadedSize;

            if (length > dataToCapture)
                loadedSize += m_base_stream->Load(&static_cast<uint8_t*>(buffer)[dataToCapture], length - dataToCapture);

            return loadedSize;
        }

        int64_t Pos() override
        {
            return m_base_stream->Pos();
        }

        void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override
        {
            assert(pCapturedData != nullptr);
            assert(pSize != nullptr);

            assert(m_captured_data_size == m_capture_size);

            *pCapturedData = m_data.get();
            *pSize = m_captured_data_size;
        }

    private:
        std::unique_ptr<uint8_t[]> m_data;
        size_t m_capture_size;
        size_t m_captured_data_size;
    };
} // namespace

namespace processor
{
    std::unique_ptr<IProcessorCaptureData> CreateProcessorCaptureData(size_t captureSize)
    {
        return std::make_unique<ProcessorCaptureData>(captureSize);
    }
} // namespace processor
