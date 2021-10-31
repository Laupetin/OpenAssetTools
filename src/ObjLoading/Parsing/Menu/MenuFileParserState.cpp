#include "MenuFileParserState.h"

MenuFileParserState::MenuFileParserState(const MenuFeatureLevel featureLevel)
    : m_feature_level(featureLevel),
      m_in_global_scope(false)
{
}
