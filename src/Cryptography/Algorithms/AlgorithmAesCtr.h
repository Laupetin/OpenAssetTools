#pragma once

#include "IStreamCipher.h"

#include <memory>

namespace cryptography
{
    /**
     * \brief Creates an AES cipher in CTR mode with a little-endian counter.
     * \param keyBytes The AES key (16, 24 or 32 bytes).
     * \param keySize The size of the key in bytes.
     * \remarks SetIv must be called with a 16 byte value before processing data. Calling it again restarts the counter.
     */
    std::unique_ptr<IStreamCipher> CreateAesCtr(const uint8_t* keyBytes, size_t keySize);
} // namespace cryptography
