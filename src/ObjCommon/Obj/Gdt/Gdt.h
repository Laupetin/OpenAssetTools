#pragma once

#include <memory>
#include <vector>

#include "GdtEntry.h"
#include "GdtVersion.h"

class Gdt
{
public:
    GdtVersion m_version;
    std::vector<std::unique_ptr<GdtEntry>> m_entries;

    Gdt();
    explicit Gdt(GdtVersion version);
};
