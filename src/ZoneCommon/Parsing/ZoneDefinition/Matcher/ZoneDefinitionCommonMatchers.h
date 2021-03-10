#pragma once

#include <limits>
#include <memory>

#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Matcher/MatcherLabel.h"

class ZoneDefinitionCommonMatchers
{
public:
    typedef AbstractMatcher<SimpleParserValue> matcher_t;
    typedef IMatcherForLabelSupplier<SimpleParserValue> supplier_t;

    static constexpr int LABEL_ASSET_NAME = std::numeric_limits<int>::max() - 1;
    
    static std::unique_ptr<matcher_t> AssetName(const supplier_t* labelSupplier);
};
