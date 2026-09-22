#pragma once

#include "PerTemplateRenderingContext.h"
#include "Persistence/IDataRepository.h"

#include <bit>

class PerVariantRenderingContext : public PerTemplateRenderingContext
{
public:
    PerVariantRenderingContext(const IDataRepository* repository, const GameVariant* variant);

    bool m_word_size_mismatch;
    WordSize m_word_size;
    unsigned m_pointer_size;
    bool m_endianness_mismatch;
    std::endian m_endianness;

    const GameVariant* m_variant;
};
