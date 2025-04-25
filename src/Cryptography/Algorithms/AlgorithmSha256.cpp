#include "AlgorithmSha256.h"

#include "Internal/CryptoLibrary.h"

#include <cstdint>

using namespace cryptography;

namespace
{
    constexpr int HASH_SIZE = 32;

    class AlgorithmSha256 final : public IHashFunction
    {
    public:
        AlgorithmSha256()
        {
            internal::CryptoLibrary::Init();

            Init();
        }

        size_t GetHashSize() override
        {
            return HASH_SIZE;
        }

        void Init() override
        {
            sha256_init(&m_state);
        }

        void Process(const void* input, const size_t inputSize) override
        {
            sha256_process(&m_state, static_cast<const uint8_t*>(input), static_cast<unsigned long>(inputSize));
        }

        void Finish(void* hashBuffer) override
        {
            sha256_done(&m_state, static_cast<uint8_t*>(hashBuffer));
        }

    private:
        hash_state m_state{};
    };
} // namespace

namespace cryptography
{
    std::unique_ptr<IHashFunction> CreateSha256()
    {
        return std::make_unique<AlgorithmSha256>();
    }
} // namespace cryptography
