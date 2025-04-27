#include "AlgorithmSha1.h"

#include "Internal/CryptoLibrary.h"

#include <cstdint>

using namespace cryptography;

namespace
{
    constexpr int HASH_SIZE = 20;

    class AlgorithmSha1 final : public IHashFunction
    {
    public:
        AlgorithmSha1()
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
            sha1_init(&m_state);
        }

        void Process(const void* input, const size_t inputSize) override
        {
            sha1_process(&m_state, static_cast<const uint8_t*>(input), static_cast<unsigned long>(inputSize));
        }

        void Finish(void* hashBuffer) override
        {
            sha1_done(&m_state, static_cast<uint8_t*>(hashBuffer));
        }

    private:
        hash_state m_state{};
    };
} // namespace

namespace cryptography
{
    std::unique_ptr<IHashFunction> CreateSha1()
    {
        return std::make_unique<AlgorithmSha1>();
    }
} // namespace cryptography
