#include "MockOutputPath.h"

#include "Utils/ObjStream.h"

#include <cstring>

namespace
{
    class MockFileBuffer final : public std::streambuf
    {
    public:
        MockFileBuffer()
            : m_pos(0u)
        {
        }

        std::vector<std::uint8_t> data()
        {
            return std::move(m_data);
        }

    protected:
        int_type overflow(const int_type b) override
        {
            if (!std::char_traits<char>::eq_int_type(b, std::char_traits<char>::eof()))
            {
                m_data.insert(m_data.begin() + static_cast<decltype(m_data)::difference_type>(m_pos), static_cast<std::uint8_t>(b));
                ++m_pos;
            }

            return b;
        }

        std::streamsize xsputn(const char* ptr, const std::streamsize count) override
        {
            const auto curSize = m_data.size();
            const auto overrideCount = m_pos < curSize ? std::min(curSize - m_pos, static_cast<size_t>(count)) : 0u;
            const auto insertCount = count - overrideCount;

            if (overrideCount > 0)
            {
                std::memcpy(&m_data[m_pos], ptr, overrideCount);
                m_pos += overrideCount;
                ptr += overrideCount;
            }

            if (insertCount > 0)
            {
                m_data.insert(m_data.begin() + static_cast<decltype(m_data)::difference_type>(m_pos), ptr, ptr + insertCount);
                m_pos += static_cast<size_t>(insertCount);
            }

            return count;
        }

        pos_type seekoff(const off_type off, const std::ios_base::seekdir dir, const std::ios_base::openmode mode) override
        {
            if (dir == std::ios::beg)
                return seekpos(off, mode);
            if (dir == std::ios::cur)
                return seekpos(static_cast<pos_type>(m_pos) + off, mode);
            if (off < static_cast<off_type>(m_data.size()))
                return seekpos(static_cast<off_type>(m_data.size()) - off, mode);
            return std::char_traits<char>::eof();
        }

        pos_type seekpos(const pos_type pos, std::ios_base::openmode) override
        {
            if (pos > m_data.size())
                m_data.resize(static_cast<decltype(m_data)::size_type>(pos));
            m_pos = static_cast<size_t>(pos);

            return pos;
        }

    private:
        std::vector<std::uint8_t> m_data;
        std::size_t m_pos;
    };

    class MockFileWrapper final : public std::ostream
    {
    public:
        MockFileWrapper(std::string name, std::vector<MockOutputFile>& files)
            : std::ostream(&m_buf),
              m_name(std::move(name)),
              m_files(files)
        {
        }

        ~MockFileWrapper() override
        {
            m_files.emplace_back(std::move(m_name), m_buf.data());
        }

    private:
        MockFileBuffer m_buf;
        std::string m_name;
        std::vector<MockOutputFile>& m_files;
    };
} // namespace

MockOutputFile::MockOutputFile() = default;

MockOutputFile::MockOutputFile(std::string name, std::vector<std::uint8_t> data)
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
