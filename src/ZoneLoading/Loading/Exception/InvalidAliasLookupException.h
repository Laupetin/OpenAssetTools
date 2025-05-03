#pragma once
#include "LoadingException.h"
#include "Zone/XBlock.h"

class InvalidAliasLookupException final : public LoadingException
{
public:
    InvalidAliasLookupException(size_t lookupIndex, size_t lookupCount);

    std::string DetailedMessage() override;
    [[nodiscard]] char const* what() const noexcept override;

private:
    size_t m_lookup_index;
    size_t m_lookup_count;
};
