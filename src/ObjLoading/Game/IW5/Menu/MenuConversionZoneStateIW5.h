#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Game/IW5/IW5.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace IW5
{
    class MenuConversionZoneState final : public IZoneAssetCreationState
    {
    public:
        MenuConversionZoneState();
        void Inject(ZoneAssetCreationInjection& inject) override;

        Statement_s* FindFunction(const std::string& functionName);

        Statement_s* AddFunction(const std::string& functionName, Statement_s* function);
        size_t AddStaticDvar(const std::string& dvarName);
        const char* AddString(const std::string& str);

        void AddLoadedFile(std::string loadedFileName, std::vector<XAssetInfo<menuDef_t>*> menusOfFile);

        void FinalizeSupportingData() const;

        std::unordered_map<std::string, std::vector<XAssetInfo<menuDef_t>*>> m_menus_by_filename;
        ExpressionSupportingData* m_supporting_data;

    private:
        Zone* m_zone;
        std::vector<Statement_s*> m_functions;
        std::unordered_map<std::string, Statement_s*> m_function_by_name;

        std::vector<StaticDvar*> m_static_dvars;
        std::unordered_map<std::string, size_t> m_dvars_by_name;

        std::vector<const char*> m_strings;
        std::unordered_map<std::string, const char*> m_strings_by_value;
    };
} // namespace IW5
