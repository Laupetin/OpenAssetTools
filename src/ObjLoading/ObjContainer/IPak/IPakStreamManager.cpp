#include "IPakStreamManager.h"
#include "IPakEntryReadStream.h"

IPakStreamManager::IPakStreamManager(FileAPI::IFile* file)
{
    m_file = file;
}

IPakStreamManager::~IPakStreamManager()
{
    m_stream_mutex.lock();
    for(const auto& openStream : m_open_streams)
    {
        openStream->Close();
    }
    m_open_streams.clear();
    m_stream_mutex.unlock();
}

FileAPI::IFile* IPakStreamManager::OpenStream(const int64_t startPosition, const size_t length)
{
    auto* stream = new IPakEntryReadStream(m_file, this, &m_read_mutex, startPosition, length);

    m_stream_mutex.lock();
    m_open_streams.push_back(stream);
    m_stream_mutex.unlock();

    return stream;
}

void IPakStreamManager::OnCloseStream(FileAPI::IFile* stream)
{
    m_stream_mutex.lock();
    const auto openStreamEntry = std::find(m_open_streams.begin(), m_open_streams.end(), stream);

    if(openStreamEntry != m_open_streams.end())
    {
        m_open_streams.erase(openStreamEntry);
    }
    m_stream_mutex.unlock();
}
