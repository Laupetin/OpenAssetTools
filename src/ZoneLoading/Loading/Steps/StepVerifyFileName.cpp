#include "StepVerifyFileName.h"

#include "Loading/Exception/InvalidFileNameException.h"

#include <sstream>

namespace
{
    class StepVerifyFileName final : public ILoadingStep
    {
    public:
        StepVerifyFileName(std::string fileName, const size_t fileNameBufferSize)
            : m_expected_file_name(std::move(fileName)),
              m_file_name_buffer_size(fileNameBufferSize)
        {
            if (m_expected_file_name.length() > (m_file_name_buffer_size - 1))
                m_expected_file_name.erase(m_file_name_buffer_size - 1);
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            std::stringstream originalFilenameStream;
            unsigned bufferOffset = 0;
            char c;

            for (; bufferOffset < m_file_name_buffer_size; bufferOffset++)
            {
                stream.Load(&c, sizeof(char));

                if (c == '\00')
                {
                    bufferOffset++;
                    break;
                }

                originalFilenameStream << c;
            }

            // Skip the rest of the buffer which should be null bytes
            while (bufferOffset < m_file_name_buffer_size)
            {
                stream.Load(&c, sizeof(char));
                bufferOffset++;
            }

            const auto originalFileName = originalFilenameStream.str();

            if (originalFileName != m_expected_file_name)
                throw InvalidFileNameException(m_expected_file_name, originalFileName);
        }

    private:
        std::string m_expected_file_name;
        size_t m_file_name_buffer_size;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifyFileName(std::string fileName, const size_t fileNameBufferSize)
    {
        return std::make_unique<StepVerifyFileName>(std::move(fileName), fileNameBufferSize);
    }
} // namespace step
