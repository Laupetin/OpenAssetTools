#include "AlgorithmSalsa20.h"

#include "salsa20.h"

#include <cassert>
#include <stdexcept>

using namespace cryptography;

namespace
{
    class AlgorithmSalsa20 final : public IStreamCipher
    {
    public:
        AlgorithmSalsa20(const uint8_t* keyBytes, const size_t keySize)
        {
            Salsa20_KeySetup(&m_context, keyBytes, static_cast<uint32_t>(keySize * 8uz));
        }

        void SetIv(const uint8_t* iv, const size_t ivSize) override
        {
            assert(ivSize == 8);

            if (ivSize != 8)
                throw std::invalid_argument("Salsa20 IV size must be 8");

            Salsa20_IVSetup(&m_context, iv);
        }

        void Process(const void* plainText, void* cipherText, const size_t amount) override
        {
            Salsa20_Encrypt_Bytes(&m_context, static_cast<const uint8_t*>(plainText), static_cast<uint8_t*>(cipherText), static_cast<uint32_t>(amount));
        }

    private:
        salsa20_ctx m_context{};
    };
} // namespace

namespace cryptography
{
    std::unique_ptr<IStreamCipher> CreateSalsa20(const uint8_t* keyBytes, const size_t keySize)
    {
        return std::make_unique<AlgorithmSalsa20>(keyBytes, keySize);
    }
} // namespace cryptography
