#pragma once

#include "Game/IGame.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/ZoneTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class ZoneDefinitionObjContainerType : uint8_t
{
    IWD,
    IPAK
};

class ZoneDefinitionObjContainer
{
public:
    std::string m_name;
    ZoneDefinitionObjContainerType m_type;

    /**
     * \brief The first asset (inclusive) to include in the obj container.
     */
    unsigned m_asset_start;

    /**
     * \brief The last asset (exclusive) to include in the obj container.
     */
    unsigned m_asset_end;

    ZoneDefinitionObjContainer(std::string name, ZoneDefinitionObjContainerType type, unsigned start);
};

class ZoneDefinitionAsset
{
public:
    ZoneDefinitionAsset(asset_type_t type, std::string name, bool isReference);

    asset_type_t m_asset_type;
    std::string m_asset_name;
    bool m_is_reference;
};

class ZoneDefinitionProperties
{
public:
    ZoneDefinitionProperties();

    void AddProperty(std::string key, std::string value);
    void Include(const ZoneDefinitionProperties& otherProperties);

    std::unordered_multimap<std::string, std::string> m_properties;
};

class ZoneDefinition
{
public:
    ZoneDefinition();

    void Include(const AssetList& assetListToInclude);
    void Include(const ZoneDefinition& definitionToInclude);

    std::string m_name;
    GameId m_game;
    ZoneDefinitionProperties m_properties;
    std::vector<std::string> m_includes;
    std::vector<std::string> m_asset_lists;
    std::vector<std::string> m_ignores;
    std::vector<std::string> m_targets_to_build;
    std::vector<std::string> m_gdts;
    std::vector<ZoneDefinitionAsset> m_assets;
    std::vector<ZoneDefinitionObjContainer> m_obj_containers;
};
