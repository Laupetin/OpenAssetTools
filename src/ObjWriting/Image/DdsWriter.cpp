#include "DdsWriter.h"

DdsWriter::~DdsWriter()
= default;

bool DdsWriter::SupportsImageFormat(const ImageFormat* imageFormat)
{
    return true;
}

std::string DdsWriter::GetFileExtension()
{
    return ".dds";
}

void DdsWriter::DumpImage(FileAPI::IFile* file, Texture* texture)
{
}
