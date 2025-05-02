#include "ProcessorStreamCipher.h"

namespace
{
    class ProcessorStreamCipher final : public StreamProcessor
    {
    public:
        explicit ProcessorStreamCipher(std::unique_ptr<cryptography::IStreamCipher> cipher)
            : m_cipher(std::move(cipher))
        {
        }

        size_t Load(void* buffer, const size_t length) override
        {
            if (m_base_stream != nullptr)
            {
                const size_t readSize = m_base_stream->Load(buffer, length);

                if (readSize > 0)
                    m_cipher->Process(buffer, buffer, readSize);

                return readSize;
            }

            return 0;
        }

        int64_t Pos() override
        {
            return m_base_stream->Pos();
        }

    private:
        std::unique_ptr<cryptography::IStreamCipher> m_cipher;
    };
} // namespace

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorStreamCipher(std::unique_ptr<cryptography::IStreamCipher> cipher)
    {
        return std::make_unique<ProcessorStreamCipher>(std::move(cipher));
    }
} // namespace processor
