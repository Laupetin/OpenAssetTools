#include "MapEntsLinker.h"

#include "Game/T6/BSP/BSPUtil.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

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
                if (className != "worldspawn")
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

    std::string loadMapEnts() {}
} // namespace

namespace BSP
{
    MapEntsLinker::MapEntsLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    MapEnts* MapEntsLinker::linkMapEnts(const BSPData& bsp) const
    {
        try
        {
            json entJs;
            const auto entityFilePath = BSPUtil::getFileNameForBSPAsset("entities.json");
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
                return nullptr;

            json spawnJs;
            const auto spawnFilePath = BSPUtil::getFileNameForBSPAsset("spawns.json");
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

            constexpr auto defenderNameCount = std::extent_v<decltype(BSPGameConstants::DEFENDER_SPAWN_POINT_NAMES)>;
            constexpr auto attackerNameCount = std::extent_v<decltype(BSPGameConstants::ATTACKER_SPAWN_POINT_NAMES)>;
            constexpr auto ffaNameCount = std::extent_v<decltype(BSPGameConstants::FFA_SPAWN_POINT_NAMES)>;

            parseSpawnpointJSON(spawnJs["attackers"], entityString, BSPGameConstants::DEFENDER_SPAWN_POINT_NAMES, defenderNameCount);
            parseSpawnpointJSON(spawnJs["defenders"], entityString, BSPGameConstants::ATTACKER_SPAWN_POINT_NAMES, attackerNameCount);
            parseSpawnpointJSON(spawnJs["FFA"], entityString, BSPGameConstants::FFA_SPAWN_POINT_NAMES, ffaNameCount);

            MapEnts* mapEnts = m_memory.Alloc<MapEnts>();
            mapEnts->name = m_memory.Dup(bsp.bspName.c_str());

            mapEnts->entityString = m_memory.Dup(entityString.c_str());
            mapEnts->numEntityChars = static_cast<int>(entityString.length() + 1); // numEntityChars includes the null character

            // don't need these
            mapEnts->trigger.count = 0;
            mapEnts->trigger.models = nullptr;
            mapEnts->trigger.hullCount = 0;
            mapEnts->trigger.hulls = nullptr;
            mapEnts->trigger.slabCount = 0;
            mapEnts->trigger.slabs = nullptr;

            return mapEnts;
        }
        catch (const json::exception& e)
        {
            con::error("JSON error when parsing map ents and spawns: {}", e.what());
            return nullptr;
        }
    }
} // namespace BSP
