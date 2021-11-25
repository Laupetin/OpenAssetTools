#include "DefinesIfDirectiveParsingState.h"

DefinesIfDirectiveParsingState::DefinesIfDirectiveParsingState(const std::map<std::string, DefinesStreamProxy::Define>& defines)
    : m_defines(defines)
{
}
