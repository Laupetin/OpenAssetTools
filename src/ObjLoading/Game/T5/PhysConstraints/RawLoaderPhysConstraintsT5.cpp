#include "RawLoaderPhysConstraintsT5.h"

#include "Game/T5/ObjConstantsT5.h"
#include "Game/T5/T5.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysConstraintsT5.h"
#include "PhysConstraints/PhysConstraintsCommon.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T5;

namespace
{
    class RawLoaderPhysConstraints final : public AssetCreator<AssetPhysConstraints>
    {
    public:
        RawLoaderPhysConstraints(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = phys_constraints::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_PHYS_CONSTRAINTS, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        phys_constraints::InfoStringLoaderT5 m_info_string_loader;
    };
} // namespace

namespace phys_constraints
{
    std::unique_ptr<AssetCreator<AssetPhysConstraints>> CreateRawLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysConstraints>(memory, searchPath, zone);
    }
} // namespace phys_constraints
