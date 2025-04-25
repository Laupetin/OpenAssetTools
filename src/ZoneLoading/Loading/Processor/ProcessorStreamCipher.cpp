#include "ProcessorStreamCipher.h"

ProcessorStreamCipher::ProcessorStreamCipher(std::unique_ptr<cryptography::IStreamCipher> cipher)
    : m_cipher(std::move(cipher))
{
}

size_t ProcessorStreamCipher::Load(void* buffer, const size_t length)
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
