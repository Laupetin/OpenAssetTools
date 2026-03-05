#include "FileUtils.h"

#include <cstring>
#include <sstream>

namespace fs = std::filesystem;

namespace utils
{
    bool ParsePathsString(const std::string& pathsString, std::set<std::string>& output)
    {
        std::ostringstream currentPath;
        auto pathStart = true;
        auto quotationPos = 0; // 0 = before quotations, 1 = in quotations, 2 = after quotations

        for (const auto character : pathsString)
        {
            switch (character)
            {
            case '"':
                if (quotationPos == 0 && pathStart)
                {
                    quotationPos = 1;
                }
                else if (quotationPos == 1)
                {
                    quotationPos = 2;
                    pathStart = false;
                }
                else
                {
                    return false;
                }
                break;

            case ';':
                if (quotationPos != 1)
                {
                    auto path = currentPath.str();
                    if (!path.empty())
                    {
                        output.insert(path);
                        currentPath = std::ostringstream();
                    }

                    pathStart = true;
                    quotationPos = 0;
                }
                else
                {
                    currentPath << character;
                }
                break;

            default:
                currentPath << character;
                pathStart = false;
                break;
            }
        }

        if (currentPath.tellp() > 0)
        {
            output.insert(currentPath.str());
        }

        return true;
    }

    TextFileCheckDirtyOutput::TextFileCheckDirtyOutput(fs::path path)
        : m_path(std::move(path)),
          m_open(false),
          m_has_existing_file(false)
    {
    }

    TextFileCheckDirtyOutput::~TextFileCheckDirtyOutput()
    {
        Close();
    }

    bool TextFileCheckDirtyOutput::Open()
    {
        if (m_open)
            return false;

        auto parentFolder(m_path);
        parentFolder.remove_filename();
        create_directories(parentFolder);

        m_has_existing_file = fs::is_regular_file(m_path);

        if (!m_has_existing_file)
        {
            m_file_stream = std::ofstream(m_path, std::fstream::out | std::fstream::binary);
            if (!m_file_stream.is_open())
                return false;
        }

        m_open = true;
        return true;
    }

    std::ostream& TextFileCheckDirtyOutput::Stream()
    {
        if (!m_has_existing_file)
            return m_file_stream;

        return m_memory;
    }

    TextFileCheckDirtyResult TextFileCheckDirtyOutput::Close()
    {
        if (!m_open)
            return TextFileCheckDirtyResult::FAILURE;

        m_open = false;

        if (m_has_existing_file)
        {
            const auto renderedContent = std::move(m_memory).str();
            m_memory = std::ostringstream();

            if (!FileIsDirty(renderedContent))
                return TextFileCheckDirtyResult::OUTPUT_WAS_UP_TO_DATE;

            std::ofstream stream(m_path, std::fstream::out | std::fstream::binary);
            if (!stream.is_open())
                return TextFileCheckDirtyResult::FAILURE;

            stream.write(renderedContent.data(), renderedContent.size());
            stream.close();
        }

        return TextFileCheckDirtyResult::OUTPUT_WRITTEN;
    }

    [[nodiscard]] bool TextFileCheckDirtyOutput::FileIsDirty(const std::string& renderedContent) const
    {
        const auto fileSize = static_cast<size_t>(fs::file_size(m_path));
        const size_t contentSize = renderedContent.size();
        if (fileSize != contentSize)
            return true;

        std::ifstream oldFileStream(m_path, std::fstream::in | std::fstream::binary);
        if (!oldFileStream.is_open())
            return true;

        char buffer[4096];
        size_t currentContentOffset = 0;
        while (currentContentOffset < contentSize)
        {
            const auto expectedReadCount = std::min<size_t>(sizeof(buffer), contentSize - currentContentOffset);
            oldFileStream.read(buffer, expectedReadCount);
            if (oldFileStream.gcount() != expectedReadCount)
            {
                oldFileStream.close();
                return true;
            }

            if (memcmp(buffer, renderedContent.data() + currentContentOffset, expectedReadCount) != 0)
            {
                oldFileStream.close();
                return true;
            }

            currentContentOffset += expectedReadCount;
        }

        oldFileStream.close();
        return false;
    }
} // namespace utils
