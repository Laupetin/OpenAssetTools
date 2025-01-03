#include "KeyValuePairsCreator.h"

#include <format>
#include <iostream>

CommonKeyValuePair::CommonKeyValuePair(std::string keyStr, std::string value)
    : m_key_str(std::move(keyStr)),
      m_value(std::move(value))
{
}

CommonKeyValuePair::CommonKeyValuePair(const unsigned keyHash, std::string value)
    : m_key_hash(keyHash),
      m_value(std::move(value))
{
}

void KeyValuePairsCreator::AddKeyValuePair(CommonKeyValuePair keyValuePair)
{
    m_key_value_pairs.emplace_back(std::move(keyValuePair));
}

void KeyValuePairsCreator::Finalize(const ZoneDefinition& zoneDefinition)
{
    for (const auto& metaData : zoneDefinition.m_properties.m_properties)
    {
        if (metaData.first.rfind("level.", 0) == 0)
        {
            std::string strValue = metaData.first.substr(std::char_traits<char>::length("level."));
            if (strValue.empty())
                continue;

            if (strValue[0] == '@')
            {
                char* endPtr;
                const unsigned keyHash = strtoul(&strValue[1], &endPtr, 16);

                if (endPtr != &strValue[strValue.size()])
                {
                    std::cerr << std::format("Could not parse metadata key \"{}\" as hash\n", metaData.first);
                    continue;
                }

                m_key_value_pairs.emplace_back(keyHash, metaData.second);
            }
            else
            {
                m_key_value_pairs.emplace_back(std::move(strValue), metaData.second);
            }
        }
    }
}

std::vector<CommonKeyValuePair> KeyValuePairsCreator::GetFinalKeyValuePairs()
{
    return std::move(m_key_value_pairs);
}
