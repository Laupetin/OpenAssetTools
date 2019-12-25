#include "FileAPI.h"

#include <cstdarg>
#include <cstdio>
#include <filesystem>

bool FileAPI::FileExists(const std::string& fileName)
{
    return std::filesystem::exists(fileName);
}

uint64_t FileAPI::FileSize(const std::string& fileName)
{
    return std::filesystem::file_size(fileName);
}

bool FileAPI::DirectoryCreate(const std::string& directoryPath)
{
    return std::filesystem::create_directories(directoryPath);
}

bool FileAPI::DirectoryExists(const std::string& directoryName)
{
    return std::filesystem::is_directory(directoryName);
}

FileAPI::File FileAPI::Open(const std::string& filename, const Mode mode)
{
    const char* modeStr;
    switch(mode)
    {
    default:
    case Mode::MODE_READ:
        modeStr = "rb";
        break;

    case Mode::MODE_WRITE:
        modeStr = "wb";
        break;
    }
    
	FILE* handle;
	if (fopen_s(&handle, filename.c_str(), modeStr) != 0)
	{
		return File(nullptr);
	}

	return File(handle);
}

FileAPI::File::File()
{
    this->m_handle = nullptr;
}

FileAPI::File::File(void* handle)
{
	this->m_handle = handle;
}

FileAPI::File::File(File&& f) noexcept
{
    m_handle = f.m_handle;
    f.m_handle = nullptr;
}

FileAPI::File::~File()
{
	this->Close();
}

FileAPI::File& FileAPI::File::operator=(File&& f) noexcept
{
    m_handle = f.m_handle;
    f.m_handle = nullptr;

    return *this;
}

bool FileAPI::File::IsOpen()
{
	return this->m_handle != nullptr;
}

size_t FileAPI::File::Read(void* buffer, const size_t elementSize, const size_t elementCount)
{
	if (!this->IsOpen())
		return 0;

	return fread(buffer, elementSize, elementCount, static_cast<FILE*>(m_handle));
}

size_t FileAPI::File::Write(const void* data, const size_t elementSize, const size_t elementCount)
{
	if (!this->IsOpen())
		return 0;

	return fwrite(data, elementSize, elementCount, static_cast<FILE*>(m_handle));
}

void FileAPI::File::Skip(const size_t amount)
{
    if(!this->IsOpen())
        return;

    fseek(static_cast<FILE*>(m_handle), amount, SEEK_CUR);
}

size_t FileAPI::File::Printf(const char* fmt, ...)
{
	va_list ap;

	if (!this->IsOpen())
		return 0;

	va_start(ap, fmt);
    const int result = vfprintf(static_cast<FILE*>(m_handle), fmt, ap);
	va_end(ap);

    return result;
}

int64_t FileAPI::File::Pos()
{
    return _ftelli64(static_cast<FILE*>(m_handle));
}

void FileAPI::File::Goto(const int64_t pos)
{
    _fseeki64(static_cast<FILE*>(m_handle), pos, SEEK_SET);
}

void FileAPI::File::Close()
{
	if (this->m_handle != nullptr)
	{
		fclose(static_cast<FILE*>(m_handle));
		this->m_handle = nullptr;
	}
}