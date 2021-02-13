#pragma once

#include <limits>
#include <memory>

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Matcher/MatcherLabel.h"

class HeaderCommonMatchers
{
public:
    typedef AbstractMatcher<HeaderParserValue> matcher_t;
    typedef IMatcherForLabelSupplier<HeaderParserValue> supplier_t;

    static constexpr int LABEL_ALIGN = std::numeric_limits<int>::max() - 1;
    static constexpr int LABEL_ARRAY_DEF = std::numeric_limits<int>::max() - 2;
    static constexpr int LABEL_TYPENAME = std::numeric_limits<int>::max() - 3;

    static std::unique_ptr<matcher_t> Align(const supplier_t* labelSupplier);
    static std::unique_ptr<matcher_t> ArrayDef(const supplier_t* labelSupplier);
    static std::unique_ptr<matcher_t> Typename(const supplier_t* labelSupplier);
};
