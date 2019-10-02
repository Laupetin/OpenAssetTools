#include "FileAPI.h"

#include <cstdarg>
#include <cstdio>

bool FileAPI::FileExists(const std::string& fileName)
{
	struct stat st{};

    return stat(fileName.c_str(), &st) >= 0 && !(st.st_mode & S_IFDIR);
}

uint64_t FileAPI::FileSize(const std::string& fileName)
{
	struct stat st{};

	if (stat(fileName.c_str(), &st) >= 0 && !(st.st_mode & S_IFDIR))
	{
		return st.st_size;
	}

	return 0;
}

bool FileAPI::DirectoryExists(const std::string& directoryName)
{
	struct stat st{};

    return stat(directoryName.c_str(), &st) >= 0 && st.st_mode & S_IFDIR;
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

bool FileAPI::File::IsOpen() const
{
	return this->m_handle != nullptr;
}

size_t FileAPI::File::Read(void* buffer, const size_t elementSize, const size_t elementCount) const
{
	if (!this->IsOpen())
		return 0;

	return fread(buffer, elementSize, elementCount, static_cast<FILE*>(m_handle));
}

size_t FileAPI::File::Write(void* data, const size_t elementSize, const size_t elementCount) const
{
	if (!this->IsOpen())
		return 0;

	return fwrite(data, elementSize, elementCount, static_cast<FILE*>(m_handle));
}

void FileAPI::File::Skip(const size_t amount) const
{
    if(!this->IsOpen())
        return;

    fseek(static_cast<FILE*>(m_handle), amount, SEEK_CUR);
}

size_t FileAPI::File::Printf(char* fmt, ...) const
{
	va_list ap;

	if (!this->IsOpen())
		return 0;

	va_start(ap, fmt);
    const int result = vfprintf(static_cast<FILE*>(m_handle), fmt, ap);
	va_end(ap);

    return result;
}

int64_t FileAPI::File::Pos() const
{
    return _ftelli64(static_cast<FILE*>(m_handle));
}

void FileAPI::File::Goto(const int64_t pos) const
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