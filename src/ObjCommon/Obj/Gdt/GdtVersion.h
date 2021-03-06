#pragma once

#include <string>

class GdtVersion
{
public:
    std::string m_game;
    int m_version;

    GdtVersion();
    GdtVersion(std::string game, int version);
};