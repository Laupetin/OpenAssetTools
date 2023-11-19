#pragma once
#include "Gdt.h"

#include <iostream>

class GdtReader
{
    std::istream& m_stream;
    char m_char;
    bool m_peeked;
    int m_line;

    static GdtEntry* GetEntryByName(const Gdt& gdt, const std::string& name);
    void PrintError(const std::string& message) const;
    int PeekChar();
    int NextChar();
    bool ReadStringContent(std::string& str);
    bool ReadProperties(GdtEntry& entry);
    bool AddEntry(Gdt& gdt, GdtEntry& entry) const;

public:
    explicit GdtReader(std::istream& stream);
    bool Read(Gdt& gdt);
};

class GdtOutputStream
{
    std::ostream& m_stream;
    bool m_open;
    unsigned m_intendation_level;

    void DoIntendation() const;

public:
    explicit GdtOutputStream(std::ostream& stream);

    void BeginStream();
    void WriteVersion(const GdtVersion& gdtVersion);
    void WriteEscaped(const std::string& str) const;
    void WriteEntry(const GdtEntry& entry);
    void EndStream();

    static void WriteGdt(const Gdt& gdt, std::ostream& stream);
};
