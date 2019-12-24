#pragma once
#include "ILoadingStream.h"
#include "Utils/FileAPI.h"

class LoadingFileStream final : public ILoadingStream
{
    FileAPI::File* m_file;

public:
    explicit LoadingFileStream(FileAPI::File* file);

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
};
