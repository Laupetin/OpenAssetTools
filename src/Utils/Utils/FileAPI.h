#pragma once

#include <cstdint>
#include <string>

class FileAPI
{
public:
    enum Mode
    {
        MODE_READ = 0,
        MODE_WRITE = 1
    };

	class File
	{
		void* m_handle;

	public:
        File();
        explicit File(void* handle);
        File(File&) = delete;
        File(File&& f) noexcept;
		~File();

        File& operator=(File&) = delete;
        File& operator=(File&& f) noexcept;

		bool IsOpen() const;
		size_t Read(void* buffer, size_t elementSize, size_t elementCount) const;
		size_t Write(void* data, size_t elementSize, size_t elementCount) const;
        void Skip(size_t amount) const;
		size_t Printf(char* fmt, ...) const;
        int64_t Pos() const;
        void Goto(int64_t pos) const;
		void Close();
	};

	static bool FileExists(const std::string& fileName);
	static uint64_t FileSize(const std::string& fileName);
	static bool DirectoryExists(const std::string& directoryName);
	static File Open(const std::string& filename, Mode mode);
};