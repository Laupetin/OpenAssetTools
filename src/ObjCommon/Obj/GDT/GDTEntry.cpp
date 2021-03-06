#include "GdtEntry.h"

GdtEntry::GdtEntry()
    : m_parent(nullptr)
{
}

GdtEntry::GdtEntry(std::string name, std::string gdfName)
    : m_name(std::move(name)),
      m_gdf_name(std::move(gdfName)),
      m_parent(nullptr)
{
}

GdtEntry::GdtEntry(std::string name, GdtEntry* parent)
    : m_name(std::move(name)),
      m_parent(parent)
{
}
