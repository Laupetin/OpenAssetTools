#include "StepVerifyMagic.h"

#include "Loading/Exception/InvalidMagicException.h"

#include <cstring>

namespace
{
    class StepVerifyMagic final : public ILoadingStep
    {
    public:
        explicit StepVerifyMagic(const char* magic)
            : m_magic(magic),
              m_magic_len(strlen(m_magic))
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            char currentCharacter;

            for (unsigned i = 0; i < m_magic_len; i++)
            {
                stream.Load(&currentCharacter, sizeof(char));

                if (currentCharacter != m_magic[i])
                    throw InvalidMagicException(m_magic);
            }
        }

    private:
        const char* m_magic;
        size_t m_magic_len;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifyMagic(const char* magic)
    {
        return std::make_unique<StepVerifyMagic>(magic);
    }
} // namespace step
