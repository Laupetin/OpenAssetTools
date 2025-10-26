#include "MapEntsLinker.h"
#include "../BSPUtil.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

namespace
{
    bool parseMapEntsJSON(json& entArrayJs, std::string& entityString)
    {
        for (size_t entIdx = 0; entIdx < entArrayJs.size(); entIdx++)
        {
            auto& entity = entArrayJs[entIdx];

            if (entIdx == 0)
            {
                std::string className;
                entity.at("classname").get_to(className);
                if (className.compare("worldspawn") != 0)
                {
                    con::error("ERROR: first entity in the map entity string must be the worldspawn class!");
                    return false;
                }
            }

            entityString.append("{\n");

            for (auto& element : entity.items())
            {
                std::string key = element.key();
                std::string value = element.value();
                entityString.append(std::format("\"{}\" \"{}\"\n", key, value));
            }

            entityString.append("}\n");
        }

        return true;
    }

    void parseSpawnpointJSON(json& entArrayJs, std::string& entityString, const char* spawnpointNames[], size_t nameCount)
    {
        for (auto& element : entArrayJs.items())
        {
            std::string origin;
            std::string angles;
            auto& entity = element.value();
            entity.at("origin").get_to(origin);
            entity.at("angles").get_to(angles);

            for (size_t nameIdx = 0; nameIdx < nameCount; nameIdx++)
            {
                entityString.append("{\n");
                entityString.append(std::format("\"origin\" \"{}\"\n", origin));
                entityString.append(std::format("\"angles\" \"{}\"\n", angles));
                entityString.append(std::format("\"classname\" \"{}\"\n", spawnpointNames[nameIdx]));
                entityString.append("}\n");
            }
        }
    }

    std::string loadMapEnts()
    {

    }
}

namespace BSP
{
    MapEntsLinker::MapEntsLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
        m_search_path(searchPath),
        m_context(context)
    {
    }

    AssetCreationResult MapEntsLinker::linkMapEnts(BSPData* bsp)
    {
        try
        {
            json entJs;
            std::string entityFilePath = BSPUtil::getFileNameForBSPAsset("entities.json");
            const auto entFile = m_search_path.Open(entityFilePath);
            if (!entFile.IsOpen())
            {
                con::warn("Can't find entity file {}, using default entities instead", entityFilePath);
                entJs = json::parse(BSPLinkingConstants::DEFAULT_MAP_ENTS_STRING);
            }
            else
            {
                entJs = json::parse(*entFile.m_stream);
            }
            std::string entityString;
            if (!parseMapEntsJSON(entJs["entities"], entityString))
                return AssetCreationResult::Failure();

            json spawnJs;
            std::string spawnFilePath = BSPUtil::getFileNameForBSPAsset("spawns.json");
            const auto spawnFile = m_search_path.Open(spawnFilePath);
            if (!spawnFile.IsOpen())
            {
                con::warn("Cant find spawn file {}, setting spawns to 0 0 0", spawnFilePath);
                spawnJs = json::parse(BSPLinkingConstants::DEFAULT_SPAWN_POINT_STRING);
            }
            else
            {
                spawnJs = json::parse(*spawnFile.m_stream);
            }
            size_t defenderNameCount = std::extent<decltype(BSPGameConstants::DEFENDER_SPAWN_POINT_NAMES)>::value;
            size_t attackerNameCount = std::extent<decltype(BSPGameConstants::ATTACKER_SPAWN_POINT_NAMES)>::value;
            size_t ffaNameCount = std::extent<decltype(BSPGameConstants::FFA_SPAWN_POINT_NAMES)>::value;
            parseSpawnpointJSON(spawnJs["attackers"], entityString, BSPGameConstants::DEFENDER_SPAWN_POINT_NAMES, defenderNameCount);
            parseSpawnpointJSON(spawnJs["defenders"], entityString, BSPGameConstants::ATTACKER_SPAWN_POINT_NAMES, attackerNameCount);
            parseSpawnpointJSON(spawnJs["FFA"], entityString, BSPGameConstants::FFA_SPAWN_POINT_NAMES, ffaNameCount);

            MapEnts* mapEnts = m_memory.Alloc<MapEnts>();
            mapEnts->name = m_memory.Dup(bsp->bspName.c_str());

            mapEnts->entityString = m_memory.Dup(entityString.c_str());
            mapEnts->numEntityChars = entityString.length() + 1; // numEntityChars includes the null character

            // don't need these
            mapEnts->trigger.count = 0;
            mapEnts->trigger.models = nullptr;
            mapEnts->trigger.hullCount = 0;
            mapEnts->trigger.hulls = nullptr;
            mapEnts->trigger.slabCount = 0;
            mapEnts->trigger.slabs = nullptr;

            auto mapEntsAsset = m_context.AddAsset<AssetMapEnts>(mapEnts->name, mapEnts);
            return AssetCreationResult::Success(mapEntsAsset);
        }
        catch (const json::exception& e)
        {
            con::error("JSON error when parsing map ents and spawns: {}", e.what());
            return AssetCreationResult::Failure();
        }
    }
}

