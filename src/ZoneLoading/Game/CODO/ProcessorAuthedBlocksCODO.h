#pragma once

#include "Loading/ILoadingStep.h"
#include "Loading/StreamProcessor.h"

#include <array>
#include <cstdint>
#include <memory>

namespace CODO
{
    inline constexpr size_t AES_KEY_SIZE = 24;
    inline constexpr size_t AES_IV_SIZE = 16;

    /**
     * \brief The key material of a zone which is stored in an RSA encrypted block at the start of the file.
     */
    struct ZoneCipherKey
    {
        std::array<uint8_t, AES_KEY_SIZE> m_key{};
        std::array<uint8_t, AES_IV_SIZE> m_iv{};
    };

    /**
     * \brief Loads the RSA encrypted block and unwraps the key material of the zone from it.
     */
    std::unique_ptr<ILoadingStep> CreateStepLoadZoneCipherKey(std::shared_ptr<ZoneCipherKey> cipherKey);

    /**
     * \brief Decrypts and verifies the authed chunks of a zone.
     * The first chunk contains the auth header which contains the hashes of the following hash table chunks.
     * Each hash table chunk is followed by a group of data chunks and contains the hashes of these.
     * The chunk contents (without the first chunk) are provided to the next stage.
     */
    std::unique_ptr<StreamProcessor> CreateProcessorAuthedBlocks(std::shared_ptr<ZoneCipherKey> cipherKey);
} // namespace CODO
