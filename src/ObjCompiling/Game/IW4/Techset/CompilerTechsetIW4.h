#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "StateMap/StateMapDefinition.h"
#include "Techset/TechsetDefinition.h"
#include "Utils/MemoryManager.h"

#include <memory>
#include <string>

namespace techset
{
    class ICreatorIW4 : public AssetCreator<IW4::AssetTechniqueSet>
    {
    public:
        ICreatorIW4() = default;
        virtual ~ICreatorIW4() = default;

        virtual TechsetDefinition* LoadTechsetDefinition(const std::string& assetName, AssetCreationContext& context, bool& failure) = 0;
        virtual const state_map::StateMapDefinition* LoadStateMapDefinition(const std::string& stateMapName, AssetCreationContext& context) = 0;
    };

    std::unique_ptr<ICreatorIW4> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace techset
