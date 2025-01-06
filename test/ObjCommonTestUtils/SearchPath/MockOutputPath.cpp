#include "MockOutputPath.h"

#include <sstream>

namespace
{
    class MockFileWrapper final : public std::ostringstream
    {
    public:
        MockFileWrapper(std::string name, std::vector<MockOutputFile>& files)
            : m_name(std::move(name)),
              m_files(files)
        {
        }

        ~MockFileWrapper() override
        {
            m_files.emplace_back(std::move(m_name), str());
        }

    private:
        std::string m_name;
        std::vector<MockOutputFile>& m_files;
    };
} // namespace

MockOutputFile::MockOutputFile() = default;

MockOutputFile::MockOutputFile(std::string name, std::string data)
    : m_name(std::move(name)),
      m_data(std::move(data))
{
}

std::unique_ptr<std::ostream> MockOutputPath::Open(const std::string& fileName)
{
    return std::make_unique<MockFileWrapper>(fileName, m_files);
}

const MockOutputFile* MockOutputPath::GetMockedFile(const std::string& name) const
{
    for (const auto& file : m_files)
    {
        if (file.m_name == name)
            return &file;
    }

    return nullptr;
}
