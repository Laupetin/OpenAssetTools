#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Zone/Definition/ZoneDefinition.h"

#include <optional>
#include <string>
#include <vector>

class CommonKeyValuePair
{
public:
    CommonKeyValuePair(std::string keyStr, std::string value);
    CommonKeyValuePair(unsigned keyHash, std::string value);

    std::optional<std::string> m_key_str;
    std::optional<unsigned> m_key_hash;
    std::string m_value;
};

class KeyValuePairsCreator final : public IZoneAssetCreationState
{
public:
    void AddKeyValuePair(CommonKeyValuePair keyValuePair);
    void Finalize(const ZoneDefinition& zoneDefinition);
    std::vector<CommonKeyValuePair> GetFinalKeyValuePairs();

private:
    std::vector<CommonKeyValuePair> m_key_value_pairs;
};
