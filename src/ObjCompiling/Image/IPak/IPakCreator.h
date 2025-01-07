#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "KeyValuePairs/KeyValuePairsCreator.h"
#include "SearchPath/IOutputPath.h"
#include "SearchPath/ISearchPath.h"

#include <memory>
#include <string>
#include <unordered_map>

class IPakToCreate
{
public:
    explicit IPakToCreate(std::string name);

    void AddImage(std::string imageName);
    void Build(ISearchPath& searchPath, IOutputPath& outPath);
    [[nodiscard]] const std::vector<std::string>& GetImageNames() const;

private:
    std::string m_name;
    std::vector<std::string> m_image_names;
};

class IPakCreator final : public IZoneAssetCreationState
{
public:
    IPakCreator();

    void Inject(ZoneAssetCreationInjection& inject) override;

    IPakToCreate* GetOrAddIPak(const std::string& ipakName);
    void Finalize(ISearchPath& searchPath, IOutputPath& outPath);

private:
    KeyValuePairsCreator* m_kvp_creator;
    std::unordered_map<std::string, IPakToCreate*> m_ipak_lookup;
    std::vector<std::unique_ptr<IPakToCreate>> m_ipaks;
};
