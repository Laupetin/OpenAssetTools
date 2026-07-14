#include "RawLoaderPhysPresetT4.h"

#include "Game/T4/ObjConstantsT4.h"
#include "Game/T4/T4.h"
#include "PhysPreset/AbstractPhysPresetLoader.h"

#include <string>

using namespace T4;

namespace
{
    class RawLoaderPhysPreset final : public phys_preset::AbstractPhysPresetLoader<AssetPhysPreset>
    {
    public:
        using AbstractPhysPresetLoader::AbstractPhysPresetLoader;

    protected:
        bool FillFromTree(PhysPreset& physPreset, const std::string& assetName, PhysPresetParser::PhysPresetContext* tree) override
        {
            physPreset.name = m_memory.Dup(assetName.c_str());

            float friction = 0.0f;
            bool isFrictionInfinity = false;

            for (auto* pair : tree->pair())
            {
                const auto key = pair->key()->getText();
                const auto value = pair->value()->getText();

                if (key == "mass")
                    physPreset.mass = std::stof(value);
                else if (key == "bounce")
                    physPreset.bounce = std::stof(value);
                else if (key == "friction")
                    friction = std::stof(value);
                else if (key == "isFrictionInfinity")
                    isFrictionInfinity = value != "0";
                else if (key == "bulletForceScale")
                    physPreset.bulletForceScale = std::stof(value);
                else if (key == "explosiveForceScale")
                    physPreset.explosiveForceScale = std::stof(value);
                else if (key == "sndAliasPrefix")
                    physPreset.sndAliasPrefix = m_memory.Dup(value.c_str());
                else if (key == "piecesSpreadFraction")
                    physPreset.piecesSpreadFraction = std::stof(value);
                else if (key == "piecesUpwardVelocity")
                    physPreset.piecesUpwardVelocity = std::stof(value);
                else if (key == "canFloat")
                    physPreset.canFloat = std::stoi(value);
                else if (key == "gravityScale")
                    physPreset.gravityScale = std::stof(value);
            }

            physPreset.friction = isFrictionInfinity ? MAX_FRICTION : friction;

            return true;
        }
    };
} // namespace

namespace phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateRawLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysPreset>(memory, searchPath);
    }
} // namespace phys_preset
