#include "MenuFileParserState.h"

using namespace menu;

MenuFileParserState::MenuFileParserState(const FeatureLevel featureLevel)
    : m_feature_level(featureLevel),
      m_in_global_scope(false)
{
}
