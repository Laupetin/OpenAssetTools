#include "DefinesDirectiveParsingState.h"

DefinesDirectiveParsingState::DefinesDirectiveParsingState(const std::map<std::string, DefinesStreamProxy::Define>& defines)
    : m_defines(defines)
{
}
