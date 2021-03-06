#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "GdtEntry.h"
#include "GdtVersion.h"

class Gdt
{
public:
    GdtVersion m_version;
    std::vector<std::unique_ptr<GdtEntry>> m_entries;

    bool ReadFromStream(std::istream& stream);
    void WriteToStream(std::ostream& stream) const;
};
