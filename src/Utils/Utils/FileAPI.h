#pragma once

#include <cstdint>
#include <string>

class FileAPI
{
public:
    enum class Mode
    {
        MODE_READ = 0,
        MODE_WRITE = 1
    };

    class IFile
    {
    public:
        virtual ~IFile() = default;

        virtual bool IsOpen() = 0;
        virtual size_t Read(void* buffer, size_t elementSize, size_t elementCount) = 0;
        virtual size_t Write(const void* data, size_t elementSize, size_t elementCount) = 0;
        virtual void Skip(int64_t amount) = 0;
        virtual size_t Printf(const char* fmt, ...) = 0;
        virtual int64_t Pos() = 0;
        virtual void Goto(int64_t pos) = 0;
        virtual void GotoEnd() = 0;
        virtual void Close() = 0;
    };

	class File final : public IFile
	{
		void* m_handle;

	public:
        File();
        explicit File(void* handle);
        File(File&) = delete;
        File(File&& f) noexcept;
		~File() override;

        File& operator=(File&) = delete;
        File& operator=(File&& f) noexcept;

		bool IsOpen() override;
		size_t Read(void* buffer, size_t elementSize, size_t elementCount) override;
		size_t Write(const void* data, size_t elementSize, size_t elementCount) override;
        void Skip(int64_t amount) override;
		size_t Printf(const char* fmt, ...) override;
        int64_t Pos() override;
        void Goto(int64_t pos) override;
        void GotoEnd() override;
		void Close() override;
	};

	static bool FileExists(const std::string& fileName);
	static uint64_t FileSize(const std::string& fileName);

    static bool DirectoryCreate(const std::string& directoryPath);
	static bool DirectoryExists(const std::string& directoryName);

	static File Open(const std::string& filename, Mode mode);
};