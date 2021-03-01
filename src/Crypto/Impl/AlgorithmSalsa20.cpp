#include "AlgorithmSalsa20.h"
#include "salsa20.h"
#include <cassert>
#include <stdexcept>

class AlgorithmSalsa20::AlgorithmSalsa20Impl
{
    salsa20_ctx m_context{};

public:
    AlgorithmSalsa20Impl(const uint8_t* keyBytes, const size_t keySize)
    {
        Salsa20_KeySetup(&m_context, keyBytes, keySize * 8);
    }

    ~AlgorithmSalsa20Impl() = default;

    AlgorithmSalsa20Impl(AlgorithmSalsa20Impl& other) = default;
    AlgorithmSalsa20Impl(AlgorithmSalsa20Impl&& other) = delete;

    AlgorithmSalsa20Impl& operator=(AlgorithmSalsa20Impl const& other) = default;
    AlgorithmSalsa20Impl& operator=(AlgorithmSalsa20Impl&& other) = delete;

    void SetIV(const uint8_t* iv, const size_t ivSize)
    {
        assert(ivSize == 8);

        if(ivSize != 8)
        {
            throw std::invalid_argument("Salsa20 IV size must be 8");
        }

        Salsa20_IVSetup(&m_context, iv);
    }

    void Process(const void* plainText, void* cipherText, const size_t amount)
    {
        Salsa20_Encrypt_Bytes(&m_context, static_cast<const uint8_t*>(plainText), static_cast<uint8_t*>(cipherText), amount);
    }
};

AlgorithmSalsa20::AlgorithmSalsa20(const uint8_t* keyBytes, const size_t keySize)
{
    m_impl = new AlgorithmSalsa20Impl(keyBytes, keySize);
}

AlgorithmSalsa20::~AlgorithmSalsa20()
{
    delete m_impl;
    m_impl = nullptr;
}

AlgorithmSalsa20::AlgorithmSalsa20(AlgorithmSalsa20& other)
{
    m_impl = new AlgorithmSalsa20Impl(*other.m_impl);
}

AlgorithmSalsa20::AlgorithmSalsa20(AlgorithmSalsa20&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;
}

AlgorithmSalsa20& AlgorithmSalsa20::operator=(AlgorithmSalsa20 const& other)
{
    m_impl = new AlgorithmSalsa20Impl(*other.m_impl);

    return *this;
}

AlgorithmSalsa20& AlgorithmSalsa20::operator=(AlgorithmSalsa20&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;

    return *this;
}

void AlgorithmSalsa20::SetIV(const uint8_t* iv, const size_t ivSize)
{
    m_impl->SetIV(iv, ivSize);
}

void AlgorithmSalsa20::Process(const void* plainText, void* cipherText, const size_t amount)
{
    m_impl->Process(plainText, cipherText, amount);
}
