#include <foxen/flac.h>
#include "FlacDecoder.h"

class fx_flac_raii
{
public:
    fx_flac_raii()
    {
        ptr = FX_FLAC_ALLOC_DEFAULT();
    }

    ~fx_flac_raii()
    {
        free(ptr);
    }

    operator fx_flac_t* ()
    {
        return ptr;
    }

private: 
    fx_flac_t* ptr;
};

class FlacDecoderImpl : public FlacDecoder
{
private:
    void* m_data;
    size_t m_length;
    std::unique_ptr<fx_flac_raii> m_flac;

public:
    explicit FlacDecoderImpl(void* data, size_t length)
        : m_data(data),
          m_length(length)
    {

    }

    unsigned int GetFrameCount() override
    {
        return static_cast<unsigned int>(fx_flac_get_streaminfo(*m_flac.get(), FLAC_KEY_N_SAMPLES));
    }

    unsigned int GetFrameRate() override
    {
        return static_cast<unsigned int>(fx_flac_get_streaminfo(*m_flac.get(), FLAC_KEY_SAMPLE_RATE));
    }

    unsigned int GetNumChannels() override 
    {
        return static_cast<unsigned int>(fx_flac_get_streaminfo(*m_flac.get(), FLAC_KEY_N_CHANNELS));
    }

    bool Decode() override
    {
        m_flac = std::make_unique<fx_flac_raii>();

        while (true)
        {
            auto res = fx_flac_process(*m_flac.get(), reinterpret_cast<uint8_t*>(m_data), &m_length, nullptr, nullptr);
            if (res == FLAC_ERR)
                return false;

            if (res == FLAC_END_OF_METADATA)
                break;
        }

        return true;
    }
};

std::unique_ptr<FlacDecoder> FlacDecoder::Create(void* data, size_t length)
{
    return std::make_unique<FlacDecoderImpl>(data, length);
}
