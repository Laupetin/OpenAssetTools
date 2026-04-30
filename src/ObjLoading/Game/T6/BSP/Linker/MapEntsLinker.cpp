#include "MapEntsLinker.h"

#include "../BSPUtil.h"

namespace
{
    inline const std::vector<const char*> DEFENDER_SPAWN_POINT_NAMES = {"mp_ctf_spawn_allies",
                                                                        "mp_ctf_spawn_allies_start",
                                                                        "mp_sd_spawn_defender",
                                                                        "mp_dom_spawn_allies_start",
                                                                        "mp_dem_spawn_defender_start",
                                                                        "mp_dem_spawn_defenderOT_start",
                                                                        "mp_dem_spawn_defender",
                                                                        "mp_tdm_spawn_allies_start",
                                                                        "mp_tdm_spawn_team1_start",
                                                                        "mp_tdm_spawn_team2_start",
                                                                        "mp_tdm_spawn_team3_start"};

    inline const std::vector<const char*> ATTACKER_SPAWN_POINT_NAMES = {"mp_ctf_spawn_axis",
                                                                        "mp_ctf_spawn_axis_start",
                                                                        "mp_sd_spawn_attacker",
                                                                        "mp_dom_spawn_axis_start",
                                                                        "mp_dem_spawn_attacker_start",
                                                                        "mp_dem_spawn_attackerOT_start",
                                                                        "mp_dem_spawn_attacker",
                                                                        "mp_tdm_spawn_axis_start",
                                                                        "mp_tdm_spawn_team4_start",
                                                                        "mp_tdm_spawn_team5_start",
                                                                        "mp_tdm_spawn_team6_start"};

    inline const std::vector<const char*> FFA_SPAWN_POINT_NAMES = {"mp_tdm_spawn", "mp_dm_spawn", "mp_dom_spawn"};

    void addSpawnsToEntString(BSP::BSPData* bsp, std::string& entityString)
    {
        if (bsp->isZombiesMap)
        {
            for (auto& spawnPoint : bsp->spawnpoints)
            {
                entityString.append("{\n");
                entityString.append("\"classname\" \"script_struct\"\n");
                entityString.append(std::format("\"targetname\" \"{}\"\n", spawnPoint.spawnpointGroupName));
                entityString.append(std::format("\"origin\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(spawnPoint.origin)));
                vec3_t angles = BSP::BSPUtil::convertForwardVectorToViewAngles(spawnPoint.forward);
                entityString.append(std::format("\"angles\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(angles)));
                entityString.append("}\n");
            }
            return;
        }

        size_t defenderNameCount = std::extent<decltype(DEFENDER_SPAWN_POINT_NAMES)>::value;
        size_t attackerNameCount = std::extent<decltype(ATTACKER_SPAWN_POINT_NAMES)>::value;
        size_t ffaNameCount = std::extent<decltype(FFA_SPAWN_POINT_NAMES)>::value;

        for (auto& spawnPoint : bsp->spawnpoints)
        {
            vec3_t origin = spawnPoint.origin;
            vec3_t angles = BSP::BSPUtil::convertForwardVectorToViewAngles(spawnPoint.forward);

            std::string originStr = std::format("\"origin\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(origin));
            std::string anglesStr = std::format("\"angles\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(angles));

            const std::vector<const char*>* spawnPointList;
            if (!spawnPoint.spawnpointGroupName.compare("defender"))
                spawnPointList = &DEFENDER_SPAWN_POINT_NAMES;
            else if (!spawnPoint.spawnpointGroupName.compare("attacker"))
                spawnPointList = &ATTACKER_SPAWN_POINT_NAMES;
            else // SPAWNPOINT_TYPE_ALL
                spawnPointList = &FFA_SPAWN_POINT_NAMES;

            for (const char* spawnName : *spawnPointList)
            {
                entityString.append("{\n");
                entityString.append(originStr);
                entityString.append(anglesStr);
                entityString.append(std::format("\"classname\" \"{}\"\n", spawnName));
                entityString.append("}\n");
            }
        }
    }

    void addZombiesEntitiesToEntString(BSP::BSPData* bsp, std::string& entityString)
    {
        for (auto& zone : bsp->zm_zones)
        {
            entityString.append("{\n");
            entityString.append("\"classname\" \"info_volume\"\n");
            entityString.append("\"script_noteworthy\" \"player_volume\"\n");
            entityString.append(std::format("\"targetname\" \"{}\"\n", zone.zoneName));
            entityString.append(std::format("\"target\" \"{}\"\n", zone.spawnerGroupName));
            entityString.append(std::format("\"origin\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(zone.origin)));
            entityString.append(std::format("\"model\" \"*{}\"\n", zone.modelIndex));
            entityString.append("}\n");

            entityString.append("{\n");
            entityString.append("\"classname\" \"script_struct\"\n");
            entityString.append(std::format("\"targetname\" \"player_respawn_point\"\n"));
            entityString.append(std::format("\"script_noteworthy\" \"{}\"\n", zone.zoneName));
            entityString.append(std::format("\"target\" \"{}\"\n", zone.spawnpointGroupName));
            entityString.append("}\n");
        }

        for (auto& zSpawner : bsp->zm_spawners)
        {
            entityString.append("{\n");
            entityString.append("\"classname\" \"script_struct\"\n");
            entityString.append("\"script_noteworthy\" \"riser_location\"\n");
            entityString.append("\"script_string\" \"find_flesh\"\n");
            entityString.append(std::format("\"targetname\" \"{}\"\n", zSpawner.spawnerGroupName));
            entityString.append(std::format("\"origin\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(zSpawner.origin)));
            vec3_t angles = BSP::BSPUtil::convertForwardVectorToViewAngles(zSpawner.forward);
            entityString.append(std::format("\"angles\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(angles)));
            entityString.append("}\n");
        }
    }

    void addClassEntitiesToEntString(BSP::BSPData* bsp, std::string& entityString)
    {
        for (auto& entity : bsp->entities)
        {
            vec3_t origin = entity.origin;
            vec3_t axis[3];
            BSP::BSPUtil::convertQuaternionToAxis(&entity.rotationQuaternion, axis);
            vec3_t angles = BSP::BSPUtil::convertAxisToAngles(axis);

            entityString.append("{\n");
            entityString.append(std::format("\"origin\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(origin)));

            for (auto& entry : entity.entries)
                entityString.append(std::format("\"{}\" \"{}\"\n", entry.key, entry.value));
            if (entity.modelIndex != 0)
                entityString.append(std::format("\"model\" \"*{}\"\n", entity.modelIndex));
            else // entities with generated models can't have rotation data
                entityString.append(std::format("\"angles\" \"{}\"\n", BSP::BSPUtil::convertVec3ToString(angles)));

            entityString.append("}\n");
        }
    }
} // namespace

namespace BSP
{
    MapEntsLinker::MapEntsLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    MapEnts* MapEntsLinker::linkMapEnts(BSPData* bsp)
    {
        std::string entityString;

        // worldspawn must be the first entity
        entityString.append("{\n");
        for (auto& entry : bsp->worldspawn.entries)
            entityString.append(std::format("\"{}\" \"{}\"\n", entry.key, entry.value));
        entityString.append("}\n");

        addSpawnsToEntString(bsp, entityString);

        if (bsp->isZombiesMap)
            addZombiesEntitiesToEntString(bsp, entityString);

        addClassEntitiesToEntString(bsp, entityString);

        MapEnts* mapEnts = m_memory.Alloc<MapEnts>();
        mapEnts->name = m_memory.Dup(bsp->bspName.c_str());

        mapEnts->entityString = m_memory.Dup(entityString.c_str());
        mapEnts->numEntityChars = static_cast<int>(entityString.length() + 1); // numEntityChars includes the null character

        return mapEnts;
    }
} // namespace BSP
