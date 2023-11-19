#pragma once

#include "GdtEntry.h"
#include "GdtVersion.h"

#include <memory>
#include <vector>

class Gdt
{
public:
    GdtVersion m_version;
    std::vector<std::unique_ptr<GdtEntry>> m_entries;

    Gdt();
    explicit Gdt(GdtVersion version);
};
