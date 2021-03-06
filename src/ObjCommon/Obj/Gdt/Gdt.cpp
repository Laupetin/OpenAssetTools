#include "Gdt.h"

Gdt::Gdt()
= default;

Gdt::Gdt(GdtVersion version)
    : m_version(std::move(version))
{
}
