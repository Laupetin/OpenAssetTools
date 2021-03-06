#include "GdtVersion.h"

GdtVersion::GdtVersion()
    : m_version(0)
{
}

GdtVersion::GdtVersion(std::string game, const int version)
    : m_game(std::move(game)),
      m_version(version)
{
}
