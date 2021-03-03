#include "ProcessorCaptureData.h"

#include <cassert>
#include <cstring>

ProcessorCaptureData::ProcessorCaptureData(const size_t captureSize)
    : m_data(std::make_unique<uint8_t[]>(captureSize)),
      m_capture_size(captureSize),
      m_captured_data_size(0)
{
}

ProcessorCaptureData::~ProcessorCaptureData()
= default;

size_t ProcessorCaptureData::Load(void* buffer, const size_t length)
{
    if (m_captured_data_size >= m_capture_size)
        return m_base_stream->Load(buffer, length);

    size_t dataToCapture = m_capture_size - m_captured_data_size;

    if (length < dataToCapture)
        dataToCapture = length;

    size_t loadedSize = m_base_stream->Load(&m_data[m_captured_data_size], dataToCapture);
    assert(length >= loadedSize);
    memcpy(buffer, &m_data[m_captured_data_size], loadedSize);

    m_captured_data_size += loadedSize;

    if(length > dataToCapture)
        loadedSize += m_base_stream->Load(&static_cast<uint8_t*>(buffer)[dataToCapture], length - dataToCapture);

    return loadedSize;
}

int64_t ProcessorCaptureData::Pos()
{
    return m_base_stream->Pos();
}

void ProcessorCaptureData::GetCapturedData(const uint8_t** pCapturedData, size_t* pSize)
{
    assert(pCapturedData != nullptr);
    assert(pSize != nullptr);

    assert(m_captured_data_size == m_capture_size);

    *pCapturedData = m_data.get();
    *pSize = m_captured_data_size;
}
