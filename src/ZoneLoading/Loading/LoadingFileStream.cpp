#include "LoadingFileStream.h"

LoadingFileStream::LoadingFileStream(FileAPI::File* file)
{
    m_file = file;
}

size_t LoadingFileStream::Load(void* buffer, const size_t length)
{
    return m_file->Read(buffer, 1, length);
}