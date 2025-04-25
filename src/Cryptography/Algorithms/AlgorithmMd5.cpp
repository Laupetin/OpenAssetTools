#include "AlgorithmMd5.h"

#include "Internal/CryptoLibrary.h"

#include <cstdint>

using namespace cryptography;

namespace
{
    constexpr int HASH_SIZE = 16;

    class AlgorithmMd5 final : public IHashFunction
    {
    public:
        AlgorithmMd5()
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
            md5_init(&m_state);
        }

        void Process(const void* input, const size_t inputSize) override
        {
            md5_process(&m_state, static_cast<const uint8_t*>(input), static_cast<unsigned long>(inputSize));
        }

        void Finish(void* hashBuffer) override
        {
            md5_done(&m_state, static_cast<uint8_t*>(hashBuffer));
        }

    private:
        hash_state m_state{};
    };
} // namespace

namespace cryptography
{
    std::unique_ptr<IHashFunction> CreateMd5()
    {
        return std::make_unique<AlgorithmMd5>();
    }
} // namespace cryptography
