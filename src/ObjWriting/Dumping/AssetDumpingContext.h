#pragma once

#include "IZoneAssetDumperState.h"
#include "Obj/Gdt/GdtStream.h"
#include "SearchPath/IOutputPath.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/ProgressCallback.h"
#include "Zone/Zone.h"

#include <memory>
#include <ostream>
#include <string>
#include <typeindex>

class AssetDumpingContext
{
public:
    AssetDumpingContext(const Zone& zone,
                        const std::string& basePath,
                        IOutputPath& outputPath,
                        ISearchPath& objSearchPath,
                        std::optional<std::unique_ptr<ProgressCallback>> progressCallback);

    [[nodiscard]] std::unique_ptr<std::ostream> OpenAssetFile(const std::string& fileName) const;

    void IncrementProgress();
    void SetTotalProgress(size_t value);
    [[nodiscard]] bool ShouldTrackProgress() const;

    template<typename T> T* GetZoneAssetDumperState()
    {
        static_assert(std::is_base_of_v<IZoneAssetDumperState, T>, "T must inherit IZoneAssetDumperState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_asset_dumper_states.find(typeid(T));
        if (foundEntry != m_zone_asset_dumper_states.end())
            return dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(m_zone);
        auto* newStatePtr = newState.get();
        m_zone_asset_dumper_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneAssetDumperState>>(typeid(T), std::move(newState)));
        return newStatePtr;
    }

    const Zone& m_zone;
    const std::string& m_base_path;
    IOutputPath& m_output_path;
    ISearchPath& m_obj_search_path;
    std::unique_ptr<GdtOutputStream> m_gdt;

private:
    std::unordered_map<std::type_index, std::unique_ptr<IZoneAssetDumperState>> m_zone_asset_dumper_states;
    std::unique_ptr<ProgressCallback> m_progress_callback;
    size_t m_current_progress;
    size_t m_total_progress;
};
