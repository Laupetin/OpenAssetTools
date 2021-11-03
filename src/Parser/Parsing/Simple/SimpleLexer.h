#pragma once

#include <cstdint>
#include <limits>
#include <memory>

#include "SimpleParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

class SimpleLexer : public AbstractLexer<SimpleParserValue>
{
public:
    class Config
    {
    public:
        class MultiCharacterToken
        {
        public:
            int m_id;
            std::string m_value;

            MultiCharacterToken(int id, std::string value);
        };

        bool m_emit_new_line_tokens;
        bool m_read_strings;
        bool m_read_numbers;
        std::vector<MultiCharacterToken> m_multi_character_tokens;
    };

protected:
    class MultiCharacterTokenLookupEntry
    {
    public:
        int m_id;
        std::string m_value;
        std::unique_ptr<MultiCharacterTokenLookupEntry> m_next;

        MultiCharacterTokenLookupEntry(int id, std::string value);
    };

    Config m_config;
    bool m_check_for_multi_character_tokens;
    int m_last_line;

    std::unique_ptr<MultiCharacterTokenLookupEntry> m_multi_character_token_lookup[std::numeric_limits<uint8_t>::max() + 1];

    void AddMultiCharacterTokenConfigToLookup(Config::MultiCharacterToken tokenConfig);
    bool ReadMultiCharacterToken(const MultiCharacterTokenLookupEntry* multiTokenLookup);

    SimpleParserValue GetNextToken() override;

public:
    explicit SimpleLexer(IParserLineStream* stream);
    SimpleLexer(IParserLineStream* stream, Config config);
};