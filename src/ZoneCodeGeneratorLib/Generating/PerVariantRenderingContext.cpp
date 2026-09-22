#include "PerVariantRenderingContext.h"

PerVariantRenderingContext::PerVariantRenderingContext(const IDataRepository* repository, const GameVariant* variant)
    : PerTemplateRenderingContext(repository),
      m_word_size_mismatch(variant->m_word_size != OWN_WORD_SIZE),
      m_word_size(variant->m_word_size),
      m_pointer_size(GetPointerSizeForWordSize(variant->m_word_size)),
      m_endianness_mismatch(std::endian::native != variant->m_endianness),
      m_endianness(variant->m_endianness),
      m_variant(variant)
{
}
