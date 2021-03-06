#include "Gdt.h"

#include <sstream>

class GdtConst
{
public:
    static constexpr const char* VERSION_ENTRY_NAME = "version";
    static constexpr const char* VERSION_ENTRY_GDF = "version.gdf";
    static constexpr const char* VERSION_KEY_GAME = "game";
    static constexpr const char* VERSION_KEY_VERSION = "version";
};

class GdtReader
{
    std::istream& m_stream;
    char m_char;
    bool m_peeked;
    int m_line;

    void PrintError(const std::string& message) const
    {
        std::cout << "GDT Error at line " << m_line << ": " << message << "\n";
    }

    int PeekChar()
    {
        if (m_peeked)
            return m_char;

        int c;
        do
        {
            c = m_stream.get();
        }
        while (isspace(c));

        m_peeked = true;
        m_char = c;
        return c;
    }

    int NextChar()
    {
        if (m_peeked)
        {
            m_peeked = false;
            return m_char;
        }

        int c;
        do
        {
            c = m_stream.get();
        }
        while (isspace(c));

        return c;
    }

    bool ReadStringContent(std::string& str)
    {
        std::ostringstream ss;

        if (NextChar() != '"')
        {
            PrintError("Expected string opening tag");
            return false;
        }

        auto c = m_stream.get();
        while (c != '"' && c != '\n' && c != EOF)
        {
            ss << static_cast<char>(c);
            c = m_stream.get();
        }

        if (c == '"')
        {
            str = ss.str();
            return true;
        }

        return false;
    }

    static GdtEntry* GetEntryByName(const Gdt& gdt, const std::string& name)
    {
        for (const auto& entry : gdt.m_entries)
        {
            if (entry->m_name == name)
                return entry.get();
        }

        return nullptr;
    }

    bool ReadProperties(GdtEntry& entry)
    {
        while (PeekChar() == '"')
        {
            std::string propertyKey;
            std::string propertyValue;

            if (!ReadStringContent(propertyKey))
                return false;

            if (PeekChar() != '"')
            {
                PrintError("Expected value string");
                return false;
            }

            if (!ReadStringContent(propertyValue))
                return false;

            entry.m_properties.emplace(std::move(propertyKey), std::move(propertyValue));
        }


        if (NextChar() != '}')
        {
            PrintError("Expected closing tags");
            return false;
        }

        return true;
    }

    bool AddEntry(Gdt& gdt, GdtEntry& entry) const
    {
        if(entry.m_name == GdtConst::VERSION_ENTRY_NAME
            && entry.m_gdf_name == GdtConst::VERSION_ENTRY_GDF)
        {
            auto foundEntry = entry.m_properties.find(GdtConst::VERSION_KEY_GAME);
            if(foundEntry == entry.m_properties.end())
            {
                PrintError("Version does not feature game property");
                return false;
            }
            gdt.m_version.m_game = foundEntry->second;

            foundEntry = entry.m_properties.find(GdtConst::VERSION_KEY_VERSION);
            if (foundEntry == entry.m_properties.end())
            {
                PrintError("Version does not feature version property");
                return false;
            }
            gdt.m_version.m_version = strtol(foundEntry->second.c_str(), nullptr, 0);
        }
        else
        {
            gdt.m_entries.emplace_back(std::make_unique<GdtEntry>(std::move(entry)));
        }

        return true;
    }

public:
    explicit GdtReader(std::istream& stream)
        : m_stream(stream),
          m_char(0),
          m_peeked(false),
          m_line(0)
    {
    }

    bool Read(Gdt& gdt)
    {
        if (NextChar() != '{')
        {
            PrintError("Expected opening tag");
            return false;
        }

        while (PeekChar() == '"')
        {
            GdtEntry entry;

            if (!ReadStringContent(entry.m_name))
            {
                PrintError("Failed to read string");
                return false;
            }

            if (PeekChar() == '(')
            {
                NextChar();
                if (!ReadStringContent(entry.m_gdf_name))
                    return false;
                if (NextChar() != ')')
                {
                    PrintError("Expected closing parenthesis");
                    return false;
                }
            }
            else if (PeekChar() == '[')
            {
                NextChar();
                std::string parentName;
                if (!ReadStringContent(parentName))
                    return false;
                if (NextChar() != ']')
                {
                    PrintError("Expected closing square brackets");
                    return false;
                }
                entry.m_parent = GetEntryByName(gdt, parentName);
                if (entry.m_parent == nullptr)
                {
                    PrintError("Could not find parent with name");
                    return false;
                }
            }
            else
            {
                return false;
            }

            if (NextChar() != '{')
            {
                PrintError("Expected opening tag for entries");
                return false;
            }

            if (!ReadProperties(entry))
                return false;

            if (!AddEntry(gdt, entry))
                return false;
        }


        if (NextChar() != '}')
        {
            PrintError("Expected closing tags");
            return false;
        }

        return true;
    }
};

class GdtWriter
{
    std::ostream& m_stream;
    unsigned m_intendation_level;

    void DoIntendation() const
    {
        for (auto i = 0u; i < m_intendation_level; i++)
            m_stream << "\t";
    }

    void WriteVersion(const GdtVersion& gdtVersion)
    {
        GdtEntry versionEntry;
        versionEntry.m_name = GdtConst::VERSION_ENTRY_NAME;
        versionEntry.m_gdf_name = GdtConst::VERSION_ENTRY_GDF;
        versionEntry.m_properties[GdtConst::VERSION_KEY_GAME] = gdtVersion.m_game;
        versionEntry.m_properties[GdtConst::VERSION_KEY_VERSION] = std::to_string(gdtVersion.m_version);

        WriteEntry(versionEntry);
    }

    void WriteEntry(const GdtEntry& entry)
    {
        DoIntendation();
        m_stream << "\"" << entry.m_name << "\" ";
        if (entry.m_parent)
            m_stream << "[ \"" << entry.m_parent->m_name << "\" ]\n";
        else
            m_stream << "( \"" << entry.m_gdf_name << " )\n";
        m_stream << "{\n";

        m_intendation_level++;

        for (const auto& [propertyKey, propertyValue] : entry.m_properties)
        {
            DoIntendation();
            m_stream << "\"" << propertyKey << "\" \"" << propertyValue << "\"\n";
        }

        m_intendation_level--;
        m_stream << "}\n";
    }

public:
    explicit GdtWriter(std::ostream& stream)
        : m_stream(stream),
          m_intendation_level(0)
    {
    }

    void Write(const Gdt& gdt)
    {
        m_stream << "{\n";
        m_intendation_level++;

        WriteVersion(gdt.m_version);
        for (const auto& entry : gdt.m_entries)
        {
            WriteEntry(*entry);
        }

        m_intendation_level--;
        m_stream << "}";
    }
};

bool Gdt::ReadFromStream(std::istream& stream)
{
    GdtReader reader(stream);
    return reader.Read(*this);
}

void Gdt::WriteToStream(std::ostream& stream) const
{
    GdtWriter writer(stream);
    writer.Write(*this);
}
